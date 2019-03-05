#ifndef BTGIO_H
#define BTGIO_H

#include <stdio.h>
#include <time.h>

#include "header.h"

int read_btg  (FILE *f, btg_header **header);
int write_btg (FILE *f, btg_header *header);

#endif
