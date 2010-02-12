#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "SDL.h"

#include "xyz.h"
#include "pizza.h"

void init(void) {
  xyz_start();
  xyz_set_up_screen(TOTAL_HEIGHT, TOTAL_WIDTH);

  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

  load_toppings();
}

void draw(void) {
  xyz_color(0, 0, 0);
  xyz_fill();
  draw_toppings();
  xyz_done_drawing();
}

void shutdown(void) {
  free_toppings();
}

/************ Mouse processing **************/

static xyz_sprite* selected_sprite = NULL;

void mouse_button_down(int button, int x, int y) {
  if(button == 1) {
    selected_sprite = xyz_intersect_draggable_sprite(x, y);
  }
  /* button 3 is right button */
}

void mouse_button_up(int button, int x, int y) {
  if(button == 1) {
    selected_sprite = NULL;
  }
  /* button 3 is right button */
}

void process_mouse(void) {
  int x, y;

  SDL_GetMouseState(&x, &y);

  if(selected_sprite) {
    xyz_sprite_set_x(selected_sprite, x);
    xyz_sprite_set_y(selected_sprite, y);
  }
}

/**************** Main loop ****************/

void process_event(SDL_Event *eventp) {
  switch(eventp->type) {
  case SDL_KEYDOWN: {
    const char *keyname = SDL_GetKeyName(eventp->key.keysym.sym);
    if(!strcasecmp(keyname, "q")) {
      exit(0);
    }
    /* printf("The %s key was pressed!\n", keyname); */
    break;
  }
  case SDL_MOUSEBUTTONDOWN:
    mouse_button_down(eventp->button.button, eventp->button.x,
		      eventp->button.y);
    break;
  case SDL_MOUSEBUTTONUP:
    mouse_button_up(eventp->button.button, eventp->button.x,
		    eventp->button.y);
    break;
  case SDL_QUIT:
    exit(0);
    break;
  }
}

void main_loop(void) {
  SDL_Event event;

  while(1) {
    int pending;
    do {
      pending = SDL_PollEvent(&event);

      if(pending) {
	process_event(&event);
      }
    } while(pending);

    process_mouse();

    draw();
    usleep(50);
  }

}

int main(int argc, char** argv) {
    init();

    main_loop();

    shutdown();
    xyz_end();

    return 0;
}
