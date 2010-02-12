#include <stdio.h>
#include "xyz.h"
#include "pizza.h"

typedef struct {
  const char *filename;
  xyz_sprite *sprite;
  xyz_image *image;
  int x;
  int y;
  int width;
  int height;
} topping;

static topping toppings[] = {
  { "images/sausage_small_white.png", NULL, NULL, 0, SYMBOLS_TOP_HEIGHT },
  { "images/pepper_small_white.png", NULL, NULL, 64, SYMBOLS_TOP_HEIGHT },
  { "images/pineapple_small_white.png", NULL, NULL, 128, SYMBOLS_TOP_HEIGHT },
  { NULL, NULL }
};

void load_toppings(void) {
  int idx = 0;

  while(toppings[idx].filename) {
    toppings[idx].image = xyz_load_image(toppings[idx].filename);
    if(!toppings[idx].image)
      xyz_fatal_error("Can't load image '%s'!", toppings[idx].filename);
    toppings[idx].sprite = xyz_new_sprite(toppings[idx].x, toppings[idx].y,
					  SYMBOLS_WIDTH, SYMBOLS_HEIGHT,
					  toppings[idx].image);
    if(!toppings[idx].sprite)
      xyz_fatal_error("Can't create sprite for '%s'!", toppings[idx].filename);
    xyz_sprite_set_draggable(toppings[idx].sprite, 1);

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
    xyz_free_image(toppings[idx].image);
    toppings[idx].image = NULL;
    idx++;
  }
}
