#ifndef BTG_TRIANGLE_H
#define BTG_TRIANGLE_H

#include <stdio.h>

#include "struct.h"

int add_triangle (btg_element *elem, btg_base *base, btg_object *object);
void free_triangle (btg_triangle *triangle);

int rec_triangle (btg_base *base, btg_triangle *triangle);
void unrec_triangle (btg_base *base, btg_triangle *triangle);
void check_triangle (btg_base *base, btg_triangle *triangle_start);

#endif


