#include <stdio.h>
#include "connectors.h"

connector_type type1 = { 1, 1, NULL };

static int assertions_failed = 0;
static int assertions_passed = 0;

/******* Test Infrastructure **************/

void assert(int cond, char *message) {
  if(!cond) {
    fprintf(stderr, "Assertion failed: %s\n", message);
    assertions_failed++;
  } else {
    assertions_passed++;
  }
}

/******* Tests ****************************/

void connection1_test(void) {
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

  if(assertions_failed) {
    fprintf(stderr, "%d assertions failed!\n", assertions_failed);
  }
  printf("%d assertions passed, %d assertions total\n",
	 assertions_passed, assertions_passed + assertions_failed);

  return assertions_failed ? -1 : 0;
}
