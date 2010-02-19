/* Connector objects, input and output objects, all identified by pointers */

#include "connectors.h"

/* Static objects for individual signals. */
static int signal_one = 0;
static int signal_zero = 0;

void *get_signal_one(void) {
  return (void*)&signal_one;
}

void *get_signal_zero(void) {
  return (void*)&signal_zero;
}

void init_connectors(void) {

}

void shutdown_connectors(void) {

}
