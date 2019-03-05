#ifndef BTG_COLOR_H
#define BTG_COLOR_H

#include <stdio.h>

#include "struct.h"

int read_color (FILE *f, btg_base *base, unsigned int ver, btg_element *elem);
unsigned int count_color (btg_color *color);
int write_color (FILE *f, btg_color *color, unsigned int ver);
void free_color (btg_color *color);

void remove_unused_colors (btg_color *color);
void check_same_colors (btg_color *color);

#endif