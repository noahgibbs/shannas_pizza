#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_image.h"

#include "xyz.h"

void xyz_start(void) {
  /* Initialize defaults, Video and Audio subsystems */
  if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)==-1)) {
    xyz_fatal_error("Could not initialize SDL: %s.\n", SDL_GetError());
  }
}

static SDL_Surface *surface = NULL;
void xyz_set_up_screen(int width, int height) {
  const SDL_VideoInfo *vinf;

  vinf = SDL_GetVideoInfo();

  surface = SDL_SetVideoMode(width, height,
			     vinf->vfmt->BitsPerPixel, SDL_SWSURFACE);
  if(!surface) {
    xyz_fatal_error("Could not set video mode: %s.\n", SDL_GetError());
  }
}

void xyz_end(void) {
  SDL_Quit();
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

    /* Update the changed portion of the screen */
    SDL_UpdateRects(surface, 1, &dest);
}

void xyz_free_image(xyz_image *image) {
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
