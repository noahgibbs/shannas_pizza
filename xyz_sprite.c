#include "xyz.h"
#include "xyz_sprite.h"

/*********** xyz_sprite ****************/

struct _xyz_sprite_t {
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  xyz_image *image;
  int draggable;
  int own_image;
  struct _xyz_sprite_t *next;
  struct _xyz_sprite_t *prev;
};

static xyz_sprite *sprite_head = NULL;

xyz_sprite *xyz_new_sprite(unsigned int x, unsigned int y,
			   unsigned int width, unsigned int height,
			   xyz_image *image) {
  xyz_sprite *tmp = NULL;

  tmp = calloc(sizeof(xyz_sprite), 1);
  if(!tmp) xyz_fatal_error("Could not allocate structure for sprite!");

  tmp->x = x; tmp->y = y;
  tmp->width = width; tmp->height = height;
  tmp->image = image;

  /* Put tmp at head of doubly-linked list */
  tmp->next = sprite_head;
  tmp->prev = NULL;
  if(sprite_head) sprite_head->prev = tmp;
  sprite_head = tmp;

  return tmp;
}

xyz_sprite* sprite_from_spec(sprite_spec *spec) {
  xyz_image *image;
  xyz_sprite *sprite;
  image = xyz_load_image(spec->filename);
  if(!image)
    xyz_fatal_error("Couldn't load image '%s'!", spec->filename);
  sprite = xyz_new_sprite(spec->x, spec->y, spec->width, spec->height, image);
  if(!sprite)
    xyz_fatal_error("Couldn't create sprite for image '%s'!", spec->filename);
  xyz_sprite_set_draggable(sprite, spec->draggable);
  sprite->own_image = 1;

  return sprite;
}

void xyz_free_sprite(xyz_sprite *sprite) {
  if(sprite->prev)
    sprite->prev->next = sprite->next;
  if(sprite->next)
    sprite->next->prev = sprite->prev;

  if(sprite_head == sprite) {
    sprite_head = sprite->next;
  }

  free(sprite);
}

void xyz_draw_sprite(xyz_sprite *sprite) {
  xyz_draw_image(sprite->image, sprite->x, sprite->y);
}

unsigned int xyz_sprite_get_x(xyz_sprite *sprite) {
  return sprite->x;
}

unsigned int xyz_sprite_get_y(xyz_sprite *sprite) {
  return sprite->y;
}

void xyz_sprite_set_x(xyz_sprite *sprite, unsigned int x) {
  sprite->x = x;
}

void xyz_sprite_set_y(xyz_sprite *sprite, unsigned int y) {
  sprite->y = y;
}

void xyz_sprite_set_width(xyz_sprite *sprite, unsigned int width) {
  sprite->width = width;
}

void xyz_sprite_set_height(xyz_sprite *sprite, unsigned int height) {
  sprite->height = height;
}

void xyz_sprite_set_image(xyz_sprite *sprite, xyz_image *image) {
  sprite->image = image;
}

void xyz_sprite_set_draggable(xyz_sprite *sprite, int draggable) {
  sprite->draggable = draggable;
}

int xyz_sprite_intersect_point(xyz_sprite *sprite, unsigned int x, unsigned int y) {
  if(x < sprite->x || y < sprite->y) return 0;
  if(x > sprite->x + sprite->width || y > sprite->y + sprite->height)
    return 0;

  return 1;
}

xyz_sprite *xyz_intersect_draggable_sprite(unsigned int x, unsigned y) {
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
