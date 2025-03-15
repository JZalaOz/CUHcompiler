#ifndef COMMON_H
#define COMMON_H

#define ATTRIBUTE_RESET		0
#define ATTRIBUTE_BRIGHT 	1
#define ATTRIBUTE_DIM		2
#define ATTRIBUTE_UNDERLINE 3
#define ATTRIBUTE_BLINK		4   
#define ATTRIBUTE_REVERSE	7
#define ATTRIBUTE_HIDDEN	8

#define COLOR_BLACK   0
#define COLOR_RED	  1
#define COLOR_GREEN	  2
#define COLOR_YELLOW  3
#define COLOR_BLUE	  4
#define COLOR_MAGENTA 5
#define COLOR_CYAN	  6
#define	COLOR_WHITE	  7

void textColor(int attribute, int color);
void resetTextColor();

void textColorStdErr(int attribute, int color);
void resetTextColorStdErr();

int minInt(int a, int b);
int maxInt(int a, int b);

#endif