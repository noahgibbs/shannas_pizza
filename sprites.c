#include <stdio.h>
#include "xyz.h"
#include "xyz_sprite.h"
#include "connectors.h"
#include "pizza.h"

void draw_connector(connector *conn);

/****** Sprite definitions ************/

static void go_button_event_handler(xyz_sprite *sprite,
				    xyz_sprite_event *event);

static xyz_sprite_methods go_button_methods = { NULL, go_button_event_handler };

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1, \
                 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec pizzasprites[] = {
  {"images/go_button.png",
   TOOLBOX_LEFT_WIDTH, CONVEYOR_BOTTOM_HEIGHT, 100, 50,
   &go_button_methods, EVENTS, NULL},
  { NULL }
};

/******* Connector functions **********/

void draw_connector(connector *conn) {
  int i;
  xyz_sprite *sprite1, *sprite2;
  connector *output_conn;
  conn_input *input;
  conn_output *output;
  conn_input_private *input_private;
  conn_output_private *output_private;

  for(i = 0; i < conn->num_inputs; i++) {
    input = conn->inputs[i];

    if(input) {
      output = input->attached;
      sprite1 = (xyz_sprite*)conn->user_info;

      if(output) {
	int ix, iy;
	int ox, oy;

	input_private = (conn_input_private*)input->user_info;
	output_private = (conn_output_private*)output->user_info;
	output_conn = output->host;
	sprite2 = (xyz_sprite*)output_conn->user_info;

	ix = xyz_sprite_get_x(sprite1) + input_private->x;
	iy = xyz_sprite_get_y(sprite1) + input_private->y;
	ox = xyz_sprite_get_x(sprite2) + output_private->x;
	oy = xyz_sprite_get_y(sprite2) + output_private->y;

	xyz_color(input_private->r, input_private->g, input_private->b);
	wire_from_to(ix, iy, ox, oy);
      }
    }
  }
}

/******* Sprite functions *************/

void load_sprites(void) {
  xyz_sprites_from_specs(-1, pizzasprites);

  init_gate_sprites();
  init_toppings();
}

void draw_sprites(void) {
  xyz_draw_sprites();
}

void free_sprites(void) {
  delete_gate_sprites();
  delete_toppings();

  /* XYZ will free sprites on exit */
}

/*********** Sprite event handlers ******************/

static void go_button_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    printf("Clicked on 'GO' button, mouse button %d!\n",
	   event->button);
    break;
  }
}
