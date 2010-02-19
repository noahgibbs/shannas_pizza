#include "connectors.h"
#include "xyz_sprite.h"

void topping_connector_process(connector *conn);

connector_type topping_type = {
  0, 1, topping_connector_process
};


void init_toppings(void) {
  /* Initialize connectors */
}

void topping_connector_process(connector *conn) {
  if(conn->num_outputs < 1) return;

  /* Turn output on */
  conn->outputs[0].signal = get_signal_one();
}
