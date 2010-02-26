#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xyz.h"
#include "xyz_sprite.h"
#include "pizza.h"
#include "connectors.h"

#define CURSOR_FILE            "images/cursor_small.png"
/* #define DRAGGING_CURSOR_FILE   "images/dragging_cursor.png" */

void init(void) {
  xyz_start();
  /* xyz_set_up_screen_fullscreen(); */
  xyz_set_up_screen_window(TOTAL_HEIGHT, TOTAL_WIDTH);

  xyz_set_key_handler(keyhandler);
  xyz_set_mouse_handlers(mouse_move_handler, mouse_button_handler);

  load_sprites();
  init_connectors();

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
    xyz_color(255, 0, 0);
    xyz_rectangle_coords(from_x, from_y, from_x + 1, to_y);
    xyz_rectangle_coords(from_x, to_y, to_x, to_y + 1);
}

void draw(void) {
  char mousebox[100];
  int x, y;

  xyz_mouse_position(&x, &y);

  xyz_color(0, 0, 0);
  xyz_fill();

  /* Conveyor box */
  xyz_color(255, 0, 0);
  xyz_rectangle(0, 0, TOTAL_WIDTH, CONVEYOR_BOTTOM_HEIGHT);

  /* Mouse Coords */
  if(show_mousebox) {
    xyz_color(128, 128, 128);
    xyz_rectangle(TOTAL_WIDTH-100, TOTAL_HEIGHT-50, 100, 50);
    xyz_color(0, 0, 255);
    snprintf(mousebox, 100, "%d, %d", x, y);
    xyz_block_text(TOTAL_WIDTH-100, TOTAL_HEIGHT-50, mousebox);
  }

  /* Connection-in-progress */
  if(drag_connector) {
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

void keyhandler(const char *keyname, int down) {
  if(!strcasecmp(keyname, "q"))
    exit(0);
  if(!strcasecmp(keyname, "m") && down == 1)
    show_mousebox = !show_mousebox;
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
  if(button == 1) {
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
	    printf("Same-to-same connection.  Break wire!\n");
	  } else {
	    printf("Same-to-different connection.  Connect!\n");
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

/**************** Main loop ****************/

void main_loop(void) {
  while(1) {
    no_drag = 0;
    xyz_process_events();
    process_events();
    draw();
    usleep(50);
  }
}

int main(int argc, char** argv) {
    init();

    main_loop();

    shutdown();

    return 0;
}
