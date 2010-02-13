#include <stdio.h>
#include "xyz.h"
#include "xyz_sprite.h"
#include "pizza.h"

static void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void go_button_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void toolbox_draw(xyz_sprite *sprite);
static void toolbox_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);

static xyz_sprite_methods topping_methods = { NULL, topping_event_handler };
static xyz_sprite_methods go_button_methods = { NULL, go_button_event_handler };
static xyz_sprite_methods toolbox_methods = { toolbox_draw, toolbox_event_handler };

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec pizzasprites[] = {
  { "images/sausage_small_white.png", NULL, NULL, 50, 425, 32, 32, 1,
    &topping_methods, EVENTS },
  { "images/pepper_small_white.png", NULL, NULL, 130, 450, 32, 32, 1,
    &topping_methods, EVENTS },
  { "images/pineapple_small_white.png", NULL, NULL, 210, 425, 32, 32, 1,
    &topping_methods, EVENTS },
  {"", NULL, NULL,
   TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT, TOTAL_WIDTH - TOOLBOX_LEFT_WIDTH,
   TOOLBOX_BOTTOM_HEIGHT - TOOLBOX_TOP_HEIGHT, 0,
   &toolbox_methods, EVENTS},
  {"images/go_button.png", NULL, NULL,
   TOOLBOX_LEFT_WIDTH, CONVEYOR_BOTTOM_HEIGHT, 100, 50, 0,
   &go_button_methods, EVENTS},
  { NULL }
};

#if 0
static xyz_sprite_spec gatesprites[] = {
  { "images/and_gate.png", NULL, NULL, 50, 425, 32, 32, 1,
    &gate_methods, EVENTS },
};
#endif

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

static void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  /* printf("Sprite event handler, event %d\n", event->type); */
}

static void go_button_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    printf("Clicked on 'GO' button, mouse button %d!\n",
	   event->button);
    break;
  }
}

static void toolbox_draw(xyz_sprite *sprite) {
  xyz_color(0, 0, 255);
  xyz_rectangle(TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT,
		TOTAL_WIDTH - TOOLBOX_LEFT_WIDTH,
		TOOLBOX_BOTTOM_HEIGHT - TOOLBOX_TOP_HEIGHT);
}

static void toolbox_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    if(event->button == 1) {
      /* Create new gate object */
    }
    break;
  }
}
