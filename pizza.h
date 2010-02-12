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

void keyhandler(const char *keyname, int down);

/* From toppings.c */
void draw_toppings(void);
void load_toppings(void);
void free_toppings(void);

/* From buttons.c */
void draw_buttons(void);
void load_buttons(void);
void free_buttons(void);

#endif /* __PIZZA_H__ */
