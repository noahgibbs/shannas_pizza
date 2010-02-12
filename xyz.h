/**
 * XYZ is a great name for an SDK because when I come up with a real
 * name for it, I can easily search and replace all instances with
 * whatever I *do* call it.
 */

#ifndef __XYZ_H__
#define __XYZ_H__

#include <stdio.h>
#include <stdlib.h>  /* for exit(), invoked from macros */

#define xyz_fatal_error(format, args...) \
  { \
    fprintf(stderr, format, ##args); \
    exit(-1); \
  }

typedef struct _xyz_image_t xyz_image;
typedef struct _xyz_sprite_t xyz_sprite;

void xyz_start(void);
void xyz_set_up_screen(int width, int height);
void xyz_end(void);

void xyz_color(int r, int g, int b);
void xyz_color_alpha(int r, int g, int b, int alpha);
void xyz_rectangle(int x, int y, int width, int height);
void xyz_fill();
void xyz_done_drawing();

xyz_image *xyz_load_image(const char *file);
void xyz_draw_image(xyz_image *image, int x, int y);
void xyz_free_image(xyz_image *image);

xyz_sprite *xyz_new_sprite(unsigned int x, unsigned int y,
			   unsigned int width, unsigned int height,
			   xyz_image *image);
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


#endif /* __XYZ_H__ */
