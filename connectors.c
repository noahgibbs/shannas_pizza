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

connector_set *new_connector_set(void) {
  connector_set *tmp = xyz_new(connector_set);
  tmp->allocated_connectors = 16;
  tmp->connectors = calloc(sizeof(connector*) * tmp->allocated_connectors, 1);
  tmp->num_connectors = 0;

  return tmp;
}

connector *new_connector(connector_type *type, connector_set *set,
			 void *user_info) {
  connector *tmp = xyz_new(connector);

  tmp->type = type;
  tmp->user_info = user_info;
  tmp->outputs = calloc(sizeof(conn_output*) * type->max_outputs, 1);
  tmp->inputs = calloc(sizeof(conn_input*) * type->max_inputs, 1);
  tmp->set = set;

  if(set->num_connectors >= set->allocated_connectors) {
    set->allocated_connectors *= 2;
    set->connectors = realloc(set->connectors,
			      sizeof(connector*) * set->allocated_connectors);
    if(!set->connectors) {
      xyz_fatal_error("Can't reallocate connectors in connector_set!");
    }
  }
  set->connectors[set->num_connectors] = tmp;
  set->num_connectors++;

  return tmp;
}

void destroy_connector(connector *conn) {
  int i;
  connector_set *set = conn->set;

  for(i = 0; i < conn->num_inputs; i++) {
    connector_disconnect_input(conn->inputs[i]);
    connector_delete_input(conn->inputs[i]);
  }

  for(i = 0; i < conn->num_outputs; i++) {
    connector_disconnect_output(conn->outputs[i]);
    connector_delete_output(conn->outputs[i]);
  }

  for(i = 0; i < set->num_connectors; i++) {
    if(set->connectors[i] == conn) break;
  }
  if(i >= set->num_connectors)
    xyz_fatal_error("Couldn't find connector in connector set!");

  set->connectors[i] = set->connectors[set->num_connectors - 1];

  set->num_connectors--;

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
  if(input->attached && input->attached->attached == input) {
    input->attached->attached = NULL;
  }
  if(input->attached && input->attached->attached) {
    xyz_fatal_error("Consistency error (input)!  Dying!");
  }
  input->attached = NULL;
}

void connector_disconnect_output(conn_output *output) {
  if(output->attached && output->attached->attached == output) {
    output->attached->attached = NULL;
  }
  if(output->attached && output->attached->attached) {
    xyz_fatal_error("Consistency error (output)!  Dying!");
  }
  output->attached = NULL;
}

void connector_delete_input(conn_input *input) {
  free(input);
}

void connector_delete_output(conn_output *output) {
  free(output);
}

void connector_connect(conn_input *input, conn_output *output) {
  connector_disconnect_input(input);
  connector_disconnect_output(output);
  input->attached = output;
  output->attached = input;
}

connector* ioro_connector(conn_input *input, conn_output *output) {
  if(input) return input->host;
  if(output) return output->host;
  return NULL;
}

void ioro_disconnect(conn_input *input, conn_output *output) {
  if(input) connector_disconnect_input(input);
  if(output) connector_disconnect_output(output);
}

void connector_set_process(connector_set *set) {
  int i, j;
  connector *index;

  /* Propagate output signals to attached inputs, if any */
  for(i = 0; i < set->num_connectors; i++) {
    index = set->connectors[i];

    /* Propagate signals from attached output to attached inputs */
    for(j = 0; j < index->num_outputs; j++) {
      conn_output *output = index->outputs[j];
      if(output->attached) {
	connector_free_signal(output->attached->signal);
	output->attached->signal = connector_copy_signal(output->signal);
      }
    }

    /* No signal to unattached inputs */
    for(j = 0; j < index->num_inputs; j++) {
      conn_input *input = index->inputs[j];
      if(!input->attached) {
	connector_free_signal(input->signal);
	input->signal = NULL;
      }
    }
  }

  /* Call process() to get calculated signals */
  for(i = 0; i < set->num_connectors; i++) {
    index = set->connectors[i];
    if(index->type->process)
      index->type->process(index);
  }

  /* Move calculated signals over to regular signals */
  for(i = 0; i < set->num_connectors; i++) {
    index = set->connectors[i];
    for(j = 0; j < index->num_outputs; j++) {
      conn_output *output = index->outputs[j];
      connector_free_signal(output->signal);
      output->signal = output->calculated_signal;
    }
  }
}

void connector_free_signal(void *signal) {
  /* For now, do nothing */
}

void *connector_copy_signal(void *signal) {
  /* For now, just return it again */
  return signal;
}
