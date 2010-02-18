#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xyz.h"
#include "xyz_sprite.h"
#include "pizza.h"

#define CURSOR_FILE            "images/cursor_small.png"
#define CURSOR_X_OFFSET        6
#define CURSOR_Y_OFFSET        4
/* #define DRAGGING_CURSOR_FILE   "images/dragging_cursor.png" */

void init(void) {
  xyz_start();
  xyz_set_up_screen_fullscreen(); /* (TOTAL_HEIGHT, TOTAL_WIDTH);*/

  xyz_set_key_handler(keyhandler);

  load_sprites();

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

/**************** Main loop ****************/

void main_loop(void) {
  while(1) {
    xyz_process_events();
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
