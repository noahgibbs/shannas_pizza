#include <stdio.h>
#include "test.h"
#include "connectors.h"

connector_type type1 = { 1, 1, NULL };

static void connection1_test(void) {
  connector *conn;

  conn = new_connector(&type1, NULL);
  assert(!!conn, "Couldn't allocate connection!");

  assert(!!connector_new_input(conn), "Couldn't allocate input!");
  assert(!!connector_new_output(conn), "Couldn't allocate output!");

  connector_connect(conn->inputs[0], conn->outputs[0]);
  assert(conn->inputs[0]->attached == conn->outputs[0],
	 "Couldn't connect input to output!");
  assert(conn->outputs[0]->attached == conn->inputs[0],
	 "Couldn't connect input to output!");

  destroy_connector(conn);
}

int main(void) {
  init_connectors();
  connection1_test();
  shutdown_connectors();

  return assertion_check();
}
