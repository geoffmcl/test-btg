#ifndef BTG_POINT_H
#define BTG_POINT_H

#include <stdio.h>

#include "struct.h"

int add_point (btg_element *elem, btg_base *base, btg_object *object);
//btg_geometry *read_point (FILE *f, btg_base *base, unsigned int ver, unsigned int cnt, unsigned char mask, char *material);
void free_point (btg_point *point);

void rec_point (btg_point *point);
void unrec_point (btg_point *point);
void check_points (btg_point *point_start);

#endif


