#ifndef BTG_OBJECT_H
#define BTG_OBJECT_H

#include <stdio.h>

#include "struct.h"

btg_object *read_object (FILE *f, btg_base *base, unsigned int ver);
unsigned int count_object (btg_object *object);
int write_object (FILE *f, btg_object *obj, unsigned int ver);
void free_object (btg_object *obj);
btg_object *new_object (btg_object *object_all, unsigned char type, unsigned char mask, char *material);

#endif