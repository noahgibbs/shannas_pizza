#!/bin/bash
CONN_SOURCE="../connectors.c test_conn.c"
TEST_SOURCE="test.c"
OPTIONS="-Wall -O -g"
INC="-I. -I.."
gcc $OPTIONS -o test_conn $CONN_SOURCE $TEST_SOURCE $INC

XYZ_TEST_SOURCE="test_xyz.c"
XYZ_SOURCE="../xyz.c ../xyz_sprite.c ../xyz_events.c ../xyz_util.c ../xyz_variable.c"
SDL_CONFIG=`sdl-config --cflags --libs`
LIBS="-lSDL_image -lSDL_ttf -lSDL_gfx"
gcc $OPTIONS -o test_xyz $XYZ_TEST_SOURCE $XYZ_SOURCE $TEST_SOURCE $SDL_CONFIG $INC $LIBS
