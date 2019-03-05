#ifndef BTG_GEOMETRY_H
#define BTG_GEOMETRY_H

#include <stdio.h>

#include "header.h"
#include "struct.h"

btg_geometry *read_geometry (FILE *f, btg_base *base, unsigned int ver, unsigned char mask);
unsigned int count_geometry (btg_geometry *geometry);
int write_geometry (FILE *f, btg_geometry *geo, unsigned int ver, unsigned char mask);
void free_geometry (btg_geometry *geo);

void rec_geometry (btg_geometry *geo);
void unrec_geometry (btg_geometry *geo);
void unalias_geometry (btg_geometry *geo);

btg_geometry *new_geometry (btg_vertex *vertex, btg_normal *normal, btg_texcoo *texcoo, btg_color *color);
#endif