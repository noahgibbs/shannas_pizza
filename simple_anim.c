#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xyz_anim.h"

double screen_color[3];

int screen_flash_eval(xyz_anim *anim) {
  double current_ratio = xyz_anim_get_current_ratio(anim);
  fprintf(stderr, "Screen flash cur ratio: %g\n", current_ratio);
  screen_color[0] = 0.0;
  screen_color[1] = 0.0;
  screen_color[2] = 0.0;
  return 0;
}

int screen_flash_draw(xyz_anim *anim) {
  fprintf(stderr, "Draw screen flash!\n");
  xyz_color((char)screen_color[0],
	    (char)screen_color[1],
	    (char)screen_color[2]);
  xyz_fill();
  return 0;
}

xyz_anim *screen_background_anim = NULL;
xyz_anim_spec screen_background_spec = {
  0, 750, 0, NULL, NULL, NULL, NULL, screen_flash_eval, screen_flash_draw
};

void start_screen_flash(void) {
  fprintf(stderr, "Start screen flash!\n");

  if(screen_background_anim) {
    xyz_anim_delete(screen_background_anim);
    screen_background_anim = NULL;
  }

  xyz_anim_create(&screen_background_spec, NULL);
}

void screen_draw_background(void) {
  if(screen_background_anim) {
    xyz_anim_draw(screen_background_anim);
  } else {
    xyz_color(0, 0, 0);
    xyz_fill();
  }
}
