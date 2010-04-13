#include "xyz_anim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define ANIM_CREATED    1
#define ANIM_STARTED    2
#define ANIM_STOPPED    3

struct _xyz_anim_t {
  /* TODO: add state - started, stopped, etc */
  struct timeval start_time;
  struct timeval current_time;
  double current_ratio;
  struct timeval duration;
  int repeat;
  int n_variables;
  xyz_variable **variables;
  void *user_info;
  xyz_anim *next;
  xyz_anim *prev;
  int state;
  void *private_data;

  xyz_anim_func construct;
  xyz_anim_func destroy;
  xyz_anim_func start;
  xyz_anim_func stop;
  xyz_anim_func evaluate;
  xyz_anim_func draw;
};

static xyz_anim *head = NULL;
static xyz_anim *tail = NULL;

void xyz_anim_periodic(void) {
  xyz_anim *index = head;
  xyz_anim *index_next = NULL;
  long millisecs;
  struct timeval current;
  struct timeval diff;

  if(gettimeofday(&current, NULL)) {
    xyz_fatal_error("Can't get time when evaluating animation!");
  }

  while(index) {
    index_next = index->next;

    if(index->state == ANIM_STARTED) {
      xyz_timeval_minus(&diff, &current, &index->start_time);
      millisecs = diff.tv_usec / 1000;
      xyz_anim_frame(index, diff.tv_sec, millisecs);
    }

    index = index_next;
  }
}

void xyz_anim_delete_all(void) {
  xyz_anim *index = head;
  xyz_anim *index_next = NULL;

  while(index) {
    index_next = index->next;
    xyz_anim_delete(index);
    index = index_next;
  }
}

xyz_anim* xyz_anim_create(xyz_anim_spec *spec, void *user_info) {
  xyz_anim *ret = xyz_new(xyz_anim);

  ret->construct = spec->construct;
  ret->destroy = spec->destroy;
  ret->start = spec->start;
  ret->stop = spec->stop;
  ret->evaluate = spec->evaluate;
  ret->draw = spec->draw;

  ret->duration.tv_sec = spec->duration_seconds;
  ret->duration.tv_usec = spec->duration_milliseconds * 1000;
  ret->repeat = spec->repeat;

  ret->user_info = user_info;
  ret->private_data = NULL;
  if(spec->private_data_size > 0) {
    ret->private_data = calloc(1, spec->private_data_size);
  }

  gettimeofday(&ret->current_time, NULL);
  ret->current_ratio = 0.0;

  if(head) head->prev = ret;
  ret->next = head;
  head = ret;
  if(!tail) tail = ret;

  if(ret->construct)
    ret->construct(ret);

  ret->state = ANIM_CREATED;

  return ret;
}

void* xyz_anim_get_user_info(xyz_anim *anim) {
  return anim->user_info;
}

void* xyz_anim_get_private_data(xyz_anim *anim) {
  return anim->private_data;
}

double xyz_anim_get_current_ratio(xyz_anim *anim) {
  return anim->current_ratio;
}

void xyz_anim_start(xyz_anim *anim) {
  gettimeofday(&anim->start_time, NULL);
  gettimeofday(&anim->current_time, NULL);
  anim->current_ratio = 0.0;

  if(anim->start)
    anim->start(anim);

  anim->state = ANIM_STARTED;
}

void xyz_anim_evaluate(xyz_anim *anim) {
  if(anim->evaluate)
    anim->evaluate(anim);
}

void xyz_anim_frame(xyz_anim *anim, long seconds, long milliseconds) {
  long current_milli;
  long duration_milli;

  if(seconds > anim->duration.tv_sec ||
     (seconds == anim->duration.tv_sec && milliseconds >
      (anim->duration.tv_usec / 1000))) {
    /* Exceeded duration */
    if(anim->repeat) {
      xyz_anim_start(anim);
    } else {
      xyz_anim_delete(anim);
    }
    return;
  }

  anim->current_time.tv_sec = seconds;
  anim->current_time.tv_usec = milliseconds * 1000;
  current_milli = seconds * 1000 + milliseconds;
  duration_milli = anim->duration.tv_sec * 1000 +
    (anim->duration.tv_usec / 1000);
  anim->current_ratio = (double)current_milli / duration_milli;

  if(anim->evaluate)
    anim->evaluate(anim);
}

void xyz_anim_draw(xyz_anim *anim) {
  if(anim->draw)
    anim->draw(anim);
}

void xyz_anim_stop(xyz_anim *anim) {
  if(anim->stop)
    anim->stop(anim);

  anim->state = ANIM_STOPPED;
}

void xyz_anim_delete(xyz_anim *anim) {
  int i;

  if(anim->destroy)
    anim->destroy(anim);

  anim->state = 0;  /* Invalid */

  if(anim->private_data)
    free(anim->private_data);

  /* Destroy variables */
  for(i = 0; i < anim->n_variables; i++) {
    free(anim->variables[i]);
  }
  if(anim->variables) free(anim->variables);

  if(anim->prev)
    anim->prev->next = anim->next;
  if(anim->next)
    anim->next->prev = anim->prev;
  if(tail == anim)
    tail = anim->prev;
  if(head == anim)
    head = anim->next;

  free(anim);
}

void xyz_anim_add_variable(xyz_anim *anim, xyz_variable *variable) {
  anim->n_variables++;
  anim->variables = realloc(anim->variables, sizeof(xyz_variable*) * anim->n_variables);
}
