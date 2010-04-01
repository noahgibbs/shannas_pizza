#!/bin/bash
cd ..
SOURCE_FILES="pizza.c sprites.c connectors.c toppings.c gates.c"
XYZ_FILES="xyz.c xyz_sprite.c xyz_events.c xyz_util.c xyz_variable.c"
ALL_SOURCE="$SOURCE_FILES $XYZ_FILES"
LIBS="-lSDL_image -lSDL_ttf -lSDL_gfx"
OPTIONS="-Wall -g"
gcc $OPTIONS -o win32/pizza $ALL_SOURCE -I. -Lwin32/dlls `sdl-config --cflags --libs` $LIBS
