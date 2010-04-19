#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xyz_anim.h"

#include "pizza.h"

/*** Animation to show a block of text briefly ***/

typedef struct {
  const char *text;

  /* Text location */
  int x;
  int y;

  /* Background block of color location */
  int bg_x;
  int bg_y;
  int bg_w;
  int bg_h;

  /* Background color */
  int bg_r;
  int bg_g;
  int bg_b;
} show_text_private;

static int show_text_create(xyz_anim *anim) {
  return 0;
}

static int show_text_delete(xyz_anim *anim) {
  return 0;
}

static int show_text_draw(xyz_anim *anim) {
  show_text_private *priv = (show_text_private*)xyz_anim_get_private_data(anim);
  xyz_color(priv->bg_r, priv->bg_g, priv->bg_b);
  xyz_rectangle(priv->bg_x, priv->bg_y, priv->bg_w, priv->bg_h);

  xyz_block_text(priv->x, priv->y, priv->text);

  return 0;
}

xyz_anim_spec show_text_spec = {
  0, 3000, 0,
  show_text_create, show_text_delete,
  NULL, NULL, NULL, show_text_draw,
  sizeof(show_text_private)
};

xyz_anim* show_text_anim(int duration_millis,
			 const char *text,
			 int x, int y) {
  xyz_anim *tmp;
  show_text_private *priv;

  show_text_spec.duration_seconds = duration_millis / 1000;
  show_text_spec.duration_milliseconds = duration_millis % 1000;
  tmp = xyz_anim_create(&show_text_spec, NULL);
  priv = (show_text_private*)xyz_anim_get_private_data(tmp);
  priv->text = text;

  priv->x = x;
  priv->y = y;

  /* Gray background */
  priv->bg_r = 0xc0;
  priv->bg_g = priv->bg_r;
  priv->bg_b = priv->bg_r;

  priv->bg_x = x - 10;
  priv->bg_y = y - 10;
  priv->bg_w = TOTAL_WIDTH - 2 * priv->bg_x;
  priv->bg_h = 100;

  /* xyz_anim_start(tmp); */
  return tmp;
}
