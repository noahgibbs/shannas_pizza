#!/bin/bash
SOURCE_FILES="pizza.c toppings.c buttons.c"
XYZ_FILES="xyz.c xyz_sprite.c xyz_events.c"
ALL_SOURCE="$SOURCE_FILES $XYZ_FILES"
LIBS="-lSDL_image -lSDL_ttf"
gcc -Wall -o pizza $ALL_SOURCE -I. `sdl-config --cflags --libs` $LIBS
