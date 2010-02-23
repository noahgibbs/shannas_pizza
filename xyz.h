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
    fprintf(stderr, format "\n", ##args); \
    exit(-1); \
  }

typedef struct _xyz_image_t xyz_image;

void xyz_start(void);
void xyz_set_up_screen_fullscreen();
void xyz_set_up_screen_window(int width, int height);
void xyz_end(void);

void xyz_custom_cursor_from_file(const char *filename, int x_off, int y_off);
void xyz_draw_cursor(void);

const char *xyz_last_error(void);

void xyz_mouse_position(int *x, int *y);
int xyz_mouse_is_button_down(int button);
void xyz_set_key_handler(void (*key_handler)(const char *key_name, int down));
void xyz_set_mouse_handlers(int (*mouse_move_handler)(int x, int y),
			    int (*mouse_button_handler)(int button,
							int is_down));
void xyz_process_events(void);

void xyz_color(int r, int g, int b);
void xyz_color_alpha(int r, int g, int b, int alpha);
void xyz_rectangle(int x, int y, int width, int height);
void xyz_fill();
void xyz_block_text(int x, int y, const char *text);
void xyz_done_drawing();

xyz_image *xyz_load_image(const char *file);
void xyz_draw_image(xyz_image *image, int x, int y);
void xyz_free_image(xyz_image *image);

/* Utils */

int xyz_point_distance(int x1, int y1, int x2, int y2);

#endif /* __XYZ_H__ */
