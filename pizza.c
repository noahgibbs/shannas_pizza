#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xyz.h"
#include "pizza.h"

void init(void) {
  xyz_start();
  xyz_set_up_screen(TOTAL_HEIGHT, TOTAL_WIDTH);

  load_toppings();
}

void draw(void) {
  xyz_color(0, 0, 0);
  xyz_rectangle(0, 0, TOTAL_HEIGHT, TOTAL_WIDTH);
  draw_toppings();
  xyz_done_drawing();
}

void shutdown(void) {
  free_toppings();
  xyz_end();
}

/************ Mouse processing **************/

static xyz_sprite* selected_sprite = NULL;

void mouse_button_down(int button, int x, int y) {
  if(button == 1) {
    selected_sprite = xyz_intersect_draggable_sprite(x, y);
  }
  /* button 3 is right button */
}

void mouse_button_up(int button, int x, int y) {
  if(button == 1) {
    selected_sprite = NULL;
  }
  /* button 3 is right button */
}

static int old_button1 = 0;

void process_mouse(void) {
  int x, y;
  int button1;

  xyz_mouse_position(&x, &y);
  button1 = xyz_mouse_is_button_down(1);
  if(button1 != old_button1) {
    if(button1) {
      mouse_button_down(1, x, y);
    } else {
      mouse_button_up(1, x, y);
    }
    old_button1 = button1;
  }

  if(selected_sprite) {
    xyz_sprite_set_x(selected_sprite, x);
    xyz_sprite_set_y(selected_sprite, y);
  }
}

/**************** Main loop ****************/

void main_loop(void) {
  while(1) {
    xyz_process_events();
    process_mouse();
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
