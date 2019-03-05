#ifndef BTG_SEAM_H
#define BTG_SEAM_H

#include <stdio.h>
#include "struct.h"

void check_seam (btg_header *tile, btg_fence *airport);
void check_seam_edges (btg_header *tile, btg_fence *airport);

#endif