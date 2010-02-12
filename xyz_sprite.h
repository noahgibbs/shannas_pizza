#include "xyz.h"

#ifndef __XYZ_SPRITE_H__
#define __XYZ_SPRITE_H__

#define XYZ_SPRITE_BUTTONDOWN  1
#define XYZ_SPRITE_BUTTONUP    2
#define XYZ_SPRITE_MOVED       3
#define XYZ_SPRITE_OUTOFBOUNDS 4
#define XYZ_SPRITE_CLICKED     5

typedef struct _sprite_event_t {
  int type;
  int mouse_x;
  int mouse_y;
  int button;
  int sprite_x;
  int sprite_y;
} sprite_event;

typedef struct _xyz_sprite_t xyz_sprite;

typedef struct {
  const char *filename;
  xyz_sprite *sprite;
  xyz_image *image;
  int x;
  int y;
  int width;
  int height;
  int draggable;
} sprite_spec;

xyz_sprite *xyz_new_sprite(unsigned int x, unsigned int y,
			   unsigned int width, unsigned int height,
			   xyz_image *image);
xyz_sprite* sprite_from_spec(sprite_spec *spec);
void xyz_free_sprite(xyz_sprite *sprite);
void xyz_draw_sprite(xyz_sprite *sprite);
void xyz_sprite_set_x(xyz_sprite *sprite, unsigned int x);
void xyz_sprite_set_y(xyz_sprite *sprite, unsigned int y);
void xyz_sprite_set_width(xyz_sprite *sprite, unsigned int width);
void xyz_sprite_set_height(xyz_sprite *sprite, unsigned int height);
void xyz_sprite_set_image(xyz_sprite *sprite, xyz_image *image);
void xyz_sprite_set_draggable(xyz_sprite *sprite, int draggable);
int xyz_sprite_intersect_point(xyz_sprite *sprite, unsigned int x, unsigned int y);
xyz_sprite *xyz_intersect_draggable_sprite(unsigned int x, unsigned int y);

#endif /* __XYZ_SPRITE_H__ */
