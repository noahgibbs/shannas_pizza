#include "xyz.h"
#include <math.h>

int xyz_point_distance(int x1, int y1, int x2, int y2) {
  int xdist = x2 - x1;
  int ydist = y2 - y1;

  return (int)sqrt(xdist * xdist + ydist * ydist);
}

void xyz_timespec_minus(struct timespec *diff, struct timespec *ts1,
			struct timespec *ts2) {
  long int secs;
  long int nsecs;

  secs = ts1->tv_sec - ts2->tv_sec;
  nsecs = ts1->tv_nsec - ts2->tv_nsec;

  if(nsecs < 0) {
    nsecs += 1000000000;
    secs--;
  }

  diff->tv_sec = secs;
  diff->tv_nsec = nsecs;
}
