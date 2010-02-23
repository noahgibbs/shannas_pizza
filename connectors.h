#ifndef __CONNECTORS_H__
#define __CONNECTORS_H__

typedef struct _connector_t connector;

typedef struct _input_t {
  void *signal;
  connector *host;
  connector *attached;
  void *user_info;
} conn_input;

typedef struct _output_t {
  void *signal;
  connector *host;
  connector *attached;
  void *user_info;
} conn_output;

typedef struct _connector_type_t {
  int max_inputs;
  int max_outputs;
  void (*process)(connector* connector);
} connector_type;

struct _connector_t {
  connector_type *type;
  conn_input **inputs;
  int num_inputs;
  conn_output **outputs;
  int num_outputs;
  void *user_info;
};

void init_connectors(void);
void shutdown_connectors(void);

void *get_signal_one(void);
void *get_signal_zero(void);

connector *new_connector(connector_type *type, void *user_info);
void destroy_connector(connector *conn);
void connect_input(connector *conn, conn_input *input);
void connect_output(connector *conn, conn_output *output);

#endif /* __CONNECTORS_H__ */
