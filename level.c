#include <stdio.h>
#include <string.h>

#include "xyz.h"

static int level_number = 0;

void sp_set_level(int level) {
  level_number = level;
}

void sp_next_level(void) {
  sp_set_level(level_number + 1);

  fprintf(stderr, "Made it to level %d!\n", level_number);
}

int sp_has_topping(const char *topping_name) {
  return 1;
}

int sp_get_n_toppings(void) {
  return 3;
}

int sp_should_be_true(int topping_mask) {
  int topping1 = !!(topping_mask & 0x1);
  int topping2 = !!(topping_mask & 0x2);
  int topping3 = !!(topping_mask & 0x4);

  int value = (topping1 || topping2) ^ topping3;

  return value;
}

const char *sp_get_end_of_level_image_filename(void) {
  return "resources/huge_happy_arms_up_shanna_400.png";
}

static xyz_image* topping_images[] = { NULL, NULL, NULL, NULL };

xyz_image** sp_get_topping_images(void) {
  if(topping_images[0]) {
    return topping_images;
  }

  topping_images[0] = xyz_load_image("resources/sausage_small_trans.png");
  topping_images[1] = xyz_load_image("resources/pepper_small_trans.png");
  topping_images[2] = xyz_load_image("resources/pineapple_small_trans.png");

  return topping_images;
}

