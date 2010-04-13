#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xyz_anim.h"

#include "pizza.h"

static void start_roll_pizza(int n_toppings, xyz_image **toppings,
			     int start_x, int end_x, int duration_millis);

static xyz_anim *roll_pizza_anim = NULL;

void start_pizzas_rolling(int n_toppings, xyz_image **toppings) {
  if(roll_pizza_anim) {
    start_screen_flash();
  }

  start_roll_pizza(n_toppings, toppings, 5, 280, 3000);
}

typedef struct roll_pizza_private_struct {
  int start_x;
  int end_x;
  int n_toppings;
  xyz_image **toppings;
} roll_pizza_private;

int roll_pizza_create(xyz_anim *anim) {
  return 0;
}

int roll_pizza_delete(xyz_anim *anim) {
  roll_pizza_anim = NULL;
  return 0;
}

int roll_pizza_start(xyz_anim *anim) {
  return 0;
}

int roll_pizza_stop(xyz_anim *anim) {
  return 0;
}

int roll_pizza_eval(xyz_anim *anim) {
  return 0;
}

int roll_pizza_draw(xyz_anim *anim) {
  int cur_x;
  double cur_ratio = xyz_anim_get_current_ratio(anim);
  roll_pizza_private *priv = xyz_anim_get_private_data(anim);

  cur_x = (1.0 - cur_ratio) * priv->start_x + cur_ratio * priv->end_x;

  draw_pizza(cur_x, 5);

  /* TODO: draw toppings */

  return 0;
}

xyz_anim_spec roll_pizza_spec = {
  0, 2000, 0,
  roll_pizza_create, roll_pizza_delete,
  roll_pizza_start, roll_pizza_stop,
  roll_pizza_eval, roll_pizza_draw,
  sizeof(roll_pizza_private)
};

static void start_roll_pizza(int n_toppings, xyz_image **toppings,
			     int start_x, int end_x, int duration_millis) {
  roll_pizza_private *priv = NULL;

  roll_pizza_spec.duration_seconds = duration_millis / 1000;
  roll_pizza_spec.duration_milliseconds = duration_millis % 1000;
  roll_pizza_anim = xyz_anim_create(&roll_pizza_spec, NULL);
  priv = xyz_anim_get_private_data(roll_pizza_anim);
  priv->start_x = start_x;
  priv->end_x = end_x;
  priv->n_toppings = n_toppings;
  priv->toppings = toppings;
  xyz_anim_start(roll_pizza_anim);
}

void roll_pizza_refresh(void) {
  if(roll_pizza_anim)
    xyz_anim_draw(roll_pizza_anim);
}
