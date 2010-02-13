#include <stdio.h>
#include "xyz.h"
#include "xyz_sprite.h"
#include "pizza.h"

static void event_handler(xyz_sprite *sprite, xyz_sprite_event *event);

static xyz_sprite_methods methods = { NULL, event_handler };

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec pizzasprites[] = {
  { "images/sausage_small_white.png", NULL, NULL, 50, 425, 32, 32, 1,
    &methods, EVENTS },
  { "images/pepper_small_white.png", NULL, NULL, 130, 450, 32, 32, 1,
    &methods, EVENTS },
  { "images/pineapple_small_white.png", NULL, NULL, 210, 425, 32, 32, 1,
    &methods, EVENTS },
  {"images/go_button.png", NULL, NULL,
   TOOLBOX_LEFT_WIDTH, CONVEYOR_BOTTOM_HEIGHT, 100, 50, 0, &methods, EVENTS},
  { NULL }
};

void load_sprites(void) {
  int idx = 0;

  while(pizzasprites[idx].filename) {
    pizzasprites[idx].sprite = sprite_from_spec(&pizzasprites[idx]);
    if(!pizzasprites[idx].sprite)
      xyz_fatal_error("Can't create sprite for '%s'!",
		      pizzasprites[idx].filename);
    idx++;
  }
}

void draw_sprites(void) {
  int idx = 0;

  while(pizzasprites[idx].filename) {
    xyz_draw_sprite(pizzasprites[idx].sprite);
    idx++;
  }
}

void free_sprites(void) {
  int idx = 0;

  while(pizzasprites[idx].filename) {
    xyz_free_sprite(pizzasprites[idx].sprite);
    pizzasprites[idx].sprite = NULL;
    idx++;
  }
}

static void event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  printf("Sprite event handler, event %d\n", event->type);
}
