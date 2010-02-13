#include <stdio.h>
#include "xyz.h"
#include "xyz_sprite.h"
#include "pizza.h"

static xyz_sprite_methods methods = { NULL };

static xyz_sprite_spec toppings[] = {
  { "images/sausage_small_white.png", NULL, NULL, 50, 425, 32, 32, 1,
    &methods },
  { "images/pepper_small_white.png", NULL, NULL, 130, 450, 32, 32, 1,
    &methods },
  { "images/pineapple_small_white.png", NULL, NULL, 210, 425, 32, 32, 1,
    &methods },
  { NULL }
};

void load_toppings(void) {
  int idx = 0;

  while(toppings[idx].filename) {
    toppings[idx].sprite = sprite_from_spec(&toppings[idx]);
    if(!toppings[idx].sprite)
      xyz_fatal_error("Can't create sprite for '%s'!", toppings[idx].filename);

    idx++;
  }
}

void draw_toppings(void) {
  int idx = 0;

  while(toppings[idx].filename) {
    xyz_draw_sprite(toppings[idx].sprite);
    idx++;
  }
}

void free_toppings(void) {
  int idx = 0;

  while(toppings[idx].filename) {
    xyz_free_sprite(toppings[idx].sprite);
    toppings[idx].sprite = NULL;
    idx++;
  }
}
