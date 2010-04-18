#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "xyz.h"
#include "xyz_sprite.h"
#include "xyz_anim.h"
#include "pizza.h"
#include "connectors.h"

#define CURSOR_FILE            "resources/cursor_small.png"
/* #define DRAGGING_CURSOR_FILE   "resources/dragging_cursor.png" */

connector_set *pizza_connector_set = NULL;

void init(int argc, char **argv) {
  int i;
  int fullscreen = 0;

  for(i = 1; i < argc; i++) {
    if(!strcasecmp(argv[i], "--fullscreen")) {
      fullscreen = 1;
    } else {
      xyz_fatal_error("Unrecognized parameter '%s'!", argv[i]);
    }
  }

  xyz_start();
  if(fullscreen)
    xyz_set_up_screen_fullscreen();
  else
    xyz_set_up_screen_window(TOTAL_HEIGHT, TOTAL_WIDTH);

  xyz_set_key_handler(keyhandler);
  xyz_set_mouse_handlers(mouse_move_handler, mouse_button_handler);

  init_connectors();
  pizza_connector_set = new_connector_set();
  load_sprites();

  xyz_custom_cursor_from_file(CURSOR_FILE, CURSOR_X_OFFSET, CURSOR_Y_OFFSET);
}

static int show_mousebox = 0;

/* For dragging whole sprites */
static int no_drag = 0;
static xyz_sprite* dragged_sprite = NULL;
static int selected_x_offset;
static int selected_y_offset;

/* For creating new connections */
static int drag_begin_x;
static int drag_begin_y;
static connector *drag_connector = NULL;
static conn_input *drag_input = NULL;
static conn_output *drag_output = NULL;


void wire_from_to(int from_x, int from_y, int to_x, int to_y) {
    xyz_rectangle_coords(from_x, from_y, from_x + 1, to_y);
    xyz_rectangle_coords(from_x, to_y, to_x, to_y + 1);
}

void draw(void) {
  int x, y;

  xyz_mouse_position(&x, &y);

  screen_draw_background();

  /* Conveyor box */
  xyz_color(255, 0, 0);
  xyz_rectangle(0, 0, TOTAL_WIDTH, CONVEYOR_BOTTOM_HEIGHT);

  /* Mouse Coords */
  if(show_mousebox) {
    char mousebox[100];
    xyz_color(128, 128, 128);
    xyz_rectangle(TOTAL_WIDTH-100, TOTAL_HEIGHT-50, 100, 50);
    xyz_color(0, 0, 255);
    snprintf(mousebox, 100, "%d, %d", x, y);
    xyz_block_text(TOTAL_WIDTH-100, TOTAL_HEIGHT-50, mousebox);
  }

  /* Connection-in-progress */
  if(drag_connector) {
    xyz_color(255, 0, 0);
    wire_from_to(drag_begin_x, drag_begin_y, x, y);
  }

  draw_sprites();
  xyz_draw_cursor();
  xyz_done_drawing();
}

void shutdown(void) {
  xyz_custom_cursor_from_file(NULL, 0, 0);
  shutdown_connectors();
  free_sprites();
  xyz_end();
}

/************ Event processing **************/

void keyhandler(int keysym, const char *keyname, int down) {
  if(keysym == 27 && down == 1) {  /* Esc key */
    if(pizza_is_rolling())
      esc_to_cancel_pizza();
    return;
  }

  if(!strcasecmp(keyname, "q"))
    exit(0);
  else if(!strcasecmp(keyname, "m") && down == 1)
    show_mousebox = !show_mousebox;
  else if(down)
    start_screen_flash();
}

int mouse_move_handler(int x, int y) {
  return 0;
}

static int is_draggable(xyz_sprite *sprite) {
  xyz_sprite_methods *methods = xyz_sprite_get_methods(sprite);

  if(methods->handle_event == topping_event_handler)
    return 1;
  if(methods->handle_event == gate_event_handler)
    return 1;
  if(methods->handle_event == splitter_event_handler)
    return 1;

  return 0;
}

void drag_sprite(xyz_sprite *sprite) {
  drag_sprite_with_offset(sprite, 0, 0);
}

void drag_sprite_with_offset(xyz_sprite *sprite, int x_off, int y_off) {
  if(sprite == NULL) {
    no_drag = 1;
    dragged_sprite = NULL;
    drag_connector = NULL;
    return;
  }

  dragged_sprite = sprite;
  selected_x_offset = x_off;
  selected_y_offset = y_off;
}

