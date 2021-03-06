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

connector_type andor_gate_conn_type = {
  2, 1, gate_connector_process
};

connector_type not_gate_conn_type = {
  1, 1, gate_connector_process
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
  {"",
   TOOLBOX_LEFT_WIDTH, TOOLBOX_TOP_HEIGHT + 2 * GATE_HEIGHT,
   GATE_WIDTH, GATE_HEIGHT,
   &gate_target_methods, EVENTS, (void*)GATE_TYPE_NOT},
  {"",
   TOOLBOX_LEFT_WIDTH + GATE_WIDTH, TOOLBOX_TOP_HEIGHT, GATE_WIDTH, GATE_HEIGHT,
   &gate_target_methods, EVENTS, (void*)(GATE_MOD_UPSIDE_DOWN|GATE_TYPE_AND)},
  {"",
   TOOLBOX_LEFT_WIDTH + GATE_WIDTH, TOOLBOX_TOP_HEIGHT + GATE_HEIGHT,
   GATE_WIDTH, GATE_HEIGHT,
   &gate_target_methods, EVENTS, (void*)(GATE_MOD_UPSIDE_DOWN|GATE_TYPE_OR)},
  {"",
   TOOLBOX_LEFT_WIDTH + GATE_WIDTH, TOOLBOX_TOP_HEIGHT + 2 * GATE_HEIGHT,
   GATE_WIDTH, GATE_HEIGHT,
   &gate_target_methods, EVENTS, (void*)(GATE_MOD_UPSIDE_DOWN|GATE_TYPE_NOT)},
  { NULL }
};

static xyz_sprite_spec gatesprites[] = {
  { "", 0, 0, GATE_WIDTH, GATE_HEIGHT,
    &gate_methods, EVENTS, NULL, sizeof(GatePrivate) },
};

static xyz_sprite *toolbox_sprite = NULL;
static xyz_image *andor_gate_image = NULL;
static xyz_image *not_gate_image = NULL;
static xyz_image *flipped_andor_gate_image = NULL;
static xyz_image *flipped_not_gate_image = NULL;

void init_gate_sprites(void) {
  andor_gate_image = xyz_load_image("resources/base_gate_small.png");
  not_gate_image = xyz_load_image("resources/not_gate_small.png");

  flipped_andor_gate_image = xyz_turn_image(andor_gate_image, 2);
  flipped_not_gate_image = xyz_turn_image(not_gate_image, 2);

  xyz_sprites_from_specs(-1, gatefilesprites);

  toolbox_sprite = xyz_get_sprite_by_user_info(&toolbox_sprite_user_info);

  init_splitter_sprite();
}

void delete_gate_sprites(void) {

}

xyz_sprite* get_toolbox_sprite(void) {
  return toolbox_sprite;
}

/************** Gate functions ********************/

static int num_gates = 0;
static xyz_sprite* gates[MAX_NUM_GATES];

