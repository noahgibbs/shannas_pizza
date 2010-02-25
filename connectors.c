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
  connector *tmp = xyz_new(connector);

  tmp->type = type;
  tmp->user_info = user_info;
  tmp->outputs = calloc(sizeof(conn_output*) * type->max_outputs, 1);
  tmp->inputs = calloc(sizeof(conn_input*) * type->max_inputs, 1);

  return tmp;
}

void destroy_connector(connector *conn) {
  int i;

  for(i = 0; i < conn->num_inputs; i++) {
    connector_disconnect_input(conn->inputs[i]);
    connector_delete_input(conn->inputs[i]);
  }

  for(i = 0; i < conn->num_outputs; i++) {
    connector_disconnect_output(conn->outputs[i]);
    connector_delete_output(conn->outputs[i]);
  }

  free(conn->inputs);
  free(conn->outputs);
  free(conn);
}

void connector_add_input(connector *conn, conn_input *input) {
  if(conn->num_inputs >= conn->type->max_inputs)
    xyz_fatal_error("Trying to connect too many inputs!");

  conn->inputs[conn->num_inputs] = input;
  conn->num_inputs++;

  input->host = conn;
}

void connector_add_output(connector *conn, conn_output *output) {
  if(conn->num_outputs >= conn->type->max_outputs)
    xyz_fatal_error("Trying to connect too many outputs!");

  conn->outputs[conn->num_outputs] = output;
  conn->num_outputs++;

  output->host = conn;
}

conn_output *connector_new_output(connector *conn) {
  conn_output *tmp = xyz_new(conn_output);

  connector_add_output(conn, tmp);

  return tmp;
}

conn_input *connector_new_input(connector *conn) {
  conn_input *tmp = xyz_new(conn_input);

  connector_add_input(conn, tmp);

  return tmp;
}

void connector_disconnect_input(conn_input *input) {
  if(input->attached) {
    input->attached->attached = NULL;
  }
}

void connector_disconnect_output(conn_output *output) {
  if(output->attached) {
    output->attached->attached = NULL;
  }
}

void connector_delete_input(conn_input *input) {
  free(input);
}

void connector_delete_output(conn_output *output) {
  free(output);
}

void connector_connect(conn_input *input, conn_output *output) {
  input->attached = output;
  output->attached = input;
}

connector* ioro_connector(conn_input *input, conn_output *output) {
  if(input) return input->host;
  if(output) return output->host;
  return NULL;
}
