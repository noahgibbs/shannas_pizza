#include "connectors.h"
#include "xyz_sprite.h"
#include "pizza.h"

static void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
static void topping_connector_process(connector *conn);

static xyz_sprite_methods topping_methods = { NULL, topping_event_handler };

#define EVENTS { 1, 1, 1, 1, 1, 1, 1, 1 }

static xyz_sprite_spec toppingsprites[] = {
  { "images/sausage_small_white.png", 50, 425, 32, 32, 1,
    &topping_methods, EVENTS, NULL },
  { "images/pepper_small_white.png", 130, 450, 32, 32, 1,
    &topping_methods, EVENTS, NULL },
  { "images/pineapple_small_white.png", 210, 425, 32, 32, 1,
    &topping_methods, EVENTS, NULL },
  { NULL }
};

connector_type topping_type = {
  0, 1, topping_connector_process
};


void init_toppings(void) {
  /* Initialize connectors */
  xyz_sprites_from_specs(-1, toppingsprites);
}

static int hack = 0;

static void topping_connector_process(connector *conn) {
  if(conn->num_outputs < 1) return;

  /* Turn output on or off */
  hack++;
  conn->outputs[0]->signal = hack ? get_signal_one() : get_signal_zero();
}

static void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event) {
  int x, y;

  /* printf("Sprite event handler, event %d\n", event->type); */
  switch(event->type) {
  case XYZ_SPRITE_BUTTONDOWN:
    if(event->button == 1) {
      /* Get in-sprite x and y offset */
      x = event->mouse_x - xyz_sprite_get_x(sprite);
      y = event->mouse_y - xyz_sprite_get_y(sprite);
      printf("Clicked on topping item [%d, %d]!\n", x, y);

      if(xyz_point_distance(SYMBOLS_WIDTH / 2, 0, x, y) < CONNECT_RADIUS) {
	printf("Clicked and connected!\n");
      }
    }
    break;
  }
}