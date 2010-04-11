#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xyz_anim.h"

double screen_color[3];
xyz_anim *screen_background_anim = NULL;

int screen_flash_eval(xyz_anim *anim) {
  double current_ratio = xyz_anim_get_current_ratio(anim);
  screen_color[0] = 255.0 * (1.0 - current_ratio);
  screen_color[1] = 0.0;
  screen_color[2] = 0.0;
  return 0;
}

int screen_flash_draw(xyz_anim *anim) {
  xyz_color((char)screen_color[0],
	    (char)screen_color[1],
	    (char)screen_color[2]);
  xyz_fill();
  return 0;
}

int screen_flash_delete(xyz_anim *anim) {
  screen_background_anim = NULL;
  return 0;
}

xyz_anim_spec screen_background_spec = {
  0, 250, 0, NULL, screen_flash_delete, NULL, NULL, screen_flash_eval, screen_flash_draw
};

void start_screen_flash(void) {
  if(screen_background_anim) {
    xyz_anim_delete(screen_background_anim);
    screen_background_anim = NULL;
  }

  screen_background_anim = xyz_anim_create(&screen_background_spec, NULL);
  xyz_anim_start(screen_background_anim);
}

void screen_draw_background(void) {
  if(screen_background_anim) {
    xyz_anim_draw(screen_background_anim);
  } else {
    xyz_color(0, 0, 0);
    xyz_fill();
  }
}
