#include "xyz_variable.h"
#include "test.h"

static void variable1_test(void) {
  xyz_variable *var1;
  double times[] = {-0.25, 0.7, 1.0, 1.9, 5.0};
  double values[] = {1.4, -3.8, 2.5, 6.1, 999.4};

  var1 = xyz_variable1_new("x", 7.0, XYZ_INTERP_LINEAR, 5, times, values);
  assert(var1 == xyz_variable_by_name("x"), "Couldn't look up variable!");
  assert(xyz_variable1_at_time(var1, -7) == 1.4,
	 "Value is wrong before start time!");
  assert(xyz_variable1_at_time(var1, 7) == 999.4,
	 "Value is wrong after end time!");
  assert(within_delta(xyz_variable1_at_time(var1, -0.25), 1.4, 0.001),
	 "Value is wrong at start time!");
  assert(within_delta(xyz_variable1_at_time(var1, -0.24), 1.345, 0.001),
	 "Value is wrong near start time!");
  assert(within_delta(xyz_variable1_at_time(var1, 0.225), -1.2, 0.001),
	 "Value is wrong at center of interval!");
  assert(within_delta(xyz_variable1_at_time(var1, 0.999), 2.49, 0.1),
	 "Value is wrong near end of interval!");
  assert(within_delta(xyz_variable1_at_time(var1, 1.0), 2.5, 0.001),
	 "Value is wrong at end of interval!");
}

int main(void) {
  variable1_test();

  return assertion_check();
}
