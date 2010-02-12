#!/bin/bash
SOURCE_FILES="pizza.c toppings.c xyz.c xyz_sprite.c"
gcc -Wall -o pizza $SOURCE_FILES -I. `sdl-config --cflags --libs` -lSDL_image
