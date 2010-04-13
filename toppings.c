#include "connectors.h"
#include "xyz_sprite.h"
#include "pizza.h"

static void topping_connector_process(connector *conn);

static xyz_sprite_methods topping_methods = { NULL, topping_event_handler };

static xyz_image *pizza_image = NULL;

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1, \
                 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec toppingsprites[] = {
  { "resources/sausage_small_white.png", 50, 425, 32, 32,
    &topping_methods, EVENTS, (void*)0, sizeof(ToppingPrivate) },
  { "resources/pepper_small_white.png", 130, 450, 32, 32,
    &topping_methods, EVENTS, (void*)1, sizeof(ToppingPrivate) },
  { "resources/pineapple_small_white.png", 210, 425, 32, 32,
    &topping_methods, EVENTS, (void*)2, sizeof(ToppingPrivate) },
  { NULL }
};

connector_type topping_type = {
  0, 1, topping_connector_process
};


void init_toppings(void) {
  /* Initialize connectors */
  xyz_sprites_from_specs(-1, toppingsprites);

  pizza_image = xyz_load_image("resources/pizza_100.png");
}

void delete_toppings(void) {

}

void draw_pizza(int x, int y) {
  xyz_draw_image(pizza_image, x, y);
}

static int hack = 0;

static void topping_connector_process(connector *conn) {
  xyz_sprite *sprite = (xyz_sprite*)conn->user_info;
  ToppingPrivate *tp = (ToppingPrivate*)xyz_sprite_get_private_data(sprite);

  if(conn->num_outputs < 1) return;

  if(pizza_is_rolling()) {
    int rolling = topping_is_rolling(tp->topping_number);

    conn->outputs[0]->calculated_signal = rolling ? get_signal_one() : get_signal_zero();
    return;
  }

  /* Turn output on or off */
  hack++;
  conn->outputs[0]->calculated_signal = (hack % 2) ? get_signal_one() : get_signal_zero();
}

void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  ToppingPrivate *tp = (ToppingPrivate*)xyz_sprite_get_private_data(sprite);

  switch(event->type) {
  case XYZ_SPRITE_CREATED: {
    conn_output *output;
    conn_output_private *priv = xyz_new(conn_output_private);

    /* Make new connector */
    tp->conn = new_connector(&topping_type, pizza_connector_set,
			     (void*)sprite);
    tp->topping_number = (int)xyz_sprite_get_user_info(sprite);

    /* Make new output for connector */
    output = connector_new_output(tp->conn);
    priv->x = SYMBOLS_WIDTH / 2;
    priv->y = 0;
    output->user_info = (void*)priv;
    break;
  }
  case XYZ_SPRITE_DESTROYED:
    destroy_connector(tp->conn);
    break;
  }
}
