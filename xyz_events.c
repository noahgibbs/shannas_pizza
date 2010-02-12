#include "xyz.h"
#include "SDL.h"

void xyz_mouse_position(int *x, int *y) {
  SDL_GetMouseState(x, y);
}

static int mouse_button[4] = { 0, 0, 0, 0 };

int xyz_mouse_is_button_down(int button) {
  if(button < 1 || button > 3) return 0;

  return mouse_button[button];
}

static void (*xyz_key_handler)(const char *key_name, int down);

void xyz_set_key_handler(void (*key_handler)(const char *key_name, int down)) {
  xyz_key_handler = key_handler;
}

static void xyz_process_event(SDL_Event *eventp) {
  switch(eventp->type) {
  case SDL_KEYDOWN: {
    const char *keyname = SDL_GetKeyName(eventp->key.keysym.sym);
    if(xyz_key_handler) {
      xyz_key_handler(keyname, 1);
    }
    break;
  }
  case SDL_KEYUP: {
    const char *keyname = SDL_GetKeyName(eventp->key.keysym.sym);
    if(xyz_key_handler) {
      xyz_key_handler(keyname, 0);
    }
    break;
  }
  case SDL_MOUSEBUTTONDOWN: {
    int button = eventp->button.button;
    if(button >= 1 && button <= 3) {
      mouse_button[button] = 1;
    }
    break;
  }
  case SDL_MOUSEBUTTONUP: {
    int button = eventp->button.button;
    if(button >= 1 && button <= 3) {
      mouse_button[button] = 0;
    }
    break;
  }
  case SDL_QUIT:
    exit(0);
    break;
  }
}

void xyz_process_events(void) {
  SDL_Event event;

  int pending;
  do {
    pending = SDL_PollEvent(&event);

    if(pending) {
      xyz_process_event(&event);
    }
  } while(pending);
}
