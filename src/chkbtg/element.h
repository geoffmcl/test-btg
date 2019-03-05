#ifndef BTG_ELEMENT_H
#define BTG_ELEMENT_H

#include <stdio.h>

#include "struct.h"

btg_element *read_element (FILE *f, btg_base *base, unsigned int ver, unsigned char type, unsigned char mask, char *material);
unsigned int count_element (btg_element *elem, unsigned char type);
int write_element(FILE *f, btg_element *elem, unsigned int ver, unsigned char type, unsigned char mask);
void free_element(btg_element *elem, unsigned char type);
btg_element *new_element (void);
#endif