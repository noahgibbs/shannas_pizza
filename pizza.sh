#!/bin/bash
SOURCE_FILES="pizza.c sprites.c connectors.c toppings.c"
XYZ_FILES="xyz.c xyz_sprite.c xyz_events.c"
ALL_SOURCE="$SOURCE_FILES $XYZ_FILES"
LIBS="-lSDL_image -lSDL_ttf"
OPTIONS="-Wall -O -g"
gcc $OPTIONS -o pizza $ALL_SOURCE -I. `sdl-config --cflags --libs` $LIBS
