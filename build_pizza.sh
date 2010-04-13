#!/bin/bash
SOURCE_FILES="pizza.c sprites.c connectors.c toppings.c gates.c simple_anim.c roll_pizza_anim.c level.c"
XYZ_FILES="xyz.c xyz_sprite.c xyz_events.c xyz_util.c xyz_variable.c xyz_anim.c"
ALL_SOURCE="$SOURCE_FILES $XYZ_FILES"
LIBS="-lSDL_image -lSDL_ttf -lSDL_gfx"
OPTIONS="-Wall -g"
SDL_CONFIG=`sdl-config --cflags --libs`
gcc $OPTIONS -o pizza $ALL_SOURCE -I. $SDL_CONFIG $LIBS
