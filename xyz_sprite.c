#include <string.h>
#include "xyz.h"
#include "xyz_sprite.h"

/*********** xyz_sprite ****************/

struct _xyz_sprite_t {
  int x;
  int y;
  int width;
  int height;
  xyz_image *image;
  int draggable;
  int own_image;
  unsigned char subscribed_events[XYZ_SPRITE_MAXEVENT];

  xyz_sprite_methods *methods;
  void *user_info;

  struct _xyz_sprite_t *next;
  struct _xyz_sprite_t *prev;

  xyz_sprite_event *events_head;
  xyz_sprite_event *events_tail;
};

static xyz_sprite *sprite_head = NULL;
static xyz_sprite *sprite_tail = NULL;

xyz_sprite *xyz_new_sprite(int x, int y, int width, int height,
			   xyz_image *image) {
  xyz_sprite *tmp = NULL;

  tmp = calloc(sizeof(xyz_sprite), 1);
  if(!tmp) xyz_fatal_error("Could not allocate structure for sprite!");

  tmp->x = x; tmp->y = y;
  tmp->width = width; tmp->height = height;
  tmp->image = image;

  memset(tmp->subscribed_events, 0, XYZ_SPRITE_MAXEVENT);

  /* Put tmp at tail of doubly-linked list */
  tmp->prev = sprite_tail;
  tmp->next = NULL;
  if(sprite_tail) sprite_tail->next = tmp;
  sprite_tail = tmp;
  if(!sprite_head) sprite_head = tmp;

  return tmp;
}

xyz_sprite* xyz_sprite_from_spec(xyz_sprite_spec *spec) {
  xyz_image *image = NULL;
  xyz_sprite *sprite;
  if(spec->filename && spec->filename[0] != 0) {
    image = xyz_load_image(spec->filename);
    if(!image)
      xyz_fatal_error("Couldn't load image '%s'!", spec->filename);
  }
  sprite = xyz_new_sprite(spec->x, spec->y, spec->width, spec->height, image);
  if(!sprite)
    xyz_fatal_error("Couldn't create sprite for image '%s'!", spec->filename);
  xyz_sprite_set_draggable(sprite, spec->draggable);
  xyz_sprite_set_methods(sprite, spec->methods);
  memcpy(sprite->subscribed_events, spec->events, XYZ_SPRITE_MAXEVENT);
  sprite->own_image = 1;
  sprite->user_info = spec->user_info;

  return sprite;
}

void xyz_free_sprite(xyz_sprite *sprite) {
  if(sprite->prev)
    sprite->prev->next = sprite->next;
  if(sprite->next)
    sprite->next->prev = sprite->prev;

  if(sprite_head == sprite)
    sprite_head = sprite->next;
  if(sprite_tail == sprite)
    sprite_tail = sprite->prev;

  free(sprite);
}

void xyz_free_all_sprites(void) {
  while(sprite_head) {
    xyz_free_sprite(sprite_head);
  }
}

void xyz_draw_sprite(xyz_sprite *sprite) {
  if(sprite->methods && sprite->methods->draw) {
    sprite->methods->draw(sprite);
  } else if(sprite->image) {
    xyz_draw_image(sprite->image, sprite->x, sprite->y);
  } else {
    xyz_fatal_error("There's no way to draw sprite %p!\n", sprite);
  }
}

int xyz_sprite_get_x(xyz_sprite *sprite) {
  return sprite->x;
}

int xyz_sprite_get_y(xyz_sprite *sprite) {
  return sprite->y;
}

int xyz_sprite_get_width(xyz_sprite *sprite) {
  return sprite->width;
}

int xyz_sprite_get_height(xyz_sprite *sprite) {
  return sprite->height;
}

xyz_image* xyz_sprite_get_image(xyz_sprite *sprite) {
  return sprite->image;
}

int xyz_sprite_get_draggable(xyz_sprite *sprite) {
  return sprite->draggable;
}

int xyz_sprite_subscribes_to(xyz_sprite *sprite, int event) {
  return sprite->subscribed_events[event];
}

void *xyz_sprite_get_user_info(xyz_sprite *sprite) {
  return sprite->user_info;
}

void xyz_sprite_set_methods(xyz_sprite *sprite, xyz_sprite_methods *methods) {
  sprite->methods = methods;
}

void xyz_sprite_set_x(xyz_sprite *sprite, int x) {
  sprite->x = x;
}

void xyz_sprite_set_y(xyz_sprite *sprite, int y) {
  sprite->y = y;
}

void xyz_sprite_set_width(xyz_sprite *sprite, int width) {
  sprite->width = width;
}

