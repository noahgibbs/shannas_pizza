#ifndef __PIZZA_H__
#define __PIZZA_H__

#include "xyz.h"
#include "xyz_sprite.h"
#include "xyz_anim.h"

#include "connectors.h"

/* Cursor "hot spot" */
#define CURSOR_X_OFFSET        6
#define CURSOR_Y_OFFSET        4

/* Screen layout */
#define TOTAL_HEIGHT           500
#define TOTAL_WIDTH            500

#define BACKGROUND_COLOR_R    0xf8
#define BACKGROUND_COLOR_G    0xf0
#define BACKGROUND_COLOR_B    0x9f

#define FLASH_COLOR_R         0xff
#define FLASH_COLOR_G         0x00
#define FLASH_COLOR_B         0x00

/* Outputs for whether toppings are present */

#define TOPPING_BOX_R         0x00
#define TOPPING_BOX_G         0x00
#define TOPPING_BOX_B         0x00

/* Conveyor, and top background above it */
#define CONVEYOR_TOP_HEIGHT     40
#define CONVEYOR_HEIGHT         60
#define TOPBACK_COLOR_R       0xff
#define TOPBACK_COLOR_G       0xff
#define TOPBACK_COLOR_B       0xbb
#define CONVEYOR_COLOR_R      0x6f
#define CONVEYOR_COLOR_G      0x2b
#define CONVEYOR_COLOR_B      0x09

/* Shanna Picture */

#define SFRAME_COLOR_R        0xff
#define SFRAME_COLOR_G        0xf0
#define SFRAME_COLOR_B        0x6b

/* Toolbox */
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

#define SPLITTER_WIDTH  32
#define SPLITTER_HEIGHT 64

#define JUDGE_WIDTH                32
#define JUDGE_HEIGHT               32
#define JUDGE_LEFT_WIDTH           ((TOOLBOX_LEFT_WIDTH - JUDGE_WIDTH) / 2)
#define JUDGE_TOP_HEIGHT           (CONVEYOR_TOP_HEIGHT + CONVEYOR_HEIGHT)

#define SFRAME_WIDTH              5
#define SFRAME_HEIGHT             5

#define SHANNA_IMAGE_WIDTH      150
#define SHANNA_IMAGE_HEIGHT     150
#define SHANNA_IMAGE_LEFT_WIDTH (TOTAL_WIDTH - SHANNA_IMAGE_WIDTH - SFRAME_WIDTH)
#define SHANNA_IMAGE_TOP_HEIGHT SFRAME_HEIGHT

/* Timing and locations for animations */

#define ROLL_PIZZA_X_START         5
#define ROLL_PIZZA_X_END         280
#define ROLL_PIZZA_Y               5
#define ROLL_PIZZA_DURATION     3000
#define ROLL_PIZZA_PAUSE        1000

#define PASS_DURATION_MILLIS    1500
#define FAIL_DURATION_MILLIS    1500
#define SHANNA_DURATION_MILLIS  1500
#define BIG_SHANNA_DURATION_MILLIS 3000

#define INSTRUCTION_TEXT_LEFT_WIDTH  50
#define INSTRUCTION_TEXT_TOP_HEIGHT  400
#define INSTRUCTION_TEXT_DURATION_MILLIS 3000

/* Radius in pixels to click on an input or output */
#define CONNECT_RADIUS 15

/* Gate constants */
#define MAX_NUM_GATES 100
#define MAX_NUM_SPLITTERS 75

#define GATE_TYPE_AND            0
#define GATE_TYPE_OR             1
#define GATE_TYPE_NOT            2
#define GATE_TYPE_MAX            2 /* Intentional Duplicate */
#define GATE_TYPE_MASK         127

#define GATE_MOD_UPSIDE_DOWN   128

#define GATE_DELAY_USECS 250000

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

  void *sprite;
} conn_output_private;

typedef struct {
  connector *conn;
  int topping_number;
} ToppingPrivate;

typedef struct {
  connector *conn;
} GatePrivate;

typedef struct {
  connector *conn;
} SplitterPrivate;

typedef struct {
  connector *conn;
} JudgePrivate;


/* Prototypes */

/* From pizza.c */
void keyhandler(int keysym, const char *keyname, int down);
int mouse_move_handler(int x, int y);
int mouse_button_handler(int button, int is_down);
void drag_sprite(xyz_sprite *sprite);
void drag_sprite_with_offset(xyz_sprite *sprite, int x_off, int y_off);
void drag_to_connect(xyz_sprite *from_sprite,
		     connector *from_connector,
		     conn_input *from_input,
		     conn_output *from_output,
		     int from_x, int from_y);
void wire_from_to(int from_x, int from_y, int to_x, int to_y);

/* simple_anim.c */
void screen_draw_background(void);
void start_screen_flash(void);
void start_lever_animation(void);
void draw_lever(int x, int y);

/* From sprites.c */
void draw_sprites(void);
void load_sprites(void);
void free_sprites(void);
void draw_red_x(int x, int y);
void draw_connector(connector *conn);
int intersect_connector_objects(int x, int y,
				conn_input **inputOutP,
				conn_output **outputOutP);

/* From gates.c */
void init_gate_sprites(void);
void delete_gate_sprites(void);
void gate_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
xyz_sprite* get_toolbox_sprite(void);

/* From splitter.c */
void init_splitter_sprite(void);
void delete_splitter_sprite(void);
void splitter_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);

/* From toppings.c */
void init_toppings(void);
void delete_toppings(void);
void topping_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);
void draw_pizza(int x, int y);

/* From level.c */
void sp_init(void);
int sp_get_n_toppings(void);
int sp_get_topping_mask(void);
xyz_image** sp_get_topping_images(void);
const char *sp_get_end_of_level_image_filename(void);
xyz_anim* sp_get_intro_animation(void);
void sp_set_level(int level);
void sp_next_level(void);
void sp_start_level(void);
int sp_should_be_true(int topping_mask);
void sp_refresh(void);

/* From judge_input.c */
void init_judge_sprite(void);
int judge_says_true(void);
void judge_event_handler(xyz_sprite *sprite, xyz_sprite_event *event);

/* From roll_pizza_anim.c */
void start_pizzas_rolling();
void roll_pizza_refresh(void);
int pizza_is_rolling(void);
void esc_to_cancel_pizza(void);
int topping_is_rolling(int topping_number);

/* From text_anim.c */
xyz_anim* show_text_anim(int duration_millis, const char *text,
			 int x, int y);

/* Variables */

extern connector_set *pizza_connector_set;

#endif /* __PIZZA_H__ */
