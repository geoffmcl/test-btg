#ifndef BTG_EDGE_H
#define BTG_EDGE_H

#include "coord.h"
#include "struct.h"

btg_edge *rec_edge (btg_edge **all, btg_edge **last, btg_vertex *v0, btg_vertex *v1, btg_triangle *triangle);
void unrec_edge (btg_edge *edge, btg_triangle *triangle);

void free_edges (btg_edge *edges);
void free_border (btg_border *border);

int check_edges (btg_base *base, btg_edge *edge);
btg_fence *collect_border (btg_edge **edge);
btg_fence *find_fence (btg_fence *fence);
btg_fence *examine_fence (btg_fence *fence, btg_base *base);

btg_vertex *collaps_vertices (btg_vertex *v0, btg_vertex *v1, btg_base *base);
btg_edge get_longest (btg_border *border);
btg_edge get_shortest (btg_border *border);
btg_normal *vector2normal (vector vec, btg_base *base);
void close_hole (btg_fence *fence, btg_base *base);


short get_side (btg_vertex *v0, btg_vertex *v1, btg_vertex *v2);
btg_triangle *build_triangle (btg_border *b0, btg_border *b1, btg_border *b2, btg_base *base, short cnt);

#endif