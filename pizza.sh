#!/bin/bash
SOURCE_FILES="pizza.c toppings.c xyz.c xyz_sprite.c xyz_events.c"
LIBS="-lSDL_image -lSDL_ttf"
gcc -Wall -o pizza $SOURCE_FILES -I. `sdl-config --cflags --libs` $LIBS
