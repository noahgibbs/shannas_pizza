#include "pizza.h"
#include "xyz_sprite.h"

static sprite_spec buttons[] = {
  {"images/go_button.png", NULL, NULL,
   TOOLBOX_LEFT_WIDTH, CONVEYOR_BOTTOM_HEIGHT, 100, 50, 0},
  {NULL}
};

void load_buttons(void) {
  int idx = 0;

  while(buttons[idx].filename) {
    buttons[idx].sprite = sprite_from_spec(&buttons[idx]);
    if(!buttons[idx].sprite)
      xyz_fatal_error("Can't create sprite for '%s'!", buttons[idx].filename);
    idx++;
  }
}

void draw_buttons(void) {
  int idx = 0;

  while(buttons[idx].filename) {
    xyz_draw_sprite(buttons[idx].sprite);
    idx++;
  }
}

void free_buttons(void) {
  int idx = 0;

  while(buttons[idx].filename) {
    xyz_free_sprite(buttons[idx].sprite);
    buttons[idx].sprite = NULL;
    idx++;
  }
}
