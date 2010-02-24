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


int main(void) {
  connector *conn;

  conn = new_connector(&type1, NULL);

  if(assertions_failed) {
    fprintf(stderr, "%d assertions failed!\n", assertions_failed);
  }
  printf("%d assertions passed, %d assertions total\n",
	 assertions_passed, assertions_passed + assertions_failed);

  return assertions_failed ? -1 : 0;
}
