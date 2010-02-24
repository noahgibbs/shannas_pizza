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

static int (*xyz_mouse_move_handler)(int x, int y);
static int (*xyz_mouse_button_handler)(int button, int is_down);

void xyz_set_mouse_handlers(int (*mouse_move_handler)(int x, int y),
			    int (*mouse_button_handler)(int button,
							int is_down)) {
  xyz_mouse_move_handler = mouse_move_handler;
  xyz_mouse_button_handler = mouse_button_handler;
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
    int x = eventp->button.x;
    int y = eventp->button.y;

    if(xyz_mouse_button_handler) {
      if(xyz_mouse_button_handler(eventp->button.button, 1))
	break;
    }

    if(button >= 1 && button <= 3) {
      mouse_button[button] = 1;
      handler_event = eventp;
      xyz_intersect_event_sprite(x, y, XYZ_SPRITE_BUTTONDOWN,
				 &buttondown_handler);
    }

    break;
  }
  case SDL_MOUSEBUTTONUP: {
    int button = eventp->button.button;
    int x = eventp->button.x;
    int y = eventp->button.y;

    if(xyz_mouse_button_handler) {
      if(xyz_mouse_button_handler(eventp->button.button, 0))
	break;
    }

    if(button >= 1 && button <= 3) {
      mouse_button[button] = 0;
      handler_event = eventp;
      xyz_intersect_event_sprite(x, y, XYZ_SPRITE_BUTTONUP,
				 &buttonup_handler);
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
