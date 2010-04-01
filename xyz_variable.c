#include "xyz_variable.h"
#include <string.h>
#include <math.h>

struct _xyz_variable_t {
  int dimensions;
  int n_keys;
  char *name;
  double duration;
  double *times;
  double *values;
  double *returns;
  xyz_interpolation_function interpolate;

  xyz_variable *next;
  xyz_variable *prev;
};

xyz_variable *head = NULL;
xyz_variable *tail = NULL;

xyz_variable *xyz_variable_by_name(const char *name) {
  xyz_variable *index = head;

  while(index) {
    if(!strcasecmp(name, index->name)) return index;
    index = index->next;
  }

  return NULL;
}

void xyz_variable_free(xyz_variable *variable) {
  if(!variable) return;

  free(variable->name);
  free(variable->times);
  free(variable->values);

  if(variable == head)
    head = variable->next;
  if(variable == tail)
    tail = variable->prev;

  if(variable->next)
    variable->next->prev = variable->prev;
  if(variable->prev)
    variable->prev->next = variable->next;
}

double xyz_variable1_at_time(xyz_variable *var, double time) {
  if(var->dimensions != 1)
    xyz_fatal_error("Evaluating variable with more than one dimension!");
  return var->interpolate(var, time)[0];
}

const double* xyz_variableN_at_time(xyz_variable *var, double time) {
  return var->interpolate(var, time);
}

static const double* linear_interp(xyz_variable *var, double time) {
  int idx = 0;
  int i;

  if(time <= var->times[0]) return var->values;
  if(time >= var->times[var->n_keys - 1])
    return var->values + (var->n_keys - 1) * var->dimensions;

  while(1) {
    if(time >= var->times[idx] && time < var->times[idx + 1]) {
      double t_int = var->times[idx + 1] - var->times[idx];
      double ti1 = (time - var->times[idx]) / t_int;
      double ti2 = (var->times[idx + 1] - time) / t_int;

      for(i = 0; i < var->dimensions; i++) {
	var->returns[i] = ti1 * var->values[(idx + 1) * var->dimensions + i] +
	  ti2 * var->values[idx * var->dimensions + i];
      }
      return var->returns;
    }
    idx++;

    if(idx >= var->n_keys)
      xyz_fatal_error("Fell off the end of the variable's keyframe data!");
  }
}

static xyz_interpolation_function method_to_function(int interpolation_method) {
  if(interpolation_method != XYZ_INTERP_LINEAR)
    xyz_fatal_error("Unimplemented interpolation method!");
  return linear_interp;
}

xyz_variable *xyz_variable1_new(const char *name, double duration,
				int interpolation_method,
				int n_keys, double *times, double *values) {
  return xyz_variableN_new(name, 1, duration, interpolation_method,
			   n_keys, times, values);
}

xyz_variable *xyz_variableN_new(const char *name, int dimensions,
				double duration, int interpolation_method,
				int n_keys, double *times, double *values) {
  xyz_variable *ret = xyz_variable_by_name(name);

  if(ret)
    xyz_fatal_error("Variable %s already exists!", name);

  ret = xyz_new(xyz_variable);
  ret->next = head;
  head = ret;
  if(!tail) tail = ret;

  ret->dimensions = dimensions;
  ret->n_keys = n_keys;
  ret->name = strdup(name);
  ret->duration = duration;
  ret->interpolate = method_to_function(interpolation_method);
  ret->returns = calloc(1, sizeof(double) * dimensions);
  ret->times = calloc(1, sizeof(double) * n_keys);
  memcpy(ret->times, times, sizeof(double) * n_keys);
  ret->values = calloc(1, sizeof(double) * dimensions * n_keys);
  memcpy(ret->values, values, sizeof(double) * dimensions * n_keys);

  return ret;
}
