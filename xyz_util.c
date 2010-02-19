#include "xyz.h"
#include <math.h>

int xyz_point_distance(int x1, int y1, int x2, int y2) {
  int xdist = x2 - x1;
  int ydist = y2 - y1;

  return (int)sqrt(xdist * xdist + ydist * ydist);
}
