#include <stdio.h>
#include "xyz.h"
#include "xyz_sprite.h"
#include "pizza.h"

static void draw_gates(void);

/****** Sprite definitions ************/

static void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void go_button_event_handler(xyz_sprite *sprite,
				    xyz_sprite_event *event);
static void toolbox_draw(xyz_sprite *sprite);
static void toolbox_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void gate_draw(xyz_sprite *sprite);
static void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void gate_target_draw(xyz_sprite *sprite);
static void gate_target_event_handler(xyz_sprite *sprite,
				      xyz_sprite_event *event);

static xyz_sprite_methods topping_methods = { NULL, topping_event_handler };
static xyz_sprite_methods go_button_methods = { NULL, go_button_event_handler };
static xyz_sprite_methods toolbox_methods = { toolbox_draw,
					      toolbox_event_handler };
static xyz_sprite_methods gate_methods = { gate_draw, gate_event_handler };
static xyz_sprite_methods gate_target_methods = { gate_target_draw,
						  gate_target_event_handler };

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec pizzasprites[] = {
  { "images/sausage_small_white.png", 50, 425, 32, 32, 1,
    &topping_methods, EVENTS, NULL },
  { "images/pepper_small_white.png", 130, 450, 32, 32, 1,
    &topping_methods, EVENTS, NULL },
  { "images/pineapple_small_white.png", 210, 425, 32, 32, 1,
    &topping_methods, EVENTS, NULL },
  {"",
   TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT, TOTAL_WIDTH - TOOLBOX_LEFT_WIDTH,
   TOOLBOX_BOTTOM_HEIGHT - TOOLBOX_TOP_HEIGHT, 0,
   &toolbox_methods, EVENTS, NULL},
  {"",
   TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT, GATE_WIDTH, GATE_HEIGHT, 0,
   &gate_target_methods, EVENTS, (void*)GATE_TYPE_AND},
  {"",
   TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT + GATE_HEIGHT,
   GATE_WIDTH, GATE_HEIGHT, 0,
   &gate_target_methods, EVENTS, (void*)GATE_TYPE_OR},
  {"images/go_button.png",
   TOOLBOX_LEFT_WIDTH, CONVEYOR_BOTTOM_HEIGHT, 100, 50, 0,
   &go_button_methods, EVENTS, NULL},
  { NULL }
};

static xyz_sprite_spec gatesprites[] = {
  { "", 0, 0, GATE_WIDTH, GATE_HEIGHT, 1,
    &gate_methods, EVENTS, NULL },
};

static xyz_sprite *toolbox_sprite = NULL;
static xyz_image *base_gate_image = NULL;

/******* Sprite functions *************/

void load_sprites(void) {
  int idx = 0;

  base_gate_image = xyz_load_image("images/base_gate_small.png");

  while(pizzasprites[idx].filename) {
    pizzasprites[idx].sprite = xyz_sprite_from_spec(&pizzasprites[idx]);
    if(!pizzasprites[idx].sprite)
      xyz_fatal_error("Can't create sprite for '%s'!",
		      pizzasprites[idx].filename);

    if(pizzasprites[idx].methods == &toolbox_methods) {
      toolbox_sprite = pizzasprites[idx].sprite;
    }

    idx++;
  }
}

void draw_sprites(void) {
  int idx = 0;

  while(pizzasprites[idx].filename) {
    xyz_draw_sprite(pizzasprites[idx].sprite);
    idx++;
  }

  draw_gates();
}

void free_sprites(void) {
  int idx = 0;

  while(pizzasprites[idx].filename) {
    xyz_free_sprite(pizzasprites[idx].sprite);
    pizzasprites[idx].sprite = NULL;
    idx++;
  }
}

/************** Gate functions ********************/

static int num_gates = 0;
static xyz_sprite* gates[MAX_NUM_GATES];

void draw_gates(void) {
  int i = 0;

  for(i = 0; i < num_gates; i++) {
    xyz_draw_sprite(gates[i]);
  }
}

static xyz_sprite* new_gate(int x, int y, int type) {
  xyz_sprite *gate = xyz_sprite_from_spec(&gatesprites[0]);
  xyz_sprite_set_x(gate, x - GATE_WIDTH / 2);
  xyz_sprite_set_y(gate, y - GATE_HEIGHT / 2);
  xyz_sprite_set_user_info(gate, (void*)type);

  gates[num_gates] = gate;
  num_gates++;

  return gate;
}

void delete_gate(xyz_sprite *sprite) {
  int i = 0;

  while(gates[i] != sprite && i < num_gates) i++;
  if(i >= num_gates)
    xyz_fatal_error("Deleting non-existent gate sprite %p!\n", sprite);

  num_gates--;
  while(i < num_gates) {
    gates[i] = gates[i + 1];
    i++;
  }

  xyz_free_sprite(sprite);
}

/*********** Sprite event handlers ******************/

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
      if(num_gates >= MAX_NUM_GATES) {
	/* TODO: real response of some useful kind */
      } else {
	//new_gate(event->mouse_x, event->mouse_y, GATE_TYPE_AND);
      }
    }
    break;
  }
}

static void gate_draw(xyz_sprite *sprite) {
  int x = xyz_sprite_get_x(sprite);
  int y = xyz_sprite_get_y(sprite);
  int type = (int)xyz_sprite_get_user_info(sprite);

  xyz_draw_image(base_gate_image, x, y);

  switch(type) {
  case GATE_TYPE_AND:
    xyz_color(128, 128, 128);
    xyz_rectangle(x + AND_CROSSBAR_WIDTH_OFFSET, y + AND_CROSSBAR_OFF_HEIGHT,
		  AND_CROSSBAR_WIDTH, AND_CROSSBAR_HEIGHT);
    break;
  case GATE_TYPE_OR:
    xyz_color(128, 0, 180);
    xyz_rectangle(x + OR_CROSSBAR_WIDTH_OFFSET, y + OR_CROSSBAR_OFF_HEIGHT,
		  OR_CROSSBAR_WIDTH, OR_CROSSBAR_HEIGHT);
    break;
  default:
    xyz_fatal_error("Unknown gate type!");
  }
}

static void gate_target_draw(xyz_sprite *sprite) {
  int x, y, width, height;

  x = xyz_sprite_get_x(sprite);
  y = xyz_sprite_get_y(sprite);
  width = xyz_sprite_get_width(sprite);
  height = xyz_sprite_get_height(sprite);
  xyz_color(0, 80, 250);
  xyz_rectangle(x, y, width, height);
  gate_draw(sprite);
}

static void gate_target_event_handler(xyz_sprite *sprite,
				      xyz_sprite_event *event) {
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    if(event->button == 1) {
      if(num_gates >= MAX_NUM_GATES) {
	/* TODO: real response of some useful kind */
      } else {
	new_gate(event->mouse_x, event->mouse_y,
		 (int)xyz_sprite_get_user_info(sprite));
      }
    }
    break;
  }
}

static void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  /* Do we do anything special in this handler? */
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    break;
  case XYZ_SPRITE_BUTTONUP:
    if(xyz_sprite_overlap(sprite, toolbox_sprite)) {
      delete_gate(sprite);
    }
    break;
  }
}
