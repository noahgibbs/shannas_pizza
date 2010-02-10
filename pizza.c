#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    printf("Initializing SDL.\n");

    xyz_start();

    printf("SDL initialized.\n");

    printf("Quitting SDL.\n");

    xyz_end();

    return 0;
}
