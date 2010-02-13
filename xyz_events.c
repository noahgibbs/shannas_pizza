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

static SDL_Event *handler_event = NULL;

static void buttondown_handler(xyz_sprite *sprite) {
  xyz_sprite_event *event = xyz_sprite_event_new(sprite);
  event->type = XYZ_SPRITE_BUTTONDOWN;
  event->button = handler_event->button.button;
  event->mouse_x = handler_event->button.x;
  event->mouse_y = handler_event->button.y;
  xyz_sprite_handle_event(sprite, event);
}

static void buttonup_handler(xyz_sprite *sprite) {
  xyz_sprite_event *event = xyz_sprite_event_new(sprite);
  event->type = XYZ_SPRITE_BUTTONUP;
  event->button = handler_event->button.button;
  event->mouse_x = handler_event->button.x;
  event->mouse_y = handler_event->button.y;
  xyz_sprite_handle_event(sprite, event);
}

static void (*xyz_key_handler)(const char *key_name, int down);

void xyz_set_key_handler(void (*key_handler)(const char *key_name, int down)) {
  xyz_key_handler = key_handler;
}

static xyz_sprite* dragged_sprite = NULL;
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
      handler_event = eventp;
      xyz_intersect_event_sprite(x, y, XYZ_SPRITE_BUTTONDOWN,
				 &buttondown_handler);
    }
    if(button == 1) {
      dragged_sprite = xyz_intersect_draggable_sprite(x, y);
      if(dragged_sprite) {
	selected_x_offset = x - xyz_sprite_get_x(dragged_sprite);
	selected_y_offset = y - xyz_sprite_get_y(dragged_sprite);
      }
    }
    break;
  }
  case SDL_MOUSEBUTTONUP: {
    int button = eventp->button.button;
    int x = eventp->button.x;
    int y = eventp->button.y;
    if(button >= 1 && button <= 3) {
      mouse_button[button] = 0;
      handler_event = eventp;
      xyz_intersect_event_sprite(x, y, XYZ_SPRITE_BUTTONUP,
				 &buttonup_handler);
    }
    if(button == 1) dragged_sprite = NULL;
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
  if(dragged_sprite) {
    int old_x, old_y, new_x, new_y;
    old_x = xyz_sprite_get_x(dragged_sprite);
    old_y = xyz_sprite_get_y(dragged_sprite);
    new_x = x - selected_x_offset;
    new_y = y - selected_y_offset;

    if(old_x == new_x && old_y == new_y) return;

    xyz_sprite_set_x(dragged_sprite, new_x);
    xyz_sprite_set_y(dragged_sprite, new_y);
    if(xyz_sprite_subscribes_to(dragged_sprite, XYZ_SPRITE_MOVED)) {
      xyz_sprite_event *event = xyz_sprite_event_new(dragged_sprite);
      event->type = XYZ_SPRITE_MOVED;
      event->button = 1;
      event->mouse_x = x;
      event->mouse_y = y;
      xyz_sprite_handle_event(dragged_sprite, event);
    }
  }
}
