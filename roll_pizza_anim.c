#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xyz_anim.h"

#include "pizza.h"

#define ROLL_PIZZA_X_START         5
#define ROLL_PIZZA_X_END         280
#define ROLL_PIZZA_Y               5
#define ROLL_PIZZA_DURATION     3000
#define ROLL_PIZZA_PAUSE        1000

#define PASS_DURATION_MILLIS    1500
#define FAIL_DURATION_MILLIS    1500

static void start_roll_one_pizza(int topping_mask,
				 int start_x, int end_x, int duration_millis);
static void start_pause_one_pizza(int duration_millis);
static void start_pass_one_pizza();
static void start_fail_one_pizza();
static void start_roll_all_pizzas(int topping_mask, int start_x, int end_x,
				  int duration_millis);
static void draw_pizza_with_toppings(int cur_x, int cur_y, int topping_mask);
static void fail_pizzas(void);
static void show_shanna_pass(void);
static void show_shanna_fail(void);

static xyz_anim *roll_pizza_anim = NULL;

/*** APIs from other files ***/

void start_pizzas_rolling(void) {
  if(roll_pizza_anim) {
    start_screen_flash();
    return;
  }

  start_roll_all_pizzas(7, ROLL_PIZZA_X_START, ROLL_PIZZA_X_END,
			ROLL_PIZZA_DURATION);
}

void roll_pizza_refresh(void) {
  if(roll_pizza_anim)
    xyz_anim_draw(roll_pizza_anim);
}

int pizza_is_rolling(void) {
  if(roll_pizza_anim)
    return 1;

  return 0;
}

void esc_to_cancel_pizza(void) {
  /* TODO: skip to/past judgement, no just animation */
  if(roll_pizza_anim) {
    xyz_anim_delete(roll_pizza_anim);
  }
}

/*** Tracking code to roll all pizzas, one after another ***/

static int all_pizzas_topping_mask = -1;
static int all_pizzas_index = -1;
static int all_pizzas_start_x = -1;
static int all_pizzas_end_x = -1;
static int all_pizzas_millis = -1;

static int all_pizzas_state = -1;

#define STATE_NONE     0
#define STATE_ROLLING  1
#define STATE_PAUSED   2
#define STATE_FAILED   3
#define STATE_PASSED   4
#define STATE_SHANNA   5

static int next_all_pizzas_index(void) {
  do {
    all_pizzas_index++;
  } while(((all_pizzas_index & all_pizzas_topping_mask) == 0)
	  && all_pizzas_index < all_pizzas_topping_mask);

  if(all_pizzas_index <= all_pizzas_topping_mask)
    return all_pizzas_index;

  return -1;
}

int topping_is_rolling(int topping_number) {
  return !!(all_pizzas_index & (1 << topping_number));
}

static void start_roll_all_pizzas(int topping_mask, int start_x, int end_x,
				  int duration_millis) {
  int idx;

  all_pizzas_topping_mask = topping_mask;
  all_pizzas_index = -1;
  all_pizzas_start_x = start_x;
  all_pizzas_end_x = end_x;
  all_pizzas_millis = duration_millis;

  idx = next_all_pizzas_index();
  if(idx == -1) {
    xyz_fatal_error("No valid indexes, but we're still rolling out pizzas!");
  }

  start_roll_one_pizza(idx, start_x, end_x, duration_millis);
}

static void roll_pizza_finished(void) {
  start_pause_one_pizza(ROLL_PIZZA_PAUSE);
}

static void pause_pizza_finished(void) {
  /* TODO: Check pass/fail */
  start_pass_one_pizza();
}

static void fail_pizza_finished(void) {
  roll_pizza_anim = NULL;

  /* show_shanna_fail(); */

  fail_pizzas();
}

