#ifndef BTG_NORMAL_H
#define BTG_NORMAL_H

#include <stdio.h>

#include "struct.h"

int read_normal (FILE *f, btg_base *base, unsigned int ver, btg_element *elem);
unsigned int count_normal (btg_normal *normal);
int write_normal (FILE *f, btg_normal *normal, unsigned int ver);
void free_normal (btg_normal *normal);

void remove_unused_normals (btg_normal *normal);
void check_same_normals (btg_normal *normal);

#endif