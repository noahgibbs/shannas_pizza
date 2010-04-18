#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_rotozoom.h"

#include "xyz.h"
#include "xyz_sprite.h"

static TTF_Font *sanskrit_font_20 = NULL;

#define YESNO(bool) ((bool) ? "Yes" : "No")

static void print_pixel_format(const SDL_PixelFormat *format, FILE *file) {
  fprintf(file, "== Pixel Format ==\n");
  fprintf(file, "Has palette? %s\n", YESNO(format->palette));
  fprintf(file, "Bits per pixel: %d\n", format->BitsPerPixel);
  fprintf(file, "Bytes per pixel: %d\n", format->BytesPerPixel);
  fprintf(file, "Loss RGBA: %d/%d/%d/%d\n", format->Rloss, format->Gloss,
	  format->Bloss, format->Aloss);
  fprintf(file, "Shift RGBA: %d/%d/%d/%d\n", format->Rshift, format->Gshift,
	  format->Bshift, format->Ashift);
  fprintf(file, "Mask RGBA: 0x%08x/0x%08x/0x%08x/0x%08x\n", format->Rmask,
	  format->Gmask, format->Bmask, format->Amask);
  fprintf(file, "Colorkey: %d == 0x%08x\n", format->colorkey, format->colorkey);
  fprintf(file, "Alpha: %d\n", format->alpha);
  fprintf(file, "==================\n");
}

static void print_video_info(const SDL_VideoInfo *info, FILE *file) {
  fprintf(file, "****** Video mode info ********\n");
  fprintf(file, "Hardware surface available? %s\n", YESNO(info->hw_available));
  fprintf(file, "Window Manager available? %s\n", YESNO(info->wm_available));
  fprintf(file, "Accelerated blits:\n");
  fprintf(file, "  * Hardware-to-hardware? %s\n", YESNO(info->blit_hw));
  fprintf(file, "  * Hardware colorkey? %s\n", YESNO(info->blit_hw_CC));
  fprintf(file, "  * Hardware alpha? %s\n", YESNO(info->blit_hw_A));
  fprintf(file, "  * Software-to-hardware? %s\n", YESNO(info->blit_sw));
  fprintf(file, "  * Software colorkey? %s\n", YESNO(info->blit_sw_CC));
  fprintf(file, "  * Software alpha? %s\n", YESNO(info->blit_sw_A));
  fprintf(file, "  * Color fill? %s\n", YESNO(info->blit_fill));
  fprintf(file, "Video memory: %d kB\n", info->video_mem);
  fprintf(file, "Resolution: %d x %d\n", info->current_w, info->current_h);
  fprintf(file, "Pixel format:\n");
  print_pixel_format(info->vfmt, file);
  fprintf(file, "*******************************\n");
}

static void print_rectangles(SDL_Rect **rects, FILE *file) {
  int i = 0;
  fprintf(file, "///// Returned rectangles:\n");
  while(rects[i]) {
    fprintf(file, "  %d x %d\n", rects[i]->w, rects[i]->h);
    i++;
  }
  fprintf(file, "//////////////////////////\n");
}

void xyz_start(void) {
  /* Initialize defaults, Video and Audio subsystems */
  if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)==-1)) {
    xyz_fatal_error("Could not initialize SDL: %s.\n", SDL_GetError());
  }
  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

  if(TTF_Init() == -1)
    xyz_fatal_error("Couldn't initialize SDL_ttf: %s.\n",
		    TTF_GetError());
  sanskrit_font_20 = TTF_OpenFont("resources/sanskrit.ttf", 20);
  if(!sanskrit_font_20)
    xyz_fatal_error("Couldn't open font file: %s!\n",
		    TTF_GetError());
}

void xyz_write_video_info(void) {
  const SDL_VideoInfo *vinf;
  FILE *avail_modes, *cur_mode;
  SDL_Rect **modes;

  vinf = SDL_GetVideoInfo();
  cur_mode = fopen("current_mode.txt", "wb");
  if(!cur_mode) xyz_fatal_error("Couldn't write file current_mode.txt!");
  print_video_info(vinf, cur_mode);
  fclose(cur_mode);

  avail_modes = fopen("fullscreen_modes.txt", "wb");
  if(!avail_modes) xyz_fatal_error("Couldn't write file current_mode.txt!");
  modes = SDL_ListModes(vinf->vfmt, SDL_FULLSCREEN|SDL_SWSURFACE);
  print_rectangles(modes, avail_modes);
  fclose(avail_modes);
}

static SDL_Surface *surface = NULL;

void xyz_set_up_screen_fullscreen() {
  const SDL_VideoInfo *vinf;
  /* SDL_Rect **modes; */

  if(surface) xyz_fatal_error("Screen is already set up!");

  vinf = SDL_GetVideoInfo();
  /* modes = SDL_ListModes(vinf->vfmt, SDL_FULLSCREEN|SDL_SWSURFACE); */

  surface = SDL_SetVideoMode(vinf->current_w, vinf->current_h,
			     vinf->vfmt->BitsPerPixel,
			     SDL_SWSURFACE | SDL_FULLSCREEN);
  if(!surface) {
    xyz_fatal_error("Could not set video mode: %s.\n", SDL_GetError());
  }
}

void xyz_set_up_screen_window(int width, int height) {
  const SDL_VideoInfo *vinf;

  if(surface) xyz_fatal_error("Screen is already set up!");

  vinf = SDL_GetVideoInfo();

  surface = SDL_SetVideoMode(width, height,
			     vinf->vfmt->BitsPerPixel, SDL_SWSURFACE);
  if(!surface) {
    xyz_fatal_error("Could not set video mode: %s.\n", SDL_GetError());
  }
}

