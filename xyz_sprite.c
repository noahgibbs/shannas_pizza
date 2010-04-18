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
  int own_image;
  unsigned char subscribed_events[XYZ_SPRITE_MAXEVENT];

  xyz_sprite_methods *methods;
  void *user_info;
  void *private_data;

  int hidden;

  /* Auxiliary surfaces - rotated and whatnot */
  xyz_image *upside_down_image;

  struct _xyz_sprite_t *next;
  struct _xyz_sprite_t *prev;

  xyz_sprite_event *events_head;
  xyz_sprite_event *events_tail;
};

static xyz_sprite *sprite_head = NULL;
static xyz_sprite *sprite_tail = NULL;

static xyz_sprite *sprite_to_free = NULL;

static xyz_sprite *xyz_new_sprite(int x, int y, int width, int height,
			   xyz_image *image) {
  xyz_sprite *tmp = NULL;

  tmp = xyz_new(xyz_sprite);
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
  xyz_sprite_set_methods(sprite, spec->methods);
  memcpy(sprite->subscribed_events, spec->events, XYZ_SPRITE_MAXEVENT);
  sprite->own_image = 1;
  sprite->user_info = spec->user_info;
  if(spec->private_data_size > 0) {
    sprite->private_data = calloc(spec->private_data_size, 1);
  }

  if(sprite->subscribed_events[XYZ_SPRITE_CREATED]) {
    xyz_sprite_event *event = xyz_sprite_event_new(sprite);
    event->type = XYZ_SPRITE_CREATED;
    xyz_sprite_handle_event(sprite, event);
  }

  return sprite;
}

/* If num is -1, scan until hitting a NULL filename */
void xyz_sprites_from_specs(int num, xyz_sprite_spec *specs) {
  if(num >= 0) {
    int idx;

    for(idx = 0; idx < num; idx++) {
      if(!xyz_sprite_from_spec(specs + idx))
	xyz_fatal_error("Can't create sprite for '%s'!",
			(specs + idx)->filename);
    }
  } else {
    xyz_sprite_spec *index = specs;

    while(index->filename) {
      if(!xyz_sprite_from_spec(index))
	xyz_fatal_error("Can't create sprite for '%s'!", index->filename);

      index++;
    }
  }
}

void xyz_free_sprite(xyz_sprite *sprite) {
  if(sprite->subscribed_events[XYZ_SPRITE_DESTROYED]) {
    xyz_sprite_event *event = xyz_sprite_event_new(sprite);
    event->type = XYZ_SPRITE_DESTROYED;
    xyz_sprite_handle_event(sprite, event);
  }

  if(sprite->private_data)
    free(sprite->private_data);

  if(sprite->prev)
    sprite->prev->next = sprite->next;
  if(sprite->next)
    sprite->next->prev = sprite->prev;

  if(sprite_head == sprite)
    sprite_head = sprite->next;
  if(sprite_tail == sprite)
    sprite_tail = sprite->prev;

  /* Don't do "free(sprite);", put it on the "to-free" list so that
     this is safe to call in XYZ event handlers. */
  sprite->next = sprite_to_free;
  sprite_to_free = sprite->next;
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

void xyz_default_draw_sprite(xyz_sprite *sprite) {
  xyz_draw_image(sprite->image, sprite->x, sprite->y);
}

void xyz_draw_sprite_upside_down(xyz_sprite *sprite) {
  if(sprite->methods && sprite->methods->draw) {
    xyz_fatal_error("Haven't implemented graphics context yet!");
  } else if(sprite->image) {
    xyz_draw_image(sprite->upside_down_image, sprite->x, sprite->y);
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

int xyz_sprite_subscribes_to(xyz_sprite *sprite, int event) {
  return sprite->subscribed_events[event];
}

void *xyz_sprite_get_user_info(xyz_sprite *sprite) {
  return sprite->user_info;
}

void *xyz_sprite_get_private_data(xyz_sprite *sprite) {
  return sprite->private_data;
}

xyz_sprite_methods *xyz_sprite_get_methods(xyz_sprite *sprite) {
  return sprite->methods;
}

int xyz_sprite_get_hidden(xyz_sprite *sprite) {
  return sprite->hidden;
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

void xyz_sprite_subscribe(xyz_sprite *sprite, int event, int subscription) {
  sprite->subscribed_events[event] = !!subscription;
}

void xyz_sprite_set_methods(xyz_sprite *sprite, xyz_sprite_methods *methods) {
  sprite->methods = methods;
}

void xyz_sprite_set_user_info(xyz_sprite *sprite, void *user_info) {
  sprite->user_info = user_info;
}

void xyz_sprite_set_hidden(xyz_sprite *sprite, int hidden) {
  sprite->hidden = hidden;
}

/***************** Find Sprites &***************************/

xyz_sprite* xyz_get_sprite_by_user_info(void *user_info) {
  xyz_sprite* index = sprite_head;

  while(index) {
    if(index->user_info == user_info)
      return index;
    index = index->next;
  }
  return NULL;
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
  xyz_sprite_event *event = xyz_new(xyz_sprite_event);

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
  if(sprite->hidden) return 0;
  if(x < sprite->x || y < sprite->y) return 0;
  if(x > sprite->x + sprite->width || y > sprite->y + sprite->height)
    return 0;

  return 1;
}

int xyz_sprite_overlap(xyz_sprite *sprite1, xyz_sprite *sprite2) {
  int s1x1, s1x2, s1y1, s1y2;
  int s2x1, s2x2, s2y1, s2y2;

  if(sprite1->hidden || sprite2->hidden) return 0;

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

void xyz_sprite_each(int (*each_fn)(xyz_sprite *sprite)) {
  xyz_sprite *index = sprite_head;

  while(index) {
    if(each_fn(index)) break;
    index = index->next;
  }
}

xyz_sprite *xyz_intersect_filtered_sprite(int x, int y,
					  int (*filter)(xyz_sprite *sprite))
{
  xyz_sprite *index = sprite_head;
  while(index) {
    if(xyz_sprite_intersect_point(index, x, y)) {
      if(filter(index)) {
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
    if(!index->hidden) xyz_draw_sprite(index);
    index = index->next;
  }
}

void xyz_sprite_periodic(void) {
  while(sprite_to_free) {
    xyz_sprite *tmp = sprite_to_free->next;
    free(sprite_to_free);
    sprite_to_free = tmp;
  }
}

xyz_sprite *xyz_intersect_event_sprite(int x, int y, int event,
				       void (*handler)(xyz_sprite *sprite)) {
  xyz_sprite *index = sprite_head;
  while(index) {
    if(index->subscribed_events[event] && !index->hidden) {
      if(xyz_sprite_intersect_point(index, x, y)) {
	handler(index);
      }
    }
    index = index->next;
  }
  return NULL;
}
