#ifndef __CONNECTORS_H__
#define __CONNECTORS_H__

typedef struct _connector_t connector;
typedef struct _connector_set_t connector_set;

typedef struct _input_t conn_input;
typedef struct _output_t conn_output;

struct _input_t {
  void *signal;
  void *calculated_signal;
  connector *host;
  conn_output *attached;
  void *user_info;
};

struct _output_t {
  void *signal;
  void *calculated_signal;
  connector *host;
  conn_input *attached;
  void *user_info;
};

typedef struct _connector_type_t {
  int max_inputs;
  int max_outputs;
  void (*process)(connector* connector);
} connector_type;

struct _connector_t {
  connector_type *type;
  connector_set *set;
  conn_input **inputs;
  int num_inputs;
  conn_output **outputs;
  int num_outputs;
  void *user_info;
};

struct _connector_set_t {
  connector **connectors;
  int num_connectors;
  int allocated_connectors; /* Actually, allocated connector pointers */
};

void init_connectors(void);
void shutdown_connectors(void);

void *get_signal_one(void);
void *get_signal_zero(void);

connector_set *new_connector_set(void);
connector *new_connector(connector_type *type, connector_set *set,
			 void *user_info);
void destroy_connector(connector *conn);
void connector_add_input(connector *conn, conn_input *input);
void connector_add_output(connector *conn, conn_output *output);
conn_input* connector_new_input(connector *conn);
conn_output* connector_new_output(connector *conn);
void connector_disconnect_input(conn_input *input);
void connector_disconnect_output(conn_output *output);
void connector_delete_input(conn_input *input);
void connector_delete_output(conn_output *output);

connector* ioro_connector(conn_input *input, conn_output *output);
void ioro_disconnect(conn_input *input, conn_output *output);

void connector_connect(conn_input *input, conn_output *output);

void connector_free_signal(void *signal);

void connector_set_process(connector_set *set);

#endif /* __CONNECTORS_H__ */
