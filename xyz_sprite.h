#include "xyz.h"

#ifndef __XYZ_SPRITE_H__
#define __XYZ_SPRITE_H__

#define XYZ_SPRITE_NONE        0
#define XYZ_SPRITE_BUTTONDOWN  1
#define XYZ_SPRITE_BUTTONUP    2
#define XYZ_SPRITE_MOVED       3
#define XYZ_SPRITE_OUTOFBOUNDS 4  /* Use?  Not impl */
#define XYZ_SPRITE_CLICKED     5  /* Not impl */
#define XYZ_SPRITE_ENTER       6  /* Not impl */
#define XYZ_SPRITE_EXIT        7  /* Not impl */
#define XYZ_SPRITE_MAXEVENT    8

typedef struct _xyz_sprite_t xyz_sprite;

typedef struct _xyz_sprite_event_t {
  int type;
  int mouse_x;
  int mouse_y;
  int button;
  int sprite_x;
  int sprite_y;
  xyz_sprite *sprite;
  struct _xyz_sprite_event_t *next_event;
  struct _xyz_sprite_event_t *prev_event;
  struct _xyz_sprite_event_t *next_sprite_event;
  struct _xyz_sprite_event_t *prev_sprite_event;
} xyz_sprite_event;

typedef struct {
  int (*draw)(xyz_sprite *sprite);
  void (*handle_event)(xyz_sprite *sprite, xyz_sprite_event *event);
} xyz_sprite_methods;

typedef struct {
  const char *filename;
  xyz_sprite *sprite;
  xyz_image *image;
  int x;
  int y;
  int width;
  int height;
  int draggable;
  xyz_sprite_methods *methods;
  char events[XYZ_SPRITE_MAXEVENT];
} xyz_sprite_spec;

xyz_sprite *xyz_new_sprite(unsigned int x, unsigned int y,
			   unsigned int width, unsigned int height,
			   xyz_image *image);
xyz_sprite* sprite_from_spec(xyz_sprite_spec *spec);

void xyz_free_sprite(xyz_sprite *sprite);
void xyz_free_all_sprites(void);
void xyz_draw_sprite(xyz_sprite *sprite);

void xyz_sprite_set_x(xyz_sprite *sprite, unsigned int x);
void xyz_sprite_set_y(xyz_sprite *sprite, unsigned int y);
void xyz_sprite_set_width(xyz_sprite *sprite, unsigned int width);
void xyz_sprite_set_height(xyz_sprite *sprite, unsigned int height);
void xyz_sprite_set_image(xyz_sprite *sprite, xyz_image *image);
void xyz_sprite_set_draggable(xyz_sprite *sprite, int draggable);
void xyz_sprite_set_methods(xyz_sprite *sprite, xyz_sprite_methods *methods);
void xyz_sprite_subscribe(xyz_sprite *sprite, int event, int subscription);

unsigned int xyz_sprite_get_x(xyz_sprite *sprite);
unsigned int xyz_sprite_get_y(xyz_sprite *sprite);
unsigned int xyz_sprite_get_width(xyz_sprite *sprite);
unsigned int xyz_sprite_get_height(xyz_sprite *sprite);
xyz_image* xyz_sprite_get_image(xyz_sprite *sprite);
int xyz_sprite_get_draggable(xyz_sprite *sprite);
int xyz_sprite_subscribes_to(xyz_sprite *sprite, int event);

int xyz_sprite_intersect_point(xyz_sprite *sprite, unsigned int x, unsigned int y);

xyz_sprite_event *xyz_sprite_event_new(xyz_sprite *sprite);
void xyz_sprite_event_delete(xyz_sprite_event *event);

/* Used internally by sprite library */
void xyz_sprite_handle_event(xyz_sprite* sprite, xyz_sprite_event* event);
xyz_sprite *xyz_intersect_draggable_sprite(unsigned int x, unsigned int y);
xyz_sprite *xyz_intersect_event_sprite(unsigned int x, unsigned y,
				       int event,
				       void (*handler)(xyz_sprite *sprite));

#endif /* __XYZ_SPRITE_H__ */
