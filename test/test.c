#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "test.h"

static int assertions_failed = 0;
static int assertions_passed = 0;

/******* Test Infrastructure **************/

void assert(int cond, const char *message) {
  if(!cond) {
    fprintf(stderr, "Assertion failed: %s\n", message);
    assertions_failed++;
  } else {
    assertions_passed++;
  }
}

int within_delta(double x, double y, double delta) {
  int truth = fabs(x - y) < delta;

  if(!truth) {
    fprintf(stderr, "Delta check failed: %g is not within %g of %g!\n",
	    x, delta, y);
  }

  return truth;
}

int assertion_check(void) {
  if(assertions_failed) {
    fprintf(stderr, "%d assertions failed!\n", assertions_failed);
  }
  printf("%d assertions passed, %d assertions total\n",
	 assertions_passed, assertions_passed + assertions_failed);

  return assertions_failed ? -1 : 0;
}
