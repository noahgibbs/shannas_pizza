#include <stdio.h>
#include <stdlib.h>

#include "xyz.h"

typedef struct {
  const char *filename;
  xyz_image *image;
  int x;
  int y;
} topping;

static topping toppings[] = {
  { "sausage.png", NULL, 0, 0 },
  { "pepper.png", NULL, 128, 128 },
  { "pineapple.png", NULL, 0, 128 },
  { NULL, NULL }
};

void load_toppings(void) {
  int idx = 0;

  while(toppings[idx].filename) {
    toppings[idx].image = xyz_load_image(toppings[idx].filename);
    if(!toppings[idx].image)
      xyz_fatal_error("Can't load image '%s'!", toppings[idx].filename);
    idx++;
  }
}

void draw_toppings(void) {
  int idx = 0;

  while(toppings[idx].filename) {
    xyz_draw_image(toppings[idx].image, toppings[idx].x, toppings[idx].y);
    idx++;
  }
}

void free_toppings(void) {
  int idx = 0;

  while(toppings[idx].filename) {
    xyz_free_image(toppings[idx].image);
    toppings[idx].image = NULL;
    idx++;
  }
}

int main(int argc, char** argv) {
    printf("Initializing SDL.\n");

    xyz_start();

    printf("SDL initialized.\n");

    xyz_set_up_screen();

    load_toppings();
    draw_toppings();
    sleep(2);

    printf("Quitting SDL.\n");

    free_toppings();
    xyz_end();

    return 0;
}
