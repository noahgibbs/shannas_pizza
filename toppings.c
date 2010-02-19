#include "connectors.h"
#include "xyz_sprite.h"

void topping_connector_process(connector *conn);

connector_type topping_type = {
  0, 1, topping_connector_process
};


void init_toppings(void) {
  /* Initialize connectors */
}

static int hack = 0;

void topping_connector_process(connector *conn) {
  if(conn->num_outputs < 1) return;

  /* Turn output on or off */
  hack++;
  conn->outputs[0].signal = hack ? get_signal_one() : get_signal_zero();
}