void xyz_sprite_set_height(xyz_sprite *sprite, int height) {
  sprite->height = height;
}

void xyz_sprite_set_image(xyz_sprite *sprite, xyz_image *image) {
  sprite->image = image;
}

void xyz_sprite_set_draggable(xyz_sprite *sprite, int draggable) {
  sprite->draggable = draggable;
}

void xyz_sprite_subscribe(xyz_sprite *sprite, int event, int subscription) {
  sprite->subscribed_events[event] = !!subscription;
}

void xyz_sprite_set_user_info(xyz_sprite *sprite, void *user_info) {
  sprite->user_info = user_info;
}

/***************** Sprite Events ***************************/

void xyz_sprite_handle_event(xyz_sprite* sprite, xyz_sprite_event* event) {
  if(sprite->methods && sprite->methods->handle_event) {
    sprite->methods->handle_event(sprite, event);
  }
}

static xyz_sprite_event* sprite_event_head = NULL;
static xyz_sprite_event* sprite_event_tail = NULL;

xyz_sprite_event *xyz_sprite_event_new(xyz_sprite *sprite) {
  xyz_sprite_event *event = calloc(sizeof(xyz_sprite), 1);

  event->sprite = sprite;
  event->sprite_x = sprite->x;
  event->sprite_y = sprite->y;

  /* Insert into overall events list */
  event->next_event = sprite_event_head;
  event->prev_event = NULL;
  sprite_event_head = event;
  if(!sprite_event_tail) sprite_event_tail = event;

  /* Insert into sprite list */
  event->next_sprite_event = sprite->events_head;
  event->prev_sprite_event = NULL;
  sprite->events_head = event;
  if(!sprite->events_tail) sprite->events_tail = event;

  return event;
}

void xyz_sprite_event_delete(xyz_sprite_event *event) {
  xyz_sprite *sprite = event->sprite;

  /* Free from overall events list */
  if(sprite_event_head == event) {
    sprite_event_head = event->next_event;
  }
  if(sprite_event_tail == event) {
    sprite_event_tail = event->prev_event;
  }
  if(event->prev_event) event->prev_event->next_event = event->next_event;
  if(event->next_event) event->next_event->prev_event = event->prev_event;

  /* Free from per-sprite events list */
  if(sprite->events_head == event) {
    sprite->events_head = sprite->events_head->next_sprite_event;
  }
  if(sprite->events_tail == event) {
    sprite->events_tail = sprite->events_tail->prev_sprite_event;
  }
  if(event->prev_sprite_event)
    event->prev_sprite_event->next_sprite_event = event->next_sprite_event;
  if(event->next_sprite_event)
    event->next_sprite_event->prev_sprite_event = event->prev_sprite_event;

  free(event);
}

/***************** Sprite Intersection ********************/

int xyz_sprite_intersect_point(xyz_sprite *sprite, int x, int y) {
  if(x < sprite->x || y < sprite->y) return 0;
  if(x > sprite->x + sprite->width || y > sprite->y + sprite->height)
    return 0;

  return 1;
}

int xyz_sprite_overlap(xyz_sprite *sprite1, xyz_sprite *sprite2) {
  int s1x1, s1x2, s1y1, s1y2;
  int s2x1, s2x2, s2y1, s2y2;

  s1x1 = sprite1->x; s1y1 = sprite1->y;
  s1x2 = s1x1 + sprite1->width; s1y2 = s1y1 + sprite1->height;
  s2x1 = sprite2->x; s2y1 = sprite2->y;
  s2x2 = s2x1 + sprite2->width; s2y2 = s2y1 + sprite2->height;

  if(s2x2 < s1x1) return 0;
  if(s2x1 > s1x2) return 0;
  if(s2y2 < s1y1) return 0;
  if(s2y1 > s1y2) return 0;

  return 1;
}

xyz_sprite *xyz_intersect_draggable_sprite(int x, int y) {
  xyz_sprite *index = sprite_head;
  while(index) {
    if(index->draggable) {
      if(xyz_sprite_intersect_point(index, x, y)) {
	return index;
      }
    }
    index = index->next;
  }
  return NULL;
}

void xyz_draw_sprites(void) {
  xyz_sprite *index = sprite_head;
  while(index) {
    xyz_draw_sprite(index);
    index = index->next;
  }
  index = index->next;
}

xyz_sprite *xyz_intersect_event_sprite(int x, int y, int event,
				       void (*handler)(xyz_sprite *sprite)) {
  xyz_sprite *index = sprite_head;
  while(index) {
    if(index->subscribed_events[event]) {
      if(xyz_sprite_intersect_point(index, x, y)) {
	handler(index);
      }
    }
    index = index->next;
  }
  return NULL;
}
