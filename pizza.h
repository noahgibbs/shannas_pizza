#ifndef __PIZZA_H__
#define __PIZZA_H__

#define TOTAL_HEIGHT 500
#define TOTAL_WIDTH 500
#define SYMBOLS_TOP_HEIGHT  400
#define TOOLBOX_LEFT_WIDTH  400

#define SYMBOLS_WIDTH 32
#define SYMBOLS_HEIGHT 32

/* From toppings.c */
void draw_toppings(void);
void load_toppings(void);
void free_toppings(void);

#endif /* __PIZZA_H__ */
