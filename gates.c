#include "xyz_sprite.h"
#include "connectors.h"
#include "pizza.h"

static void toolbox_draw(xyz_sprite *sprite);
static void gate_draw(xyz_sprite *sprite);
/* HACK: Public to check for dragging */
void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void gate_target_draw(xyz_sprite *sprite);
static void gate_target_event_handler(xyz_sprite *sprite,
				      xyz_sprite_event *event);
static void gate_connector_process(connector *conn);

static xyz_sprite_methods toolbox_methods = { toolbox_draw, NULL };
static xyz_sprite_methods gate_methods = { gate_draw, gate_event_handler };
static xyz_sprite_methods gate_target_methods = { gate_target_draw,
						  gate_target_event_handler };

static int toolbox_sprite_user_info = 0;

connector_type gate_conn_type = {
  2, 1, gate_connector_process
};

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1, \
                 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec gatefilesprites[] = {
  {"",
   TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT, TOTAL_WIDTH - TOOLBOX_LEFT_WIDTH,
   TOOLBOX_BOTTOM_HEIGHT - TOOLBOX_TOP_HEIGHT,
   &toolbox_methods, EVENTS, &toolbox_sprite_user_info},
  {"",
   TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT, GATE_WIDTH, GATE_HEIGHT,
   &gate_target_methods, EVENTS, (void*)GATE_TYPE_AND},
  {"",
   TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT + GATE_HEIGHT,
   GATE_WIDTH, GATE_HEIGHT,
   &gate_target_methods, EVENTS, (void*)GATE_TYPE_OR},
  { NULL }
};

static xyz_sprite_spec gatesprites[] = {
  { "", 0, 0, GATE_WIDTH, GATE_HEIGHT,
    &gate_methods, EVENTS, NULL, sizeof(GatePrivate) },
};

static xyz_sprite *toolbox_sprite = NULL;
static xyz_image *base_gate_image = NULL;

void init_gate_sprites(void) {
  base_gate_image = xyz_load_image("images/base_gate_small.png");

  xyz_sprites_from_specs(-1, gatefilesprites);

  toolbox_sprite = xyz_get_sprite_by_user_info(&toolbox_sprite_user_info);
}

void delete_gate_sprites(void) {

}

/************** Gate functions ********************/

static int num_gates = 0;
static xyz_sprite* gates[MAX_NUM_GATES];

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

static void gate_connector_process(connector *conn) {

}

/*********** Gate event handlers ******************/

static void toolbox_draw(xyz_sprite *sprite) {
  xyz_color(0, 0, 255);
  xyz_rectangle(TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT,
		TOTAL_WIDTH - TOOLBOX_LEFT_WIDTH,
		TOOLBOX_BOTTOM_HEIGHT - TOOLBOX_TOP_HEIGHT);
}

static void basic_gate_draw(xyz_sprite *sprite) {
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

static void gate_draw(xyz_sprite *sprite) {
  GatePrivate *priv = (GatePrivate*)xyz_sprite_get_private_data(sprite);

  draw_connector(priv->conn);

  basic_gate_draw(sprite);
}

static void gate_target_draw(xyz_sprite *sprite) {
  int x, y, width, height;

  x = xyz_sprite_get_x(sprite);
  y = xyz_sprite_get_y(sprite);
  width = xyz_sprite_get_width(sprite);
  height = xyz_sprite_get_height(sprite);
  xyz_color(0, 80, 250);
  xyz_rectangle(x, y, width, height);
  basic_gate_draw(sprite);
}

static void gate_target_event_handler(xyz_sprite *sprite,
				      xyz_sprite_event *event) {
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    if(event->button == 1) {
      if(num_gates >= MAX_NUM_GATES) {
	/* TODO: real response of some useful kind */
	xyz_fatal_error("ERROR!  NOT ENOUGH GATES!");
      } else {
	xyz_sprite *gate = new_gate(event->mouse_x, event->mouse_y,
				    (int)xyz_sprite_get_user_info(sprite));
	drag_sprite_with_offset(gate, GATE_WIDTH / 2, GATE_HEIGHT / 2);
      }
    }
    break;
  }
}

void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  GatePrivate *priv = (GatePrivate*)xyz_sprite_get_private_data(sprite);
  /* Do we do anything special in this handler? */
  switch(event->type) {
  case XYZ_SPRITE_CREATED: {
    conn_output_private *opriv = xyz_new(conn_output_private);
    conn_input_private *ipriv1 = xyz_new(conn_input_private);
    conn_input_private *ipriv2 = xyz_new(conn_input_private);
    conn_output *output;
    conn_input *input1;
    conn_input *input2;

    priv->conn = new_connector(&gate_conn_type, pizza_connector_set,
			       (void*)sprite);
    output = connector_new_output(priv->conn);
    opriv->x = GATE_WIDTH / 2;
    opriv->y = 0;
    output->user_info = (void*)opriv;

    input1 = connector_new_input(priv->conn);
    ipriv1->x = 0;
    ipriv1->y = GATE_HEIGHT;
    input1->user_info = (void*)ipriv1;

    input2 = connector_new_input(priv->conn);
    ipriv2->x = GATE_WIDTH;
    ipriv2->y = GATE_HEIGHT;
    input2->user_info = (void*)ipriv2;
    break;
  }
  case XYZ_SPRITE_DESTROYED: {
    destroy_connector(priv->conn);
    break;
  }
  case XYZ_SPRITE_BUTTONDOWN:
    break;
  case XYZ_SPRITE_BUTTONUP:
    if(xyz_sprite_overlap(sprite, toolbox_sprite)) {
      delete_gate(sprite);
    }
    break;
  }
}
