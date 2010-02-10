#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_image.h"

#include "xyz.h"

struct _xyz_image_t {
  SDL_Surface *image;
};


void xyz_start(void) {
  /* Initialize defaults, Video and Audio subsystems */
  if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)==-1)) {
    xyz_fatal_error("Could not initialize SDL: %s.\n", SDL_GetError());
  }
}

static SDL_Surface *surface = NULL;
void xyz_set_up_screen(void) {
  const SDL_VideoInfo *vinf;

  vinf = SDL_GetVideoInfo();

  surface = SDL_SetVideoMode(vinf->current_w, vinf->current_h,
			     vinf->vfmt->BitsPerPixel, SDL_SWSURFACE);
  if(!surface) {
    xyz_fatal_error("Could not set video mode: %s.\n", SDL_GetError());
  }
}

void xyz_end(void) {
  SDL_Quit();
}

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