void drag_to_connect(xyz_sprite *from_sprite,
		     connector *from_connector,
		     conn_input *from_input,
		     conn_output *from_output,
		     int from_x, int from_y)
{
  /* Don't drag a sprite */
  dragged_sprite = NULL;
  selected_x_offset = 0;
  selected_y_offset = 0;

  /* Start creating a connection by dragging */
  drag_begin_x = from_x;
  drag_begin_y = from_y;
  drag_connector = from_connector;
  drag_input = from_input;
  drag_output = from_output;
}

int mouse_button_handler(int button, int is_down) {
  if(button != 1 && is_down) {
    start_screen_flash();
  } else {  /* Button is 1 */
    int x, y;
    conn_input *input;
    conn_output *output;

    xyz_mouse_position(&x, &y);

    if(!is_down) {
      if(drag_connector) {
	/* See if we intersect a different input or output, and if we
	   can connect a new wire */
	if(intersect_connector_objects(x, y, &input, &output)) {
	  if((input && drag_input) || (output && drag_output)) {
	    /* Went from input to input or output to output -- break wire */
	    /* TODO:  Add sound effect and visual feedback */
	  } else {
	    if(input) {
	      connector_connect(input, drag_output);
	    } else {
	      connector_connect(drag_input, output);
	    }
	  }
	}
      }
      dragged_sprite = NULL;
      drag_connector = NULL;
      return 0;
    }

    if(intersect_connector_objects(x, y, &input, &output)) {
      xyz_sprite *sprite;
      connector *conn;
      conn_input_private *ipriv;
      conn_output_private *opriv;
      int sx, sy, px, py;

      conn = ioro_connector(input, output);

      sprite = (xyz_sprite*)conn->user_info;
      sx = xyz_sprite_get_x(sprite);
      sy = xyz_sprite_get_y(sprite);

      if(input) {
	ipriv = (conn_input_private*)input->user_info;
	px = ipriv->x; py = ipriv->y;
      } else {
	opriv = (conn_output_private*)output->user_info;
	px = opriv->x; py = opriv->y;
      }

      ioro_disconnect(input, output);
      drag_to_connect(sprite, conn, input, output, sx + px, sy + py);
      return 0;
    }

    dragged_sprite = xyz_intersect_filtered_sprite(x, y, &is_draggable);
    if(dragged_sprite) {
      selected_x_offset = x - xyz_sprite_get_x(dragged_sprite);
      selected_y_offset = y - xyz_sprite_get_y(dragged_sprite);
    }
  }
  return 0;
}

void process_events(void) {
  int x, y;

  if(no_drag) {
    no_drag = 0;
    dragged_sprite = NULL;
  }

  xyz_mouse_position(&x, &y);
  if(dragged_sprite) {
    int old_x, old_y, new_x, new_y;

    old_x = xyz_sprite_get_x(dragged_sprite);
    old_y = xyz_sprite_get_y(dragged_sprite);
    new_x = x - selected_x_offset;
    new_y = y - selected_y_offset;

    if(old_x == new_x && old_y == new_y) return;

    xyz_sprite_set_x(dragged_sprite, new_x);
    xyz_sprite_set_y(dragged_sprite, new_y);
  }
}

static struct timeval last_process = { 0, 0 };

void do_calculations(void) {
  struct timeval tv;
  struct timeval diff;
  long int usecs;

  if(gettimeofday(&tv, NULL))
    xyz_fatal_error("Couldn't get time when calculating!");

  if(last_process.tv_sec == 0) {
    last_process = tv;
    connector_set_process(pizza_connector_set);
    return;
  }

  xyz_timeval_minus(&diff, &tv, &last_process);
  usecs = diff.tv_sec * 1000000 + diff.tv_usec;
  if(usecs > GATE_DELAY_USECS) {
    last_process = tv;
    connector_set_process(pizza_connector_set);
    return;
  }
}

/**************** Main loop ****************/

void main_loop(void) {
  while(1) {
    no_drag = 0;
    xyz_process_events();
    process_events();
    draw();
    usleep(50);
    do_calculations();
  }
}

int main(int argc, char** argv) {
  init(argc, argv);

  main_loop();

  shutdown();

  return 0;
}
