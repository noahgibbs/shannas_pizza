#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xyz_anim.h"

#include "pizza.h"

static void start_roll_one_pizza(int topping_mask,
				 int start_x, int end_x, int duration_millis);
static void start_roll_all_pizzas(int topping_mask, int start_x, int end_x,
				  int duration_millis);

static xyz_anim *roll_pizza_anim = NULL;

/*** APIs from other files ***/

void start_pizzas_rolling(void) {
  if(roll_pizza_anim) {
    start_screen_flash();
    return;
  }

  start_roll_all_pizzas(7, 5, 280, 3000);
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

void esc_to_cancel(void) {
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

static int next_all_pizzas_index(void) {
  do {
    all_pizzas_index++;
  } while(((all_pizzas_index & all_pizzas_topping_mask) == 0)
	  && all_pizzas_index < all_pizzas_topping_mask);

  if(all_pizzas_index <= all_pizzas_topping_mask)
    return all_pizzas_index;

  return -1;
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
  int idx = next_all_pizzas_index();

  if(idx == -1) {
    fprintf(stderr, "Got to the end successfully!  Yay!\n");
    return;
  }

  start_roll_one_pizza(idx, all_pizzas_start_x, all_pizzas_end_x,
		       all_pizzas_millis);
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
  int cur_x, cur_y, i;
  double cur_ratio = xyz_anim_get_current_ratio(anim);
  roll_pizza_private *priv = xyz_anim_get_private_data(anim);
  int n_toppings = sp_get_n_toppings();
  xyz_image **toppings = sp_get_topping_images();

  cur_x = (1.0 - cur_ratio) * priv->start_x + cur_ratio * priv->end_x;
  cur_y = 5;

  draw_pizza(cur_x, cur_y);

  for(i = 0; i < n_toppings; i++) {
    if(topping_x_offset[i] < 0) break;  /* Too many toppings */

    if(priv->topping_mask & (1 << i))
      xyz_draw_image(toppings[i], cur_x + topping_x_offset[i],
		     cur_y + topping_y_offset[i]);
  }

  return 0;
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
}
