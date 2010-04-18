#include "xyz_sprite.h"
#include "connectors.h"
#include "pizza.h"

void judge_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void judge_draw(xyz_sprite *sprite);

static xyz_sprite *judge_sprite;

connector_type judge_conn_type = {
  1, 0, NULL
};

static xyz_sprite_methods judge_sprite_methods = { judge_draw, judge_event_handler };

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1, \
                 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec judge_sprite_spec = {
  "resources/judge_input_small.png",
  JUDGE_LEFT_WIDTH, JUDGE_TOP_HEIGHT, JUDGE_WIDTH, JUDGE_HEIGHT,
  &judge_sprite_methods, EVENTS, NULL, sizeof(JudgePrivate)
};

int judge_says_true(void) {
  JudgePrivate *priv = (JudgePrivate*)xyz_sprite_get_private_data(judge_sprite);

  void *signal = priv->conn->inputs[0]->signal;

  return (signal == get_signal_one());
}

void init_judge_sprite(void) {
  judge_sprite = xyz_sprite_from_spec(&judge_sprite_spec);
}

static void judge_create_connector(xyz_sprite *sprite) {
  JudgePrivate *priv = (JudgePrivate*)xyz_sprite_get_private_data(sprite);
  conn_input_private *ipriv1 = xyz_new(conn_input_private);
  conn_input *input1;

  priv->conn = new_connector(&judge_conn_type, pizza_connector_set,
			     (void*)sprite);

  input1 = connector_new_input(priv->conn);
  ipriv1->x = JUDGE_WIDTH / 2;
  ipriv1->y = JUDGE_HEIGHT;
  input1->user_info = (void*)ipriv1;

}

static void judge_draw(xyz_sprite *sprite) {
  GatePrivate *priv = (GatePrivate*)xyz_sprite_get_private_data(sprite);

  draw_connector(priv->conn);

  /* To avoid recursing, and perhaps infinitely drawing this sprite's
     connector. */
  xyz_default_draw_sprite(sprite);
}

void judge_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  JudgePrivate *priv = (JudgePrivate*)xyz_sprite_get_private_data(sprite);
  /* Do we do anything special in this handler? */

  switch(event->type) {
  case XYZ_SPRITE_CREATED:
    judge_create_connector(sprite);
    break;
  case XYZ_SPRITE_DESTROYED:
    destroy_connector(priv->conn);
    break;
  }
}
