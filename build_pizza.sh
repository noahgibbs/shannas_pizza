#!/bin/bash
SOURCE_FILES="pizza.c sprites.c connectors.c toppings.c gates.c"
XYZ_FILES="xyz.c xyz_sprite.c xyz_events.c xyz_util.c"
ALL_SOURCE="$SOURCE_FILES $XYZ_FILES"
LIBS="-lSDL_image -lSDL_ttf"
OPTIONS="-Wall -g"
gcc $OPTIONS -o pizza $ALL_SOURCE -I. `sdl-config --cflags --libs` $LIBS