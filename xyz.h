/**
 * XYZ is a great name for an SDK because when I come up with a real
 * name for it, I can easily search and replace all instances with
 * whatever I *do* call it.
 */

#ifndef __XYZ_H__
#define __XYZ_H__

#define xyz_fatal_error(format, args...) \
  { \
    fprintf(stderr, format, ##args); \
    exit(-1); \
  }

typedef struct _xyz_image_t xyz_image;

void xyz_start(void);
void xyz_set_up_screen(void);
void xyz_end(void);

xyz_image *xyz_load_image(const char *file);
void xyz_draw_image(xyz_image *image, int x, int y);

#endif /* __XYZ_H__ */
