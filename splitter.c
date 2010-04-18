#include "xyz_sprite.h"
#include "connectors.h"
#include "pizza.h"

static void splitter_draw(xyz_sprite *sprite);
/* HACK: Public to check for dragging */
void splitter_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);

static void splitter_target_draw(xyz_sprite *sprite);
static void splitter_target_event_handler(xyz_sprite *sprite,
					  xyz_sprite_event *event);

static void splitter_connector_process(connector *conn);

static xyz_sprite_methods splitter_methods = { splitter_draw,
					       splitter_event_handler };
static xyz_sprite_methods splitter_target_methods = { splitter_target_draw,
						      splitter_target_event_handler
};

connector_type splitter_conn_type = {
  1, 2, splitter_connector_process
};

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1, \
                 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec splitterfilesprites[] = {
  {"",
   TOOLBOX_LEFT_WIDTH + 2 * GATE_WIDTH, TOOLBOX_TOP_HEIGHT,
   SPLITTER_WIDTH, SPLITTER_HEIGHT,
   &splitter_target_methods, EVENTS, NULL},
  { NULL }
};

static xyz_sprite_spec splittersprites[] = {
  { "", 0, 0, SPLITTER_WIDTH, SPLITTER_HEIGHT,
    &splitter_methods, EVENTS, NULL, sizeof(SplitterPrivate) },
};

static xyz_image *splitter_image = NULL;

void init_splitter_sprite(void) {
  splitter_image = xyz_load_image("resources/splitter_small.png");

  xyz_sprites_from_specs(-1, splitterfilesprites);
}

void delete_splitter_sprite(void) {
}

/************** Splitter functions ********************/

static int num_splitters = 0;
static xyz_sprite* splitters[MAX_NUM_SPLITTERS];

static xyz_sprite* new_splitter(int x, int y) {
  xyz_sprite *splitter = xyz_sprite_from_spec(&splittersprites[0]);
  xyz_sprite_set_x(splitter, x - SPLITTER_WIDTH / 2);
  xyz_sprite_set_y(splitter, y - SPLITTER_HEIGHT / 2);

  splitters[num_splitters] = splitter;
  num_splitters++;

  return splitter;
}

void delete_splitter(xyz_sprite *sprite) {
  int i = 0;

  while(splitters[i] != sprite && i < num_splitters) i++;
  if(i >= num_splitters)
    xyz_fatal_error("Deleting non-existent splitter sprite %p!\n", sprite);

  num_splitters--;
  while(i < num_splitters) {
    splitters[i] = splitters[i + 1];
    i++;
  }

  xyz_free_sprite(sprite);
}

static void splitter_connector_process(connector *conn) {
  conn->outputs[0]->calculated_signal = conn->inputs[0]->signal;
  conn->outputs[1]->calculated_signal = conn->inputs[0]->signal;
}

/*********** Splitter event handlers ******************/

static void splitter_draw(xyz_sprite *sprite) {
  SplitterPrivate *priv = (SplitterPrivate*)xyz_sprite_get_private_data(sprite);
  int x = xyz_sprite_get_x(sprite);
  int y = xyz_sprite_get_y(sprite);

  draw_connector(priv->conn);

  xyz_draw_image(splitter_image, x, y);
}

static void splitter_target_draw(xyz_sprite *sprite) {
  int x, y, width, height;

  x = xyz_sprite_get_x(sprite);
  y = xyz_sprite_get_y(sprite);
  width = xyz_sprite_get_width(sprite);
  height = xyz_sprite_get_height(sprite);
  xyz_color(0, 80, 250);
  xyz_rectangle(x, y, width, height);

  xyz_draw_image(splitter_image, x, y);
}

static void splitter_target_event_handler(xyz_sprite *sprite,
				      xyz_sprite_event *event) {
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    if(event->button == 1) {
      if(num_splitters >= MAX_NUM_SPLITTERS) {
	/* TODO: real response of some useful kind */
	xyz_fatal_error("ERROR!  NOT ENOUGH SPLITTERS!");
      } else {
	xyz_sprite *splitter = new_splitter(event->mouse_x, event->mouse_y);
	drag_sprite_with_offset(splitter, SPLITTER_WIDTH / 2, SPLITTER_HEIGHT / 2);
      }
    }
    break;
  }
}

static void splitter_create_connector(xyz_sprite *sprite) {
  SplitterPrivate *priv = (SplitterPrivate*)xyz_sprite_get_private_data(sprite);
  conn_output_private *opriv1 = xyz_new(conn_output_private);
  conn_output_private *opriv2 = xyz_new(conn_output_private);
  conn_input_private *ipriv = xyz_new(conn_input_private);
  conn_output *output1;
  conn_output *output2;
  conn_input *input;

  priv->conn = new_connector(&splitter_conn_type, pizza_connector_set,
			     (void*)sprite);
  output1 = connector_new_output(priv->conn);
  output2 = connector_new_output(priv->conn);
  opriv1->x = SPLITTER_WIDTH;
  opriv1->y = 0;
  opriv2->x = 0;
  opriv2->y = SPLITTER_HEIGHT;
  output1->user_info = (void*)opriv1;
  output2->user_info = (void*)opriv2;

  input = connector_new_input(priv->conn);
  ipriv->x = SPLITTER_WIDTH / 2;
  ipriv->y = SPLITTER_HEIGHT / 2;
  input->user_info = (void*)ipriv;
}

void splitter_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  SplitterPrivate *priv = (SplitterPrivate*)xyz_sprite_get_private_data(sprite);

  switch(event->type) {
  case XYZ_SPRITE_CREATED:
    splitter_create_connector(sprite);
    break;
  case XYZ_SPRITE_DESTROYED:
    destroy_connector(priv->conn);
    break;
  case XYZ_SPRITE_BUTTONDOWN:
    break;
  case XYZ_SPRITE_BUTTONUP:
    if(xyz_sprite_overlap(sprite, get_toolbox_sprite())) {
      delete_splitter(sprite);
    }
    break;
  }
}
