#ifndef __PIZZA_H__
#define __PIZZA_H__

#include "connectors.h"

/* Cursor "hot spot" */
#define CURSOR_X_OFFSET        6
#define CURSOR_Y_OFFSET        4

/* Screen layout */
#define TOTAL_HEIGHT           500
#define TOTAL_WIDTH            500
#define SYMBOLS_TOP_HEIGHT     400
#define CONVEYOR_BOTTOM_HEIGHT 100
#define TOOLBOX_TOP_HEIGHT     150
#define TOOLBOX_BOTTOM_HEIGHT  400
#define TOOLBOX_LEFT_WIDTH     400

/* Sizes of sprite types */
#define SYMBOLS_WIDTH  32
#define SYMBOLS_HEIGHT 32

#define GATE_WIDTH 32
#define GATE_HEIGHT 64

#define AND_CROSSBAR_WIDTH         28
#define AND_CROSSBAR_OFF_HEIGHT    32
#define AND_CROSSBAR_ON_HEIGHT     16
#define AND_CROSSBAR_HEIGHT        10
#define AND_CROSSBAR_WIDTH_OFFSET  ((GATE_WIDTH - AND_CROSSBAR_WIDTH) / 2)

#define OR_CROSSBAR_WIDTH          28
#define OR_CROSSBAR_OFF_HEIGHT     32
#define OR_CROSSBAR_ON_HEIGHT      16
#define OR_CROSSBAR_HEIGHT         7
#define OR_CROSSBAR_WIDTH_OFFSET   ((GATE_WIDTH - OR_CROSSBAR_WIDTH) / 2)

/* Radius in pixels to click on an input or output */
#define CONNECT_RADIUS 15

/* Gate constants */
#define MAX_NUM_GATES 100

#define GATE_TYPE_AND    0
#define GATE_TYPE_OR     1
#define GATE_TYPE_MAX    1 /* Duplicate */


/* Structures */

typedef struct {
  /* Offset within sprite */
  int x;
  int y;
} conn_input_private;

typedef struct {
  /* Offset within sprite */
  int x;
  int y;
} conn_output_private;

typedef struct {
  connector *conn;
} ToppingPrivate;

typedef struct {
  connector *conn;
} GatePrivate;


/* Prototypes */

/* From pizza.c */
void keyhandler(const char *keyname, int down);
int mouse_move_handler(int x, int y);
int mouse_button_handler(int button, int is_down);
void drag_sprite(xyz_sprite *sprite);
void drag_sprite_with_offset(xyz_sprite *sprite, int x_off, int y_off);
void drag_to_connect(xyz_sprite *from_sprite,
		     connector *from_connector,
		     int from_x, int from_y);
void wire_from_to(int from_x, int from_y, int to_x, int to_y);

/* From sprites.c */
void draw_sprites(void);
void load_sprites(void);
void free_sprites(void);
void draw_connector(connector *conn);
int intersect_connector_objects(int x, int y,
				conn_input **inputOutP,
				conn_output **outputOutP);

/* From gates.c */
void init_gate_sprites(void);
void delete_gate_sprites(void);

/* From toppings.c */
void init_toppings(void);
void delete_toppings(void);
void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);

#endif /* __PIZZA_H__ */
