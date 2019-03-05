#ifndef BTG_PROPERTY_H
#define BTG_PROPERTY_H

#include <stdio.h>

#include "struct.h"

int read_property (FILE *f, btg_object *obj, unsigned int ver);
int write_property(FILE *f, btg_object *obj, unsigned int ver);

#endif