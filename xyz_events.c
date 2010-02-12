#include "xyz.h"
#include "xyz_sprite.h"
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

static xyz_sprite* selected_sprite = NULL;
static int selected_x_offset;
static int selected_y_offset;

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
    int x = eventp->button.x;
    int y = eventp->button.y;
    if(button >= 1 && button <= 3) {
      mouse_button[button] = 1;
    }
    if(button == 1) {
      selected_sprite = xyz_intersect_draggable_sprite(x, y);
      if(selected_sprite) {
	selected_x_offset = x - xyz_sprite_get_x(selected_sprite);
	selected_y_offset = y - xyz_sprite_get_y(selected_sprite);
      }
    }
    break;
  }
  case SDL_MOUSEBUTTONUP: {
    int button = eventp->button.button;
    if(button >= 1 && button <= 3) {
      mouse_button[button] = 0;
    }
    if(button == 1) selected_sprite = NULL;
    break;
  }
  case SDL_QUIT:
    exit(0);
    break;
  }
}

void xyz_process_events(void) {
  SDL_Event event;
  int x, y;

  int pending;
  do {
    pending = SDL_PollEvent(&event);

    if(pending) {
      xyz_process_event(&event);
    }
  } while(pending);

  xyz_mouse_position(&x, &y);
  if(selected_sprite) {
    xyz_sprite_set_x(selected_sprite, x - selected_x_offset);
    xyz_sprite_set_y(selected_sprite, y - selected_y_offset);
  }
}
