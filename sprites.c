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
  {"resources/go_button.png",
   TOOLBOX_LEFT_WIDTH, CONVEYOR_TOP_HEIGHT + CONVEYOR_HEIGHT, 100, 50,
   &go_button_methods, EVENTS, NULL},
  { NULL }
};

xyz_image *red_x_image = NULL;

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

	if(input->signal == NULL) {
	  xyz_color(128, 128, 128);
	} else if(input->signal == get_signal_one()) {
	  xyz_color(255, 0, 0);
	} else if(input->signal == get_signal_zero()) {
	  xyz_color(0, 0, 255);
	} else {
	  /* Unknown signal! */
	  xyz_color(255, 255, 255);
	}
	wire_from_to(ix, iy, ox, oy);
      }
    }
  }
}

static int filt_x, filt_y;
static conn_input *filt_input = NULL;
static conn_output *filt_output = NULL;
static xyz_sprite *filt_sprite = NULL;

/* Sprite filter to check if a coordinate hits any inputs or outputs */
int conn_sprite_filter(xyz_sprite *sprite) {
  connector *conn;
  int i, sx, sy;
  xyz_sprite_methods *methods;

  filt_sprite = sprite;

  /* Check sprite info to make sure it's a connectable type */
  methods = xyz_sprite_get_methods(sprite);
  if(methods->handle_event == topping_event_handler) {
    ToppingPrivate *tp = (ToppingPrivate*)xyz_sprite_get_private_data(sprite);
    conn = tp->conn;
  } else if (methods->handle_event == gate_event_handler) {
    GatePrivate *gp = (GatePrivate*)xyz_sprite_get_private_data(sprite);
    conn = gp->conn;
  } else if (methods->handle_event == splitter_event_handler) {
    SplitterPrivate *gp = (SplitterPrivate*)xyz_sprite_get_private_data(sprite);
    conn = gp->conn;
  } else if (methods->handle_event == judge_event_handler) {
    JudgePrivate *gp = (JudgePrivate*)xyz_sprite_get_private_data(sprite);
    conn = gp->conn;
  } else {
    return 0;
  }

  filt_input = NULL;
  filt_output = NULL;

  sx = xyz_sprite_get_x(sprite);
  sy = xyz_sprite_get_y(sprite);

  for(i = 0; i < conn->num_inputs; i++) {
    conn_input *input = conn->inputs[i];
    conn_input_private *priv = (conn_input_private*)input->user_info;

    if(xyz_point_distance(sx + priv->x, sy + priv->y, filt_x, filt_y) <
       CONNECT_RADIUS) {
      filt_input = input;
      return 1;
    }
  }
  for(i = 0; i < conn->num_outputs; i++) {
    conn_output *output = conn->outputs[i];
    conn_output_private *priv = (conn_output_private*)output->user_info;

    if(xyz_point_distance(sx + priv->x, sy + priv->y, filt_x, filt_y) <
       CONNECT_RADIUS) {
      filt_output = output;
      return 1;
    }
  }

  return 0;
}

/* See if we intersect any inputs or outputs of any particular connectors */
int intersect_connector_objects(int x, int y,
				conn_input **inputOutP,
				conn_output **outputOutP) {
  xyz_sprite *sprite;

  *inputOutP = NULL;
  *outputOutP = NULL;

  filt_x = x; filt_y = y;
  xyz_sprite_each(&conn_sprite_filter);
  sprite = filt_sprite;

  if(filt_input) {
    *inputOutP = filt_input;
    return 1;
  }

  if(filt_output) {
    *outputOutP = filt_output;
    return 1;
  }

  return 0;
}

/******* Sprite functions *************/

void load_sprites(void) {
  xyz_sprites_from_specs(-1, pizzasprites);

  init_gate_sprites();
  init_judge_sprite();
  init_toppings();

  red_x_image = xyz_load_image("resources/red_x_100.png");
}

void draw_sprites(void) {
  xyz_draw_sprites();
  roll_pizza_refresh();
}

void free_sprites(void) {
  delete_gate_sprites();
  delete_toppings();

  /* XYZ will free sprites on exit */
}

void draw_red_x(int x, int y) {
  xyz_draw_image(red_x_image, x, y);
}

/*********** Sprite event handlers ******************/

static void go_button_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    start_pizzas_rolling();
    break;
  }
}