void xyz_end(void) {
  /* Free custom cursor, if any */
  xyz_custom_cursor_from_file(NULL, 0, 0);

  TTF_CloseFont(sanskrit_font_20);
  sanskrit_font_20 = NULL;
  TTF_Quit();

  xyz_free_all_sprites();
  surface = NULL;  /* Freed automatically on SDL_Quit() */
  SDL_Quit();
}

const char *xyz_last_error(void) {
  return SDL_GetError();
}

static xyz_image *custom_cursor = NULL;
static int cursor_x_offset = 0;
static int cursor_y_offset = 0;

void xyz_custom_cursor_from_file(const char *filename, int x_off, int y_off) {
  if(filename && !filename[0]) filename = NULL;

  if(!filename && !custom_cursor) return;

  if(!filename) {
    xyz_free_image(custom_cursor);
    custom_cursor = NULL;
    SDL_ShowCursor(1);
    return;
  }

  custom_cursor = xyz_load_image(filename);
  cursor_x_offset = x_off;
  cursor_y_offset = y_off;
  if(!custom_cursor) xyz_fatal_error("Couldn't load cursor '%s': %s",
				     filename, SDL_GetError());
  SDL_ShowCursor(0);
}

void xyz_draw_cursor() {
  if(custom_cursor) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    xyz_draw_image(custom_cursor, x - cursor_x_offset, y - cursor_y_offset);
  }
}

/*********** xyz_image ****************/

struct _xyz_image_t {
  SDL_Surface *image;
};

xyz_image *xyz_load_image(const char *file) {
  SDL_Surface *image;
  xyz_image *tmp;

  tmp = malloc(sizeof(xyz_image));
  if(!tmp) xyz_fatal_error("Couldn't allocate image structure!");

  image = IMG_Load(file);
  if(!image) {
    xyz_fatal_error("Couldn't load %s: %s\n", file, SDL_GetError());
  }

  tmp->image = SDL_DisplayFormatAlpha(image);
  SDL_FreeSurface(image);

  return tmp;
}

int xyz_image_get_width(xyz_image *image) {
  return image->image->w;
}

int xyz_image_get_height(xyz_image *image) {
  return image->image->h;
}

void xyz_draw_image(xyz_image *image, int x, int y)
{
    SDL_Rect dest;

    /* Blit onto the screen surface.
       The surfaces should not be locked at this point.
     */
    dest.x = x;
    dest.y = y;
    dest.w = image->image->w;
    dest.h = image->image->h;
    SDL_BlitSurface(image->image, NULL, surface, &dest);
}

xyz_image *xyz_turn_image(xyz_image *source, int num_turns_clockwise) {
  xyz_image *ret = xyz_new(xyz_image);
  ret->image = rotateSurface90Degrees(source->image, 2);
  return ret;
}

void xyz_free_image(xyz_image *image) {
  /* TODO: Maybe change to track screen parts? */
  SDL_FreeSurface(image->image);
  free(image);
}

/************** Drawing Primitives ***************/

static Uint32 current_color = 0;

void xyz_done_drawing(void) {
  SDL_UpdateRect(surface, 0, 0, surface->w, surface->h);
}

void xyz_color(int r, int g, int b) {
  current_color = SDL_MapRGB(surface->format, r, g, b);
}

void xyz_color_alpha(int r, int g, int b, int alpha) {
  current_color = SDL_MapRGBA(surface->format, r, g, b, alpha);
}

void xyz_fill() {
  SDL_FillRect(surface, NULL, current_color);
}

void xyz_rectangle(int x, int y, int width, int height) {
  SDL_Rect rect;

  rect.x = x;
  rect.y = y;
  rect.w = width;
  rect.h = height;

  SDL_FillRect(surface, &rect, current_color);
}

void xyz_rectangle_coords(int x1, int y1, int x2, int y2) {
  int tmp;

  if(x2 < x1) {
    tmp = x1;
    x1 = x2;
    x2 = tmp;
  }

  if(y2 < y1) {
    tmp = y1;
    y1 = y2;
    y2 = tmp;
  }

  xyz_rectangle(x1, y1, x2 - x1, y2 - y1);
}

void xyz_block_text(int x, int y, const char *text) {
  SDL_Surface *rendered_text;
  SDL_Rect dest;
  SDL_Color color;
  Uint8 r, g, b;

  SDL_GetRGB(current_color, surface->format, &r, &g, &b);
  color.r = r;
  color.g = g;
  color.b = b;

  rendered_text = TTF_RenderText_Solid(sanskrit_font_20, text, color);
  if(!rendered_text)
    xyz_fatal_error("Couldn't render text: %s!", TTF_GetError());

  dest.x = x;
  dest.y = y;
  dest.w = surface->w;
  dest.h = surface->h;

  SDL_BlitSurface(rendered_text, NULL, surface, &dest);
  SDL_FreeSurface(rendered_text);
}

#if 0
void direct_draw(void) {
  if ( SDL_MUSTLOCK(screen) ) {
    if ( SDL_LockSurface(screen) < 0 ) {
      xyz_fatal_error("Can't lock surface!");
    }
  }

  /* Write to surface->pixels */
  /* unsigned char *pixels = xyz_get_direct_surface(blah blah); */

  if ( SDL_MUSTLOCK(screen) ) {
    SDL_UnlockSurface(screen);
  }
}
#endif
