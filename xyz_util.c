#include "xyz.h"
#include <math.h>
#include <sys/time.h>

int xyz_point_distance(int x1, int y1, int x2, int y2) {
  int xdist = x2 - x1;
  int ydist = y2 - y1;

  return (int)sqrt(xdist * xdist + ydist * ydist);
}

void xyz_timeval_minus(struct timeval *diff, struct timeval *tv1,
		       struct timeval *tv2) {
  long int secs;
  long int usecs;

  secs = tv1->tv_sec - tv2->tv_sec;
  usecs = tv1->tv_usec - tv2->tv_usec;

  if(usecs < 0) {
    usecs += 1000000;
    secs--;
  }

  diff->tv_sec = secs;
  diff->tv_usec = usecs;
}
