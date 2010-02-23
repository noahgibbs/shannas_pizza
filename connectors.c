/* Connector objects, input and output objects, all identified by pointers */

#include "xyz.h"
#include "connectors.h"

/* Static objects for individual signals. */
static int signal_one = 0;
static int signal_zero = 0;

void *get_signal_one(void) {
  return (void*)&signal_one;
}

void *get_signal_zero(void) {
  return (void*)&signal_zero;
}

void init_connectors(void) {

}

void shutdown_connectors(void) {

}

connector *new_connector(connector_type *type, void *user_info) {
  connector *tmp = calloc(sizeof(connector), 1);

  tmp->type = type;
  tmp->user_info = user_info;

  return tmp;
}

void destroy_connector(connector *conn) {
  free(conn);
}

void connect_input(connector *conn, conn_input *input) {
  if(conn->num_inputs >= conn->type->max_inputs)
    xyz_fatal_error("Trying to connect too many inputs!");

  conn->inputs[conn->num_inputs] = input;
  conn->num_inputs++;
}

void connect_output(connector *conn, conn_output *output) {
  if(conn->num_outputs >= conn->type->max_outputs)
    xyz_fatal_error("Trying to connect too many outputs!");

  conn->outputs[conn->num_outputs] = output;
  conn->num_outputs++;
}