static void pass_pizza_finished(void) {
  int idx = next_all_pizzas_index();

  roll_pizza_anim = NULL;
  if(idx == -1) {
    fprintf(stderr, "Got to the end successfully!  Yay!\n");
    return;
  }

  start_roll_one_pizza(idx, all_pizzas_start_x, all_pizzas_end_x,
		       all_pizzas_millis);

  /* show_shanna_pass(); */
}

static void fail_pizzas(void) {
  all_pizzas_state = STATE_NONE;
  all_pizzas_index = -1;
}

/*** 'Animation' to pause ***/

static int pause_pizza_delete(xyz_anim *anim) {
  pause_pizza_finished();
  return 0;
}

static int pause_pizza_draw(xyz_anim *anim) {
  draw_pizza_with_toppings(ROLL_PIZZA_X_END, ROLL_PIZZA_Y, all_pizzas_index);
  return 0;
}

xyz_anim_spec pause_pizza_spec = {
  0, 2000, 0,
  NULL, pause_pizza_delete,
  NULL, NULL, NULL, pause_pizza_draw,
  0
};

static void start_pause_one_pizza(int duration_millis) {
  pause_pizza_spec.duration_seconds = duration_millis / 1000;
  pause_pizza_spec.duration_milliseconds = duration_millis % 1000;
  roll_pizza_anim = xyz_anim_create(&pause_pizza_spec, NULL);
  xyz_anim_start(roll_pizza_anim);

  all_pizzas_state = STATE_PAUSED;
}

/*** Animation to roll a single pizza a short distance ***/

typedef struct roll_pizza_private_struct {
  int start_x;
  int end_x;
  int topping_mask;
} roll_pizza_private;

static int roll_pizza_create(xyz_anim *anim) {
  return 0;
}

static int roll_pizza_delete(xyz_anim *anim) {
  roll_pizza_anim = NULL;
  roll_pizza_finished();
  return 0;
}

static int roll_pizza_start(xyz_anim *anim) {
  return 0;
}

static int roll_pizza_stop(xyz_anim *anim) {
  return 0;
}

static int roll_pizza_eval(xyz_anim *anim) {
  return 0;
}

static int topping_x_offset[] = { 10, 50, 25, -1 };
static int topping_y_offset[] = { 25, 10, 60, -1 };

static int roll_pizza_draw(xyz_anim *anim) {
  int cur_x, cur_y;
  double cur_ratio = xyz_anim_get_current_ratio(anim);
  roll_pizza_private *priv = xyz_anim_get_private_data(anim);

  cur_x = (1.0 - cur_ratio) * priv->start_x + cur_ratio * priv->end_x;
  cur_y = ROLL_PIZZA_Y;

  draw_pizza_with_toppings(cur_x, cur_y, priv->topping_mask);

  return 0;
}

static void draw_pizza_with_toppings(int cur_x, int cur_y,
				     int topping_mask) { 
  int n_toppings = sp_get_n_toppings();
  xyz_image **toppings = sp_get_topping_images();
  int i;

  draw_pizza(cur_x, cur_y);

  for(i = 0; i < n_toppings; i++) {
    if(topping_x_offset[i] < 0) break;  /* Too many toppings */

    if(topping_mask & (1 << i))
      xyz_draw_image(toppings[i], cur_x + topping_x_offset[i],
		     cur_y + topping_y_offset[i]);
  }

}

xyz_anim_spec roll_pizza_spec = {
  0, 2000, 0,
  roll_pizza_create, roll_pizza_delete,
  roll_pizza_start, roll_pizza_stop,
  roll_pizza_eval, roll_pizza_draw,
  sizeof(roll_pizza_private)
};

static void start_roll_one_pizza(int topping_mask, int start_x, int end_x,
				 int duration_millis) {
  roll_pizza_private *priv = NULL;

  roll_pizza_spec.duration_seconds = duration_millis / 1000;
  roll_pizza_spec.duration_milliseconds = duration_millis % 1000;
  roll_pizza_anim = xyz_anim_create(&roll_pizza_spec, NULL);
  priv = xyz_anim_get_private_data(roll_pizza_anim);
  priv->start_x = start_x;
  priv->end_x = end_x;
  priv->topping_mask = topping_mask;
  xyz_anim_start(roll_pizza_anim);

  all_pizzas_state = STATE_ROLLING;
}

