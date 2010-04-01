#ifndef __XYZ_VARIABLE_H__
#define __XYZ_VARIABLE_H__

#include "xyz.h"

typedef struct _xyz_variable_t xyz_variable;
typedef const double* (*xyz_interpolation_function)(xyz_variable *var,
						    double t);
#define XYZ_INTERP_STEP        1
#define XYZ_INTERP_LINEAR      2
#define XYZ_INTERP_CUBIC       3

xyz_variable *xyz_variable_by_name(const char *name);
void xyz_variable_free(xyz_variable *variable);
double xyz_variable1_at_time(xyz_variable *var, double time);
const double* xyz_variableN_at_time(xyz_variable *var, double time);
xyz_variable *xyz_variable1_new(const char *name, double duration,
				int interpolation_method,
				int n_keys, double *times, double *values);
xyz_variable *xyz_variableN_new(const char *name, int dimensions,
				double duration, int interpolation_method,
				int n_keys, double *times, double *values);

#endif /* __XYZ_VARIABLE_H__ */
