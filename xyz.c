#include <stdio.h>
#include <stdlib.h>

#include "xyz.h"

void xyz_start(void) {
  /* Initialize defaults, Video and Audio subsystems */
  if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
    printf("Could not initialize SDL: %s.\n", SDL_GetError());
    exit(-1);
  }
}

void xyz_end(void) {
  SDL_Quit();
}
