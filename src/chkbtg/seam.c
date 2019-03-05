#include <math.h>
#include <string.h>

#include "seam.h"
#include "edge.h"
#include "triangle.h"

void check_seam (btg_header *tile, btg_fence *airport) {

	int found = 0;
	btg_fence *fence;
	btg_border *now = airport->border, *partner;
	btg_vertex *v_partner;

	while (now) {
		if (now->neighbour == NULL) {
			fence = tile->base.fence;
			while (fence) {
				if (
			    now->vertex->projection.x >= fence->min_x &&
			    now->vertex->projection.x <= fence->max_x &&
			    now->vertex->projection.y >= fence->min_y &&
			    now->vertex->projection.y <= fence->max_y
			    ) {
					partner = fence->border;
					while (partner) {
						if (
					    fabs(partner->vertex->absolute.x - now->vertex->absolute.x) < 0.01 &&
					    fabs(partner->vertex->absolute.y - now->vertex->absolute.y) < 0.01 &&
					    fabs(partner->vertex->absolute.z - now->vertex->absolute.z) < 0.01
					    ) {
							printf("partner found on fence!\n");
							found++;
							now->tile = tile;
							now->neighbour = partner->vertex;
						}
						partner = partner->next;
					}
				}
				fence = fence->next;
			}
			if (now->neighbour == NULL) {
				v_partner = tile->base.vertex;
				while (v_partner) {
					if (
				    fabs(v_partner->absolute.x - now->vertex->absolute.x) < 0.01 &&
				    fabs(v_partner->absolute.y - now->vertex->absolute.y) < 0.01 &&
				    fabs(v_partner->absolute.z - now->vertex->absolute.z) < 0.01
				    ) {
						printf("partner found in the wild!\n");
						found++;
						now->tile = tile;
						now->neighbour = v_partner;
					}
					v_partner = v_partner->next;
				}
			}
			if (now->neighbour == NULL) {
				printf("no partner found!\n");
			}
		}
		now = now->next;
	}
}



btg_edge *search_edge (btg_edge *all_edges, btg_vertex *v0, btg_vertex *v1) {

	btg_edge *found = NULL, *now;

	now = all_edges;
	while (now) {
		if ((now->vertex[0] == v0 && now->vertex[1] == v1) || (now->vertex[0] == v1 && now->vertex[1] == v0)) {
			if (found)
				fprintf(stderr, "found more than 1 edge!\n");
			else
				found = now;
		}
		now = now->next;
	}

	return found;
}



void check_seam_edges (btg_header *tile, btg_fence *airport) {

	int tria_cnt, edge_cnt;
	btg_border *now = NULL;
	btg_edge *found = NULL;
	btg_vertex *v0, *v1, *v2;
	btg_triangle *tria;
	btg_geometry *geo;

	now = airport->border;
	while (now) {
		v0 = now->neighbour;
		if (now->next)
			v1 = now->next->neighbour;
		else
			v1 = airport->border->neighbour;

		if (v0 && v1) {
			found = search_edge(tile->base.edge, v0, v1);
			if (found) {
				for (tria_cnt = 0 ; tria_cnt < 10 ; tria_cnt++) {
					tria = found->tria[tria_cnt];
					if (tria) {
						geo = tria->elem->element;
						v2 = NULL;
						while (geo) {
							if (geo->vertex != v0 && geo->vertex != v1) {
								if (v2) fprintf(stderr, "more than one v2!\n");
								else v2 = geo->vertex;
							}
							geo = geo->next;
						}
						if (v2 && get_side(v0, v1, v2) == airport->turn) {
							printf("tria found! delete\n");
							for (edge_cnt = 0 ; edge_cnt < 3 ; edge_cnt++) {
								if (tria->edge[edge_cnt]->vertex[0] == v2 || tria->edge[edge_cnt]->vertex[1] == v2)
									tria->edge[edge_cnt]->mark = 1;
							}
							unrec_triangle (&tile->base, tria);
						}
					}
				}
			}
		}
		now = now->next;
	}

	found = tile->base.edge;
	while (found) {
		if (found->mark) {
			for (tria_cnt = 0 ; tria_cnt < 10 ; tria_cnt++) {
				if (found->tria[tria_cnt]) {
					if (strcmp(found->tria[tria_cnt]->object->prop_material, "Airport")) {
						for (edge_cnt = 0 ; edge_cnt < 3 ; edge_cnt++) {
							found->tria[tria_cnt]->edge[edge_cnt]->mark = 1;
						}
						unrec_triangle (&tile->base, found->tria[tria_cnt]);
					}
					else {
						found->mark = 0;
					}
				}
			}
			
			found = tile->base.edge;
		}
		else found = found->next;
	}

}



