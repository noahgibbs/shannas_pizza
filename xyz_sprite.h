#include "xyz.h"

#ifndef __XYZ_SPRITE_H__
#define __XYZ_SPRITE_H__

#define XYZ_SPRITE_NONE         0
#define XYZ_SPRITE_CREATED      1
#define XYZ_SPRITE_DESTROYED    2
#define XYZ_SPRITE_BUTTONDOWN   3
#define XYZ_SPRITE_BUTTONUP     4
#define XYZ_SPRITE_MOVED        5
#define XYZ_SPRITE_OUTOFBOUNDS  6  /* Use?  Not impl */
#define XYZ_SPRITE_CLICKED      7  /* Not impl */
#define XYZ_SPRITE_ENTER        8  /* Not impl */
#define XYZ_SPRITE_EXIT         9  /* Not impl */
#define XYZ_SPRITE_MAXEVENT    10

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
  void (*draw)(xyz_sprite *sprite);
  void (*handle_event)(xyz_sprite *sprite, xyz_sprite_event *event);
} xyz_sprite_methods;

typedef struct {
  const char *filename;
  int x;
  int y;
  int width;
  int height;
  xyz_sprite_methods *methods;
  unsigned char events[XYZ_SPRITE_MAXEVENT];
  void *user_info;

  int private_data_size;  /* Size of private data struct */
  xyz_sprite *sprite;
  xyz_image *image;
} xyz_sprite_spec;

xyz_sprite* xyz_sprite_from_spec(xyz_sprite_spec *spec);

/* If num is -1, scan until hitting a NULL filename */
void xyz_sprites_from_specs(int num, xyz_sprite_spec *specs);

void xyz_free_sprite(xyz_sprite *sprite);
void xyz_free_all_sprites(void);
void xyz_draw_sprite(xyz_sprite *sprite);
void xyz_draw_sprites(void);  /* Draw all sprites */

void xyz_sprite_set_x(xyz_sprite *sprite, int x);
void xyz_sprite_set_y(xyz_sprite *sprite, int y);
void xyz_sprite_set_width(xyz_sprite *sprite, int width);
void xyz_sprite_set_height(xyz_sprite *sprite, int height);
void xyz_sprite_set_image(xyz_sprite *sprite, xyz_image *image);
void xyz_sprite_set_methods(xyz_sprite *sprite, xyz_sprite_methods *methods);
void xyz_sprite_subscribe(xyz_sprite *sprite, int event, int subscription);
void xyz_sprite_set_user_info(xyz_sprite *sprite, void *user_info);
void xyz_sprite_set_hidden(xyz_sprite *sprite, int hidden);

int xyz_sprite_get_x(xyz_sprite *sprite);
int xyz_sprite_get_y(xyz_sprite *sprite);
int xyz_sprite_get_width(xyz_sprite *sprite);
int xyz_sprite_get_height(xyz_sprite *sprite);
xyz_image* xyz_sprite_get_image(xyz_sprite *sprite);
int xyz_sprite_subscribes_to(xyz_sprite *sprite, int event);
void *xyz_sprite_get_user_info(xyz_sprite *sprite);
void *xyz_sprite_get_private_data(xyz_sprite *sprite);
xyz_sprite_methods *xyz_sprite_get_methods(xyz_sprite *sprite);
int xyz_sprite_get_hidden(xyz_sprite *sprite);

xyz_sprite* xyz_get_sprite_by_user_info(void *user_info);

int xyz_sprite_intersect_point(xyz_sprite *sprite, int x, int y);
int xyz_sprite_overlap(xyz_sprite *sprite1, xyz_sprite *sprite2);

xyz_sprite_event *xyz_sprite_event_new(xyz_sprite *sprite);
void xyz_sprite_event_delete(xyz_sprite_event *event);

/* Used internally by sprite library */
void xyz_sprite_handle_event(xyz_sprite* sprite, xyz_sprite_event* event);
xyz_sprite *xyz_intersect_event_sprite(int x, int y, int event,
				       void (*handler)(xyz_sprite *sprite));
xyz_sprite *xyz_intersect_filtered_sprite(int x, int y,
					  int (*filter)(xyz_sprite *sprite));

#endif /* __XYZ_SPRITE_H__ */