static xyz_sprite* new_gate(int x, int y, int type) {
  gatesprites[0].user_info = (void*)type;
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
  int input1, input2, output;
  xyz_sprite *sprite = (xyz_sprite*)conn->user_info;
  int type = (int)xyz_sprite_get_user_info(sprite);
  void *signal_one = get_signal_one();
  void *signal_zero = get_signal_zero();

  input1 = conn->inputs[0]->signal == signal_one;
  if(conn->num_inputs > 1)
    input2 = conn->inputs[1]->signal == signal_one;

  switch(type & GATE_TYPE_MASK) {
  case GATE_TYPE_OR:
    output = input1 | input2;
    break;
  case GATE_TYPE_AND:
    output = input1 & input2;
    break;
  case GATE_TYPE_NOT:
    output = !input1;
    break;
  default:
    xyz_fatal_error("Unknown gate type '%d'!", type & GATE_TYPE_MASK);
  }

  conn->outputs[0]->calculated_signal = (output ? signal_one : signal_zero);
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

  switch(type) {
  case GATE_TYPE_AND:
    xyz_draw_image(andor_gate_image, x, y);
    xyz_color(128, 128, 128);
    xyz_rectangle(x + AND_CROSSBAR_WIDTH_OFFSET, y + AND_CROSSBAR_OFF_HEIGHT,
		  AND_CROSSBAR_WIDTH, AND_CROSSBAR_HEIGHT);
    break;
  case (GATE_TYPE_AND | GATE_MOD_UPSIDE_DOWN):
    xyz_draw_image(flipped_andor_gate_image, x, y);
    xyz_color(128, 128, 128);
    xyz_rectangle(x + AND_CROSSBAR_WIDTH_OFFSET,
		  y + GATE_HEIGHT - AND_CROSSBAR_OFF_HEIGHT -
		    AND_CROSSBAR_HEIGHT,
		  AND_CROSSBAR_WIDTH, AND_CROSSBAR_HEIGHT);
    break;
  case GATE_TYPE_OR:
    xyz_draw_image(andor_gate_image, x, y);
    xyz_color(128, 0, 180);
    xyz_rectangle(x + OR_CROSSBAR_WIDTH_OFFSET, y + OR_CROSSBAR_OFF_HEIGHT,
		  OR_CROSSBAR_WIDTH, OR_CROSSBAR_HEIGHT);
    break;
  case (GATE_TYPE_OR | GATE_MOD_UPSIDE_DOWN):
    xyz_draw_image(flipped_andor_gate_image, x, y);
    xyz_color(128, 0, 180);
    xyz_rectangle(x + OR_CROSSBAR_WIDTH_OFFSET,
		  y + GATE_HEIGHT - OR_CROSSBAR_OFF_HEIGHT - OR_CROSSBAR_HEIGHT,
		  OR_CROSSBAR_WIDTH, OR_CROSSBAR_HEIGHT);
    break;
  case GATE_TYPE_NOT:
    xyz_draw_image(not_gate_image, x, y);
    break;
  case (GATE_TYPE_NOT | GATE_MOD_UPSIDE_DOWN):
    xyz_draw_image(flipped_not_gate_image, x, y);
    break;
  default:
    xyz_fatal_error("Unknown gate type '%d/%d'!", type, type & GATE_TYPE_MASK);
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

static void gate_create_connector(xyz_sprite *sprite) {
  GatePrivate *priv = (GatePrivate*)xyz_sprite_get_private_data(sprite);
  conn_output_private *opriv = xyz_new(conn_output_private);
  conn_input_private *ipriv1 = xyz_new(conn_input_private);
  conn_input_private *ipriv2 = xyz_new(conn_input_private);
  conn_output *output;
  conn_input *input1;
  conn_input *input2;
  int type = (int)xyz_sprite_get_user_info(sprite);
  connector_type *conn_type;
  int num_inputs;
  int upside_down;

  upside_down = !!(type & GATE_MOD_UPSIDE_DOWN);
  switch(type & GATE_TYPE_MASK) {
  case GATE_TYPE_AND:
  case GATE_TYPE_OR:
    conn_type = &andor_gate_conn_type;
    break;
  case GATE_TYPE_NOT:
    conn_type = &not_gate_conn_type;
    break;
  default:
    xyz_fatal_error("Unknown gate type '%d'!", type & GATE_TYPE_MASK);
  }
  num_inputs = conn_type->max_inputs;

  priv->conn = new_connector(conn_type, pizza_connector_set,
			     (void*)sprite);
  output = connector_new_output(priv->conn);
  opriv->x = GATE_WIDTH / 2;
  opriv->y = upside_down ? GATE_HEIGHT : 0;
  output->user_info = (void*)opriv;

  input1 = connector_new_input(priv->conn);
  if(num_inputs == 1) {
    ipriv1->x = GATE_WIDTH / 2;
  } else {
    ipriv1->x = 0;
  }
  ipriv1->y = upside_down ? 0 : GATE_HEIGHT;
  input1->user_info = (void*)ipriv1;

  if(num_inputs > 1) {
    input2 = connector_new_input(priv->conn);
    ipriv2->x = GATE_WIDTH;
    ipriv2->y = upside_down ? 0 : GATE_HEIGHT;
    input2->user_info = (void*)ipriv2;
  }
}

void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  GatePrivate *priv = (GatePrivate*)xyz_sprite_get_private_data(sprite);

  switch(event->type) {
  case XYZ_SPRITE_CREATED:
    gate_create_connector(sprite);
    break;
  case XYZ_SPRITE_DESTROYED:
    destroy_connector(priv->conn);
    break;
  case XYZ_SPRITE_BUTTONDOWN:
    break;
  case XYZ_SPRITE_BUTTONUP:
    if(xyz_sprite_overlap(sprite, toolbox_sprite)) {
      delete_gate(sprite);
    }
    break;
  }
}
