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

void draw(void) {
  char mousebox[100];
  int x, y;

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
    xyz_mouse_position(&x, &y);
    snprintf(mousebox, 100, "%d, %d", x, y);
    xyz_block_text(TOTAL_WIDTH-100, TOTAL_HEIGHT-50, mousebox);
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

static xyz_sprite* dragged_sprite = NULL;
static int selected_x_offset;
static int selected_y_offset;

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

int mouse_button_handler(int button, int is_down) {
  if(button == 1) {
    int x, y;

    if(!is_down) { dragged_sprite = NULL; return 0; }

    xyz_mouse_position(&x, &y);
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
    if(xyz_sprite_subscribes_to(dragged_sprite, XYZ_SPRITE_MOVED)) {
      xyz_sprite_event *event = xyz_sprite_event_new(dragged_sprite);
      event->type = XYZ_SPRITE_MOVED;
      event->button = 1;
      event->mouse_x = x;
      event->mouse_y = y;
      xyz_sprite_handle_event(dragged_sprite, event);
    }
  }
}

/**************** Main loop ****************/

void main_loop(void) {
  while(1) {
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
