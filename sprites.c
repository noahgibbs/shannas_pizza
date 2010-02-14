#include <stdio.h>
#include "xyz.h"
#include "xyz_sprite.h"
#include "pizza.h"

static void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void go_button_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void toolbox_draw(xyz_sprite *sprite);
static void toolbox_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void gate_draw(xyz_sprite *sprite);
static void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);

static xyz_sprite_methods topping_methods = { NULL, topping_event_handler };
static xyz_sprite_methods go_button_methods = { NULL, go_button_event_handler };
static xyz_sprite_methods toolbox_methods = { toolbox_draw, toolbox_event_handler };
static xyz_sprite_methods gate_methods = { gate_draw, gate_event_handler };

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

static xyz_sprite_spec gatesprites[] = {
  { "", NULL, NULL, 0, 0, GATE_WIDTH, GATE_HEIGHT, 1,
    &gate_methods, EVENTS },
};

static xyz_image *base_gate_image;

void load_sprites(void) {
  int idx = 0;

  base_gate_image = xyz_load_image("images/base_gate_small.png");

  while(pizzasprites[idx].filename) {
    pizzasprites[idx].sprite = xyz_sprite_from_spec(&pizzasprites[idx]);
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

  /* TODO: DRAW GATES! */
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
      xyz_sprite *gate = xyz_sprite_from_spec(&gatesprites[0]);
      xyz_sprite_set_x(gate, event->mouse_x);
      xyz_sprite_set_y(gate, event->mouse_y);
      printf("Created new gate sprite\n");
    }
    break;
  }
}


static void gate_draw(xyz_sprite *sprite) {
  printf("Draw gate!\n");
  int x = xyz_sprite_get_x(sprite);
  int y = xyz_sprite_get_y(sprite);
  xyz_draw_image(base_gate_image, x, y);
}

static void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  /* Do we do anything special in this handler? */
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    printf("Clicked on gate!\n");
    break;
  case XYZ_SPRITE_MOVED:
    printf("Dragged gate!\n");
    break;
  }
}
