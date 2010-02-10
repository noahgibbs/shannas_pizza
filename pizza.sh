#!/bin/bash
gcc -Wall -o pizza pizza.c xyz.c -I. `sdl-config --cflags --libs` -lSDL_image
