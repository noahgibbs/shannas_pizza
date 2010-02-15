#ifndef __PIZZA_H__
#define __PIZZA_H__

/* Screen layout */
#define TOTAL_HEIGHT           500
#define TOTAL_WIDTH            500
#define SYMBOLS_TOP_HEIGHT     400
#define CONVEYOR_BOTTOM_HEIGHT 100
#define TOOLBOX_TOP_HEIGHT     150
#define TOOLBOX_BOTTOM_HEIGHT  400
#define TOOLBOX_LEFT_WIDTH     400

#define SYMBOLS_WIDTH  32
#define SYMBOLS_HEIGHT 32

#define GATE_WIDTH 32
#define GATE_HEIGHT 64

#define MAX_NUM_GATES 100

void keyhandler(const char *keyname, int down);

/* From sprites.c */
void draw_sprites(void);
void load_sprites(void);
void free_sprites(void);

#endif /* __PIZZA_H__ */
