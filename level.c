#include <stdio.h>
#include <string.h>

#include "xyz.h"
#include "xyz_anim.h"

#include "pizza.h"

/* This is actually a level offset */
static int level_number = 0;

/* More than we're likely to need */
#define MAX_NUM_TOPPINGS 10

typedef struct level_spec_struct {
  int level_num;

  /* This is a mask into the global toppings table for
     the toppings used in this level. */
  int topping_mask;

  int (*should_be_true)(int topping_mask);

  const char *instruction_text;

  /* Calculated from the above: */
  int num_toppings;
} level_spec;

static int level_one_sbt(int topping_mask) {
  return !!(topping_mask & 0x4);
}

static int level_two_sbt(int topping_mask) {
  int topping1 = !!(topping_mask & 0x1);
  int topping2 = !!(topping_mask & 0x2);

  return topping1 || topping2;
}

#if 0
static int level_ten_sbt(int topping_mask) {
  int topping1 = !!(topping_mask & 0x1);
  int topping2 = !!(topping_mask & 0x2);
  int topping3 = !!(topping_mask & 0x4);

  int value = (topping1 || topping2) ^ topping3;

  return value;
}
#endif

static int highest_level = -1;

level_spec levels[] = {
  {1, 0x4, level_one_sbt, "Shanna only wants pizza with "
   "pineapple right now!"},
  {2, 0x7, level_two_sbt, "Shanna wants pepper or sausage!"},
  {-1}
};

static int count_bits(int number) {
  int ret = 0;
  while(number > 0) {
    if(number & 0x1) ret++;
    number >>= 1;
  }
  return ret;
}

void sp_init(void) {
  int i;

  for(i = 0; ; i++) {
    if(levels[i].level_num < 0) {
      highest_level = i - 1;
      break;
    }

    levels[i].num_toppings = count_bits(levels[i].topping_mask);
  }
}

void sp_set_level(int level) {
  if(level > highest_level)
    level = highest_level;

  level_number = level;
}

void sp_next_level(void) {
  sp_set_level(level_number + 1);

  fprintf(stderr, "Made it to level %d!\n", levels[level_number].level_num);
}

static xyz_anim *intro_anim = NULL;

static int intro_delete_hook(xyz_anim *anim) {
  intro_anim = NULL;
  return 0;
}

void sp_start_level(void) {
  if(intro_anim)
    xyz_anim_delete(intro_anim);

  intro_anim = sp_get_intro_animation();

  if(intro_anim) {
    xyz_anim_start(intro_anim);
    xyz_anim_set_delete_hook(intro_anim, intro_delete_hook);
  }
}

void sp_refresh(void) {
  if(intro_anim) {
    xyz_anim_draw(intro_anim);
  }
}

xyz_anim* sp_get_intro_animation(void) {
  return show_text_anim(INSTRUCTION_TEXT_DURATION_MILLIS,
			levels[level_number].instruction_text,
			INSTRUCTION_TEXT_LEFT_WIDTH,
			INSTRUCTION_TEXT_TOP_HEIGHT);
}

int sp_get_n_toppings(void) {
  return levels[level_number].num_toppings;
}

int sp_get_topping_mask(void) {
  return levels[level_number].topping_mask;
}

int sp_should_be_true(int topping_mask) {
  if(levels[level_number].should_be_true)
    return levels[level_number].should_be_true(topping_mask);
  fprintf(stderr, "No should_be_true defined for this level!\n");
  return -1;
}

const char *sp_get_end_of_level_image_filename(void) {
  return "resources/huge_happy_arms_up_shanna_400.png";
}

static xyz_image* topping_images[] = { NULL, NULL, NULL, NULL };

xyz_image** sp_get_topping_images(void) {
  if(topping_images[0]) return topping_images;

  if(!topping_images[0]) {
    topping_images[0] = xyz_load_image("resources/sausage_small_trans.png");
    topping_images[1] = xyz_load_image("resources/pepper_small_trans.png");
    topping_images[2] = xyz_load_image("resources/pineapple_small_trans.png");
  }

  return topping_images;
}