/*** Animation for pizza found worthy ***/

static int pass_pizza_delete(xyz_anim *anim) {
  pass_pizza_finished();
  return 0;
}

static int pass_pizza_draw(xyz_anim *anim) {
  double cur_ratio = xyz_anim_get_current_ratio(anim);
  int segment = (int)(cur_ratio * 8.0);

  if(segment % 2)
    draw_pizza_with_toppings(ROLL_PIZZA_X_END, ROLL_PIZZA_Y, all_pizzas_index);

  return 0;
}

xyz_anim_spec pass_pizza_spec = {
  0, 1500, 0,
  NULL, pass_pizza_delete,
  NULL, NULL, NULL, pass_pizza_draw,
  0
};

static void start_pass_one_pizza(void) {
  int duration_millis = PASS_DURATION_MILLIS;
  pass_pizza_spec.duration_seconds = duration_millis / 1000;
  pass_pizza_spec.duration_milliseconds = duration_millis % 1000;
  roll_pizza_anim = xyz_anim_create(&pass_pizza_spec, NULL);
  xyz_anim_start(roll_pizza_anim);

  all_pizzas_state = STATE_PASSED;
}

/*** Animation for pizza found unworthy ***/

static int fail_pizza_delete(xyz_anim *anim) {
  fail_pizza_finished();
  return 0;
}

static int fail_pizza_draw(xyz_anim *anim) {
  double cur_ratio = xyz_anim_get_current_ratio(anim);
  int segment = (int)(cur_ratio * 8.0);

  draw_pizza_with_toppings(ROLL_PIZZA_X_END, ROLL_PIZZA_Y, all_pizzas_index);

  if(segment % 2)
    draw_red_x(ROLL_PIZZA_X_END, ROLL_PIZZA_Y);

  return 0;
}

xyz_anim_spec fail_pizza_spec = {
  0, 1500, 0,
  NULL, fail_pizza_delete,
  NULL, NULL, NULL, fail_pizza_draw,
  0
};

static void start_fail_one_pizza() {
  int duration_millis = FAIL_DURATION_MILLIS;
  fail_pizza_spec.duration_seconds = duration_millis / 1000;
  fail_pizza_spec.duration_milliseconds = duration_millis % 1000;
  roll_pizza_anim = xyz_anim_create(&fail_pizza_spec, NULL);
  xyz_anim_start(roll_pizza_anim);

  all_pizzas_state = STATE_FAILED;
}

/*** Animation for Shanna getting pizza ***/

static int shanna_pass_pizza_delete(xyz_anim *anim) {
  return 0;
}

static int shanna_pass_pizza_draw(xyz_anim *anim) {
  double cur_ratio = xyz_anim_get_current_ratio(anim);
  int segment = (int)(cur_ratio * 8.0);

  draw_pizza_with_toppings(ROLL_PIZZA_X_END, ROLL_PIZZA_Y, all_pizzas_index);

  if(segment % 2)
    draw_red_x(ROLL_PIZZA_X_END, ROLL_PIZZA_Y);

  return 0;
}

xyz_anim_spec shanna_pass_pizza_spec = {
  0, 1500, 0,
  NULL, shanna_pass_pizza_delete,
  NULL, NULL, NULL, shanna_pass_pizza_draw,
  0
};

static void start_shanna_pass_one_pizza(int duration_millis) {
  xyz_anim *tmp;

  shanna_pass_pizza_spec.duration_seconds = duration_millis / 1000;
  shanna_pass_pizza_spec.duration_milliseconds = duration_millis % 1000;
  tmp = xyz_anim_create(&shanna_pass_pizza_spec, NULL);
  xyz_anim_start(tmp);
}
