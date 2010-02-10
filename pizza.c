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
  { "sausage.png", NULL },
  { "pepper.png", NULL },
  { "pineapple.png", NULL },
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

int main(int argc, char** argv) {
    printf("Initializing SDL.\n");

    xyz_start();

    printf("SDL initialized.\n");

    xyz_set_up_screen();

    load_toppings();

    printf("Quitting SDL.\n");

    xyz_end();

    return 0;
}
