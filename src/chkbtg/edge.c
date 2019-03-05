#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "edge.h"
#include "vertex.h"
#include "triangle.h"

btg_edge *rec_edge (btg_edge **all, btg_edge **last, btg_vertex *v0, btg_vertex *v1, btg_triangle *triangle) {

	int i;
	btg_vertex *temp;
	btg_edge *edge = NULL;

	if (all == NULL) {
		fprintf(stderr, "rec_edge: pointer to all is NULL! break.\n");
		return NULL;
	}

	if (last == NULL) {
		fprintf(stderr, "rec_edge: pointer to last is NULL! break.\n");
		return NULL;
	}

	if (v0 == NULL) {
		fprintf(stderr, "rec_edge: pointer to v0 is NULL! break.\n");
		return NULL;
	}

	if (v1 == NULL) {
		fprintf(stderr, "rec_edge: pointer to v1 is NULL! break.\n");
		return NULL;
	}

	if (triangle == NULL) {
		fprintf(stderr, "rec_edge: pointer to triangle is NULL! break.\n");
		return NULL;
	}

	if (v0 > v1) {
		temp = v0;
		v0 = v1;
		v1 = temp;
	}

	edge = *all;
	if (edge) {
		while (edge) {
			if (edge->vertex[0] == v0 && edge->vertex[1] == v1) {
				edge->tria[edge->count++] = triangle;
				if (edge->count > 2) {
					printf("rec_edge: Edge usage: %d\n%f,%f,%f\n%f,%f,%f\n", edge->count,
				    edge->vertex[0]->relative.x, edge->vertex[0]->relative.y, edge->vertex[0]->relative.z,
				    edge->vertex[1]->relative.x, edge->vertex[1]->relative.y, edge->vertex[1]->relative.z
				    );
				}
				return edge;
			}
			edge = edge->next;
		}
	}

	if ((edge = malloc(sizeof (*edge))) == NULL) {
		fprintf(stderr, "rec_edge: no memory left for edges! brake.\n");
		return NULL;
	}

	edge->vertex[0] = v0;
	edge->vertex[1] = v1;
	edge->tria[0] = triangle;
	for (i = 1 ; i < 10 ; i++) edge->tria[i] = NULL;
	edge->count = 1;
	edge->mark = 0;
	edge->next = NULL;
	if (*all) (*last)->next = edge;
	else *all = edge;
	*last = edge;

	return edge;
}

void unrec_edge (btg_edge *edge, btg_triangle *triangle) {

	int cnt, flag = 1;

	if (edge == NULL) {
		fprintf(stderr, "unrec_edge: pointer to edge is NULL!\n");
		return;
	}

	for (cnt = 0 ; cnt < edge->count ; cnt++) {
		if (edge->tria[cnt] == triangle) {
			edge->tria[cnt] = NULL;
			flag = 0;
		}
	}

	if (flag) {
		fprintf(stderr, "unrec_edge: triangle inside edge not found!\n");
		return;
	}
	else {
		edge->count--;
		for (cnt = 0 ; cnt < edge->count ; cnt++) {
			if (edge->tria[cnt] == NULL) {
				edge->tria[cnt] = edge->tria[cnt + 1];
				edge->tria[cnt + 1] = NULL;
			}
		}
		if (edge->count  == 0) edge->mark = 0;
	}

}



void free_edges (btg_edge *edges) {
	btg_edge *tmp;
	while (edges) {
		tmp = edges->next;
		free(edges);
		edges = tmp;
	}
}

void free_border (btg_border *border) {
	btg_border *tmp;
	while (border) {
		tmp = border->next;
		free(border);
		border = tmp;
	}
}

int check_edges (btg_base *base, btg_edge *edge) {

	int tria_cnt, vertex_cnt, error = 0, usage[3], point[10], chosen; //, before, ap, nonap;

	while (edge) {
//		before = edge->count;
		if (edge->count > 2) {
			printf("check_edges: edge usage is %d ...\n", edge->count);
/*
			ap = 0;
			nonap = -1;
			for (tria_cnt = 0 ; tria_cnt < 10 ; tria_cnt++) {
				if (edge->tria[tria_cnt] && edge->tria[tria_cnt]->elem->valid) {
					if (strcmp(edge->tria[tria_cnt]->object->prop_material, "Airport")) nonap = tria_cnt;
					else ap++;
				}
			}
			if (ap == 2 && nonap != -1) {
				unrec_triangle (base, edge->tria[nonap]);
			}
*/
			for (tria_cnt = 0 ; tria_cnt < 10 ; tria_cnt++) {
				point[tria_cnt] = 0;
				usage[0] = usage[1] = usage[2] = 0;
				if (edge->tria[tria_cnt] && edge->tria[tria_cnt]->elem->valid) {
					for (vertex_cnt = 0 ; vertex_cnt < 3 ; vertex_cnt++) {
						if      (edge->tria[tria_cnt]->edge[vertex_cnt]->count == 1) usage[0]++;
						else if (edge->tria[tria_cnt]->edge[vertex_cnt]->count == 2) usage[1]++;
						else if (edge->tria[tria_cnt]->edge[vertex_cnt]->count >  2) usage[2]++;
					}
					if      (usage[2] == 3 && usage[1] == 0 && usage[0] == 0) point[tria_cnt] = 6;
					else if (usage[2] == 2 && usage[1] == 0 && usage[0] == 1) point[tria_cnt] = 5;
					else if (usage[2] == 1 && usage[1] == 1 && usage[0] == 1) point[tria_cnt] = 4;
					else if (usage[2] == 1 && usage[1] == 0 && usage[0] == 2) point[tria_cnt] = 3;
					else if (usage[2] == 2 && usage[1] == 1 && usage[0] == 0) point[tria_cnt] = 2;
					else if (usage[2] == 1 && usage[1] == 2 && usage[0] == 0) point[tria_cnt] = 1;
				}
			}
			while (edge->count > 2) {
				chosen = 0;
				for (tria_cnt = 0 ; tria_cnt < 10 ; tria_cnt++) {
					if (point[tria_cnt] > point[chosen]) chosen = tria_cnt;
				}
				unrec_triangle (base, edge->tria[chosen]);
				point[chosen] = 0;
				error++;
			}
		}
		edge = edge->next;
	}

	printf("check_edges: %d obscure triangles deleted.\n", error);
	return error;
}

btg_fence *collect_border (btg_edge **edge) {

	int e1 = 0, e2 = 0, er = 0, found;
	btg_edge *now = NULL, *prev = NULL;
	btg_border *new = NULL, *last = NULL;
	btg_fence *fence = NULL;

	now = *edge;

	if ((fence = malloc(sizeof (*fence))) == NULL) {
		fprintf(stderr, "no memory left for fence!\n");
		return fence;
	}
	fence->min_x = fence->max_x = fence->min_y = fence->max_y = 0.0;
	fence->turn = 1;
	fence->border = NULL;
	fence->next = NULL;

	while (now) {
		found = 0;
		if (now->count < 0) {
			fprintf(stderr, "edge usage below 0 !!!\n");
		}
		else if (now->count == 0) {
			fprintf(stderr, "unused edge !!!\n");
		}
		else if (now->count == 1) {
			if ((new = malloc(sizeof (*new))) == NULL) {
				fprintf(stderr, "no memory left for border!\n");
				return fence;
			}
			new->edge = now;
			new->side = 0;
			new->vertex = NULL;
			new->neighbour = NULL;
			new->tile = NULL;
			new->next = NULL;
			if (last) last->next = new;
			else fence->border = new;
			last = new;
			found++;

			if (fence->turn) {
				fence->min_x = now->vertex[0]->projection.x;
				fence->max_x = now->vertex[0]->projection.x;
				fence->min_y = now->vertex[0]->projection.y;
				fence->max_y = now->vertex[0]->projection.y;
				fence->turn = 0;
			}
			if (now->vertex[0]->projection.x < fence->min_x) fence->min_x = now->vertex[0]->projection.x;
			if (now->vertex[0]->projection.x > fence->max_x) fence->max_x = now->vertex[0]->projection.x;
			if (now->vertex[0]->projection.y < fence->min_y) fence->min_y = now->vertex[0]->projection.y;
			if (now->vertex[0]->projection.y < fence->max_y) fence->max_y = now->vertex[0]->projection.y;
			if (now->vertex[1]->projection.x < fence->min_x) fence->min_x = now->vertex[1]->projection.x;
			if (now->vertex[1]->projection.x > fence->max_x) fence->max_x = now->vertex[1]->projection.x;
			if (now->vertex[1]->projection.y < fence->min_y) fence->min_y = now->vertex[1]->projection.y;
			if (now->vertex[1]->projection.y < fence->max_y) fence->max_y = now->vertex[1]->projection.y;
			e1++;
		}
		else if (now->count == 2) e2++;
		else if (now->count > 2) {
			er++;
			fprintf(stderr, "edge usage is %d!!!\n", now->count);
			fprintf(stderr, "%f,%f,%f\n", now->vertex[0]->absolute.x, now->vertex[0]->absolute.y, now->vertex[0]->absolute.z);
			fprintf(stderr, "%f,%f,%f\n", now->vertex[1]->absolute.x, now->vertex[1]->absolute.y, now->vertex[1]->absolute.z);
		}
		if (found) {
			if (prev) prev->next = now->next;
			else *edge = now->next;
		}
		else prev = now;
		now = now->next;
	}

	if (fence->border == NULL) {
		free (fence);
		fence = NULL;
	}
	else {
		printf("Edges 1: %d / 2: %d / error: %d\n", e1, e2, er);
	}

	return fence;
}

double get_angle (btg_vertex *v0, btg_vertex *v1) {

	double angle, diff_x, diff_y, hyp;

	diff_x = v1->projection.x - v0->projection.x;
	diff_y = v1->projection.y - v0->projection.y;
	hyp = sqrt((diff_x * diff_x) + (diff_y * diff_y));

	angle = acos(diff_x / hyp);
	if (diff_y < 0.0) angle = (2 * M_PI) - angle;

	return angle;
}


short get_side (btg_vertex *v0, btg_vertex *v1, btg_vertex *v2) {

	short side = 0;
	double angle[2], ang_diff;

	if (v0 == NULL) {
		fprintf(stderr, "vertex 0 is NULL! break.\n");
		return side;
	}

	if (v1 == NULL) {
		fprintf(stderr, "vertex 1 is NULL! break.\n");
		return side;
	}

	if (v2 == NULL) {
		fprintf(stderr, "vertex 2 is NULL! break.\n");
		return side;
	}

	angle[0] = get_angle(v0, v1);
	angle[1] = get_angle(v0, v2);
	ang_diff = angle[0] - angle[1];

	if (ang_diff < -M_PI) ang_diff += 2 * M_PI;
	if (ang_diff >  M_PI) ang_diff -= 2 * M_PI;

	if (ang_diff < 0.0)
		side = -1;
	else
		side = 1;

	return side;
}

short get_tria_side (btg_vertex *v0, btg_vertex *v1, btg_triangle *tria) {

	short side = 0;
	btg_geometry *geo = NULL;
	btg_vertex *v2 = NULL;

	if (v0 == NULL) {
		fprintf(stderr, "vertex 0 is NULL! break.\n");
		return side;
	}

	if (v1 == NULL) {
		fprintf(stderr, "vertex 1 is NULL! break.\n");
		return side;
	}

	if (tria == NULL) {
		fprintf(stderr, "triangle is NULL! break.\n");
		return side;
	}

	geo = tria->elem->element;
	while (geo && (geo->vertex == v0 || geo->vertex == v1)) {
		geo = geo->next;
	}
	v2 = geo->vertex;

	side = get_side (v0, v1, v2);
	return side;
}


btg_border *get_next_segment (btg_fence *all, btg_vertex *search, short direction) {

	int found = 0;
	btg_border *now = NULL, *prev = NULL;
	now = all->border;

	while (!found && now) {
		if (search == now->edge->vertex[0]) {
			if (direction) {
				now->vertex = now->edge->vertex[0];
				now->side = get_tria_side (now->edge->vertex[0], now->edge->vertex[1], now->edge->tria[0]);
			}
			else {
				now->vertex = now->edge->vertex[1];
				now->side = get_tria_side (now->edge->vertex[1], now->edge->vertex[0], now->edge->tria[0]);
			}
			found = 1;
		}
		else if (search == now->edge->vertex[1]) {
			if (direction) {
				now->vertex = now->edge->vertex[1];
				now->side = get_tria_side (now->edge->vertex[1], now->edge->vertex[0], now->edge->tria[0]);
			}
			else {
				now->vertex = now->edge->vertex[0];
				now->side = get_tria_side (now->edge->vertex[0], now->edge->vertex[1], now->edge->tria[0]);
			}
			found = 1;
		}
		else {
			prev = now;
			now = now->next;
		}
	}

	if (!found) {
		now = NULL;
	}
	else {
		if (prev)
			prev->next = now->next;
		else
			all->border = now->next;
		now->next = NULL;
	}

	return now;
}


btg_fence *find_fence (btg_fence *border) {

	int count = 0, search = 1;
	btg_fence *fence = NULL;
	btg_border *last = NULL, *now = NULL;

	printf("********** new fence **********\n");

	if (border == NULL) {
		fprintf(stderr, "pointer to all borders is NULL! exit.\n");
		return NULL;
	}

	last = now = border->border;
	if (now == NULL) {
		fprintf(stderr, "pointer to first border is NULL! exit.\n");
		return NULL;
	}

	if ((fence = malloc(sizeof (*fence))) == NULL) {
		fprintf(stderr, "no memory left for fence!\n");
		return fence;
	}
	fence->turn = 0;
	fence->border = now;
	fence->next = NULL;

	border->border = border->border->next;
	now->next = NULL;

	fence->border->vertex = now->edge->vertex[0];
	fence->min_x = now->edge->vertex[0]->projection.x;
	fence->max_x = now->edge->vertex[0]->projection.x;
	fence->min_y = now->edge->vertex[0]->projection.y;
	fence->max_y = now->edge->vertex[0]->projection.y;
	if (now->edge->vertex[1]->projection.x < fence->min_x) fence->min_x = now->edge->vertex[1]->projection.x;
	if (now->edge->vertex[1]->projection.x > fence->max_x) fence->max_x = now->edge->vertex[1]->projection.x;
	if (now->edge->vertex[1]->projection.y < fence->min_y) fence->min_y = now->edge->vertex[1]->projection.y;
	if (now->edge->vertex[1]->projection.y < fence->max_y) fence->max_y = now->edge->vertex[1]->projection.y;
	now->side = get_tria_side (now->edge->vertex[0], now->edge->vertex[1], now->edge->tria[0]);
	count = 1;

	printf("search forward ...\n");
	now = (void *) 1;
	while (now && last->edge->vertex[search] != fence->border->vertex) {
		if ((now = get_next_segment (border, last->edge->vertex[search], 1))) {
			if      (last->edge->vertex[search] == now->edge->vertex[0]) search = 1;
			else if (last->edge->vertex[search] == now->edge->vertex[1]) search = 0;
			if (now->edge->vertex[search]->projection.x < fence->min_x) fence->min_x = now->edge->vertex[search]->projection.x;
			if (now->edge->vertex[search]->projection.x > fence->max_x) fence->max_x = now->edge->vertex[search]->projection.x;
			if (now->edge->vertex[search]->projection.y < fence->min_y) fence->min_y = now->edge->vertex[search]->projection.y;
			if (now->edge->vertex[search]->projection.y > fence->max_y) fence->max_y = now->edge->vertex[search]->projection.y;
			last->next = now;
			last = last->next;
			count++;
		}
	}

// if fence isn't closed, search on the other side
	if (last->edge->vertex[search] != fence->border->vertex) {
		printf("search backward ...\n");
		now = (void *) 1;
		while (now && last->edge->vertex[search] != fence->border->vertex) {
			if ((now = get_next_segment (border, fence->border->vertex, 0))) {
				if (now->vertex->projection.x < fence->min_x) fence->min_x = now->vertex->projection.x;
				if (now->vertex->projection.x > fence->max_x) fence->max_x = now->vertex->projection.x;
				if (now->vertex->projection.y < fence->min_y) fence->min_y = now->vertex->projection.y;
				if (now->vertex->projection.y > fence->max_y) fence->max_y = now->vertex->projection.y;
				now->next = fence->border;
				fence->border = now;
				count++;
			}
		}
	}

	printf("found fence with %d segments\n", count);

	return fence;
}



int check_vectorline (btg_fence *fence, short aspect) {

	double rate_x, rate_y, rate_z;
	btg_edge edge_res;
	vector vec_init, vec_now;
	btg_border *init, *now;

// get two vertices with the longest distance
	edge_res = get_longest (fence->border);
	switch (aspect) {
		case USE_ABSOLUTE:
			vec_init.x = edge_res.vertex[0]->absolute.x - edge_res.vertex[1]->absolute.x;
			vec_init.y = edge_res.vertex[0]->absolute.y - edge_res.vertex[1]->absolute.y;
			vec_init.z = edge_res.vertex[0]->absolute.z - edge_res.vertex[1]->absolute.z;
			break;
		case USE_RELATIVE:
			vec_init.x = edge_res.vertex[0]->relative.x - edge_res.vertex[1]->relative.x;
			vec_init.y = edge_res.vertex[0]->relative.y - edge_res.vertex[1]->relative.y;
			vec_init.z = edge_res.vertex[0]->relative.z - edge_res.vertex[1]->relative.z;
			break;
		case USE_PROJECTION:
			vec_init.x = edge_res.vertex[0]->projection.x - edge_res.vertex[1]->projection.x;
			vec_init.y = edge_res.vertex[0]->projection.y - edge_res.vertex[1]->projection.y;
			vec_init.z = 0.0;
			break;
	}
	init = fence->border;
	while (init->vertex != edge_res.vertex[0]) init = init->next;

// check if all vertices placed on the same line
	now = fence->border;
	while (init && now) {
		if (now != init) {
			switch (aspect) {
				case USE_ABSOLUTE:
					vec_now.x = init->vertex->absolute.x - now->vertex->absolute.x;
					vec_now.y = init->vertex->absolute.y - now->vertex->absolute.y;
					vec_now.z = init->vertex->absolute.z - now->vertex->absolute.z;
					break;
				case USE_RELATIVE:
					vec_now.x = init->vertex->relative.x - now->vertex->relative.x;
					vec_now.y = init->vertex->relative.y - now->vertex->relative.y;
					vec_now.z = init->vertex->relative.z - now->vertex->relative.z;
					break;
				case USE_PROJECTION:
					vec_now.x = init->vertex->projection.x - now->vertex->projection.x;
					vec_now.y = init->vertex->projection.y - now->vertex->projection.y;
					vec_now.z = 0.0;
					break;
			}
			rate_x = vec_now.x / vec_init.x;
			rate_y = vec_now.y / vec_init.y;
			rate_z = vec_now.z / vec_init.z;
			if (fabs(rate_x - rate_y) > 0.001 || fabs(rate_x - rate_z) > 0.001 || fabs(rate_y - rate_z) > 0.001) {
				init = NULL;
			}
		}
		now = now->next;
	}

	if (init) return 1;

	return 0;
}

btg_fence *examine_fence (btg_fence *fence, btg_base *basex) {

	int left_side = 0, right_side = 0;
	btg_border *last = NULL, *now = NULL;
	double turn, ang_diff, angle[2];
	btg_edge edge_res;
	double len;

	if (basex == NULL) {
		fprintf(stderr, "examine_fence: pointer to base is NULL! exit.\n");
		return NULL;
	}

	if (fence == NULL) {
		fprintf(stderr, "examine_fence: pointer to fence is NULL! exit.\n");
		return NULL;
	}

	last = now = fence->border;
	if (now == NULL) {
		fprintf(stderr, "examine_fence: pointer to first segment is NULL! exit.\n");
		return NULL;
	}

	while (last->next) last = last->next;

// if fence is closed
	if (last->edge->vertex[0] == fence->border->vertex || last->edge->vertex[1] == fence->border->vertex) {

// if all vertices on the same vector, free it and return nothing ...
		if (check_vectorline(fence, USE_RELATIVE)) {
			printf("examine_fence: fence has all vertices on same vector!\n");
			free_border (fence->border);
			free (fence);
			return NULL;
		}

// ... otherwise its a hole or island
// calculate turn
		turn = 0.0;
		angle[0] = get_angle(last->vertex, fence->border->vertex);
		now = fence->border;
// sum up differences
		while (now->next) {
			angle[1] = get_angle(now->vertex, now->next->vertex);
			ang_diff = angle[0] - angle[1];
			if (ang_diff < -M_PI) ang_diff += 2 * M_PI;
			if (ang_diff >  M_PI) ang_diff -= 2 * M_PI;
			turn += ang_diff;
			angle[0] = angle[1];
			if (now->side == 1) right_side++;
			else if (now->side == -1) left_side++;
			else printf("examine_fence: border without side!\n");
			now = now->next;
		}
// sum up last segment
		angle[1] = get_angle(now->vertex, fence->border->vertex);
		ang_diff = angle[0] - angle[1];
		if (ang_diff < -M_PI) ang_diff += 2 * M_PI;
		if (ang_diff >  M_PI) ang_diff -= 2 * M_PI;
		turn += ang_diff;
		if (now->side == 1) right_side++;
		else if (now->side == -1) left_side++;
		else printf("examine_fence: border without side!\n");

// check turn against triangle side
		printf("examine_fence: to left: %d / to right: %d\n", left_side, right_side);
		if (turn < 0.0) {
			if (fabs(turn + (2 * M_PI)) < 0.0001) {
				printf("examine_fence: turn left! %.9f\n", fabs(turn + (2 * M_PI)));
				fence->turn = -1;
				if (left_side > 0 && right_side == 0) {
					printf("examine_fence: fence is a island!\n");
				}
				else if (right_side > 0 && left_side == 0) {
					printf("examine_fence: fence is a hole!\n");
					if ((fence->max_x - fence->min_x) < basex->holesize && (fence->max_y - fence->min_y) < basex->holesize) {
						printf("examine_fence: close it!\n");
						close_hole (fence, basex);
						return NULL;
					}
				}
				else {
					if (check_vectorline(fence, USE_PROJECTION)) {
						close_hole (fence, basex);
						return NULL;
					}
				}
			}
			else {
				printf("examine_fence: turn should be left! %.9f\n", fabs(turn + (2 * M_PI)));
				edge_res = get_shortest (fence->border);
				len = pydacoras(edge_res.vertex[0], edge_res.vertex[1], USE_RELATIVE);
				if (len < 0.10) {
					printf("examine_fence: collaps vertex %f\n", len);
					collaps_vertices (edge_res.vertex[0], edge_res.vertex[1], basex);
					free_border (fence->border);
					free (fence);
					return NULL;
				}
				else {
					printf("examine_fence: let it untouched %f\n", len);
				}
			}
		}
		else {
			if (fabs(turn - (2 * M_PI)) < 0.0001) {
				printf("examine_fence: turn right! %.9f\n", fabs(turn - (2 * M_PI)));
				fence->turn = 1;
				if (right_side > 0 && left_side == 0) {
					printf("examine_fence: fence is a island!\n");
				}
				else if (left_side > 0 && right_side == 0) {
					printf("examine_fence: fence is a hole!\n");
					if ((fence->max_x - fence->min_x) < basex->holesize && (fence->max_y - fence->min_y) < basex->holesize) {
						printf("examine_fence: close it!\n");
						close_hole (fence, basex);
						return NULL;
					}
				}
				else {
					if (check_vectorline(fence, USE_PROJECTION)) {
						close_hole (fence, basex);
						return NULL;
					}
				}
			}
			else {
				printf("examine_fence: turn should be right! %.9f\n", fabs(turn - (2 * M_PI)));
				edge_res = get_shortest (fence->border);
				len = pydacoras(edge_res.vertex[0], edge_res.vertex[1], USE_RELATIVE);
				if (len < 0.10) {
					printf("examine_fence: collaps vertex %f\n", len);
					collaps_vertices (edge_res.vertex[0], edge_res.vertex[1], basex);
					free_border (fence->border);
					free (fence);
					return NULL;
				}
				else {
					printf("examine_fence: let it untouched %f\n", len);
				}
			}
		}
	}

	else {
		printf("examine_fence: fence isn't closed! examine vertices ...\n");
// search two vertices with the smallest distance
		edge_res = get_shortest (fence->border);
		len = pydacoras(edge_res.vertex[0], edge_res.vertex[1], USE_RELATIVE);
		if (len < 0.25) {
			printf("examine_fence: collaps vertex %f\n", len);
			collaps_vertices (edge_res.vertex[0], edge_res.vertex[1], basex);
/*
			free_border (fence->border);
			free (fence);
			return NULL;
*/
		}
		else {
			printf("examine_fence: let it untouched %f\n", len);
		}
	}

	return fence;
}



btg_trialist *get_trias_on_vertex (btg_vertex *v, btg_triangle *tria_all) {

	btg_trialist *tria_list = NULL, *new = NULL, *last = NULL;
	btg_triangle *tria = NULL;
	btg_geometry *geo;

	tria = tria_all;
	while (tria) {
		if (tria->elem->valid) {
			geo = tria->elem->element;
			while (geo) {
				if (geo->vertex == v) {
					if ((new = malloc(sizeof(*new))) == NULL) {
						fprintf(stderr, "get_trias_on_vertex: no memory left for triangle-node! break.\n");
						return tria_list;
					}
					if (tria_list) last->next = new;
					else tria_list = new;
					last = new;
					new->tria = tria;
					new->next = NULL;
				}
				geo = geo->next;
			}
		}
		tria = tria->next;
	}

	return tria_list;
}

btg_vertex *collaps_vertices (btg_vertex *v0, btg_vertex *v1, btg_base *base) {

	btg_trialist *t0 = NULL, *t1 = NULL, *t_edit = NULL, *tria = NULL;
	btg_geometry *geo;
	btg_vertex *from, *to;
	int c0 = 0, c1 = 0;

	t0 = get_trias_on_vertex(v0, base->triangle);
	t1 = get_trias_on_vertex(v1, base->triangle);

	tria = t0;
	while (tria) {
		c0++;
		tria = tria->next;
	}
	t_edit = t0;
	from = v0;
	to = v1;

	tria = t1;
	while (tria) {
		c1++;
		tria = tria->next;
	}
	if (c1 < c0) {
		t_edit = t1;
		from = v1;
		to = v0;
	}

	tria = t_edit;
	while (tria) {
		unrec_triangle (base, tria->tria);
		geo = tria->tria->elem->element;
		while (geo) {
			if (geo->vertex == from) {
				geo->vertex = to;
			}
			geo = geo->next;
		}
		if (rec_triangle(base, tria->tria))
			printf("collaps_vertices: invalid triangle! delete.\n");
		tria = tria->next;
	}

	while (t0) {
		tria = t0;
		t0 = t0->next;
		free(tria);
	}

	while (t1) {
		tria = t1;
		t1 = t1->next;
		free(tria);
	}

	return to;
}


btg_edge get_longest (btg_border *border) {

	btg_border *start = NULL, *end = NULL, *start_now = NULL, *end_now = NULL;
	btg_edge edge;
	btg_vertex *v = NULL;
	double len_init, len_now;

	edge.count = 0;
	edge.tria[0] = NULL;
	edge.next = NULL;

	start = border;
	len_init = -1.0;

	start_now = start;
	while (start_now->next) {
		end_now = start_now->next;
		while (end_now) {
			len_now = pydacoras(start_now->vertex, end_now->vertex, USE_RELATIVE);
			if (len_now > len_init) {
				len_init = len_now;
				start = start_now;
				end = end_now;
			}
			end_now = end_now->next;
		}
		start_now = start_now->next;
	}

	edge.vertex[0] = start->vertex;
	if (end) {
		edge.vertex[1] = end->vertex;
	}
	else {
		if (start->vertex == start->edge->vertex[0])
			edge.vertex[1] = start->edge->vertex[1];
		else
			edge.vertex[1] = start->edge->vertex[0];
	}

// if fence isn't closed, check also all distances from last vertex
	if (border->vertex != start_now->edge->vertex[0] && border->vertex != start_now->edge->vertex[1]) {
		printf("get_longest: border isn't closed ... check last segment ...\n");
		if (start_now->vertex == start_now->edge->vertex[0]) {
			v = start_now->edge->vertex[1];
		}
		else if (start_now->vertex == start_now->edge->vertex[1]) {
			v = start_now->edge->vertex[0];
		}

		end_now = border;
		while (end_now) {
			len_now = pydacoras(v, end_now->vertex, USE_RELATIVE);
			if (len_now > len_init) {
				len_init = len_now;
				start = NULL;
				end = end_now;
			}
			end_now = end_now->next;
		}
	}

	if (start == NULL) {
		edge.vertex[0] = v;
		edge.vertex[1] = end->vertex;
	}

	return edge;
}



btg_edge get_shortest (btg_border *border) {

	btg_border *start = NULL, *end = NULL, *start_now = NULL, *end_now = NULL;
	btg_edge edge;
	btg_vertex *v = NULL;
	double len_init, len_now;

	edge.count = 0;
	edge.tria[0] = NULL;
	edge.next = NULL;

	start = border;
	len_init = 9999.0;

	start_now = start;
	while (start_now->next) {
		end_now = start_now->next;
		while (end_now) {
			len_now = pydacoras(start_now->vertex, end_now->vertex, USE_RELATIVE);
			if (len_now < len_init) {
				len_init = len_now;
				start = start_now;
				end = end_now;
			}
			end_now = end_now->next;
		}
		start_now = start_now->next;
	}

	edge.vertex[0] = start->vertex;
	if (end) {
		edge.vertex[1] = end->vertex;
	}
	else {
		if (start->vertex == start->edge->vertex[0])
			edge.vertex[1] = start->edge->vertex[1];
		else
			edge.vertex[1] = start->edge->vertex[0];
	}

// if fence isn't closed, check also all distances from last vertex
	if (border->vertex != start_now->edge->vertex[0] && border->vertex != start_now->edge->vertex[1]) {
		printf("get_shortest: border isn't closed ... check last segment ...\n");
		if (start_now->vertex == start_now->edge->vertex[0]) {
			v = start_now->edge->vertex[1];
		}
		else if (start_now->vertex == start_now->edge->vertex[1]) {
			v = start_now->edge->vertex[0];
		}

		end_now = border;
		while (end_now) {
			len_now = pydacoras(v, end_now->vertex, USE_RELATIVE);
			if (len_now < len_init) {
				len_init = len_now;
				start = NULL;
				end = end_now;
			}
			end_now = end_now->next;
		}
	}

	if (start == NULL) {
		edge.vertex[0] = v;
		edge.vertex[1] = end->vertex;
	}

	return edge;
}



btg_normal *vector2normal (vector vec, btg_base *base) {

	btg_normal *normal = NULL;

	normal = base->normal;
	while (normal->next) normal = normal->next;

	if ((normal->next = malloc(sizeof(*normal))) == NULL) {
		fprintf(stderr, "no memory left for normal! break.\n");
		return NULL;
	}
	normal = normal->next;
	normal->valid = 1;
	normal->index = 0;
	normal->count = 0;
	normal->alias = NULL;
	normal->x = (char) ((vec.x * 127.5) + 127.5);
	normal->y = (char) ((vec.y * 127.5) + 127.5);
	normal->z = (char) ((vec.z * 127.5) + 127.5);
	normal->next = NULL;

	return normal;
}

btg_normal *get_normal_from_neighbor (btg_vertex *v, btg_triangle *tria) {

	btg_normal *normal = NULL;
	btg_geometry *now = tria->elem->element;

	while (now) {
		if (now->vertex == v) normal = now->normal;
		now = now->next;
	}

	return normal;
}

btg_texcoo *get_texcoo_from_neighbor (btg_vertex *v, btg_triangle *tria) {

	btg_texcoo *texcoo = NULL;
	btg_geometry *now = tria->elem->element;

	while (now) {
		if (now->vertex == v) texcoo = now->texcoo;
		now = now->next;
	}

	return texcoo;
}

btg_color *get_color_from_neighbor (btg_vertex *v, btg_triangle *tria) {

	btg_color *color = NULL;
	btg_geometry *now = tria->elem->element;

	while (now) {
		if (now->vertex == v) color = now->color;
		now = now->next;
	}

	return color;
}

int calc_texcoo (btg_triangle *triangle, btg_object *object, btg_base *base) {

	int i, j, found;
	double len[3], v_len, t_len, v_angle, t_angle, factor, rotate;
	btg_triangle *tria[3];
	btg_geometry *geo_found, *geo_new, *first = NULL, *second = NULL, *third = NULL;
	btg_vertex temp_texcoo[3];
	btg_texcoo *texcoo;

	for (i = 0 ; i < 3 ; i++) {
		tria[i] = NULL;
		len[i] = 0.0;
		for (j = 0 ; j < 10 ; j++) {
			if (triangle->edge[i]->tria[j] && triangle->edge[i]->tria[j]->object == triangle->object && triangle->edge[i]->tria[j] != triangle) {
				tria[i] = triangle->edge[i]->tria[j];
			}
		}
		if (tria[i]) {
			len[i] = pydacoras(triangle->edge[i]->vertex[0], triangle->edge[i]->vertex[1], USE_PROJECTION);
		}
	}

	found = 0;
	for (i = 0 ; i < 3 ; i++) {
		if (len[i] > len[found]) found = i;
	}

	if (tria[found] == NULL) return -1;

	geo_new = triangle->elem->element;
	while (geo_new) {
		geo_found = tria[found]->elem->element;
		while (geo_found) {
			if (geo_found->vertex == geo_new->vertex) {
				geo_new->texcoo = geo_found->texcoo;
				geo_found->texcoo->count++;
				if (!first) first = geo_new;
				else if (!second) second = geo_new;
				else fprintf(stderr, "have more than 2 texcoos found!\n");
			}
			geo_found = geo_found->next;
		}
		geo_new = geo_new->next;
	}

	if (!first) {
		fprintf(stderr, "havn't found first texcoo!\n");
		return 1;
	}
	if (!second) {
		fprintf(stderr, "havn't found second texcoo!\n");
		return 2;
	}
	geo_new = triangle->elem->element;
	while (geo_new) {
		if (geo_new != first && geo_new != second) {
			third = geo_new;
		}
		geo_new = geo_new->next;
	}
	if (!third) {
		fprintf(stderr, "havn't found third texcoo!\n");
		return 3;
	}

	texcoo = base->texcoo;
	while (texcoo->next) texcoo = texcoo->next;

	if ((texcoo->next = malloc(sizeof(*texcoo))) == NULL) {
		fprintf(stderr, "no memory left for texcoo! break.\n");
		return 4;
	}
	texcoo = texcoo->next;
	texcoo->valid = 1;
	texcoo->index = 0;
	texcoo->count = 1;
	texcoo->alias = NULL;
	texcoo->next = NULL;

	temp_texcoo[0].projection.x = first->texcoo->u;
	temp_texcoo[0].projection.y = first->texcoo->v;
	temp_texcoo[0].projection.z = 0.0;
	temp_texcoo[1].projection.x = second->texcoo->u;
	temp_texcoo[1].projection.y = second->texcoo->v;
	temp_texcoo[1].projection.z = 0.0;

	t_len = pydacoras(&temp_texcoo[0], &temp_texcoo[1], USE_PROJECTION);
	factor = t_len / len[found];
	v_len = pydacoras(first->vertex, third->vertex, USE_RELATIVE);
	t_len = v_len * factor;

	v_angle = get_angle(first->vertex, second->vertex);
	t_angle = get_angle(&temp_texcoo[0], &temp_texcoo[1]);
	rotate = t_angle - v_angle;
	if (rotate < 0.0) rotate += (2 * M_PI);
	v_angle = get_angle(first->vertex, third->vertex);
	t_angle = v_angle + rotate;
	if (t_angle > (2 * M_PI)) t_angle -= (2 * M_PI);

	texcoo->u = first->texcoo->u + (cos(t_angle) * t_len);
	texcoo->v = first->texcoo->v + (sin(t_angle) * t_len);
	third->texcoo = texcoo;
	return 0;
}

int project_texcoo (btg_triangle *triangle, btg_object *object, btg_base *base) {

	btg_texcoo *texcoo;
	btg_geometry *geo;

	geo = triangle->elem->element;
	texcoo = base->texcoo;
	while (texcoo->next) texcoo = texcoo->next;

	while (geo) {
		if ((texcoo->next = malloc(sizeof(*texcoo))) == NULL) {
			fprintf(stderr, "no memory left for texcoo! break.\n");
			return 1;
		}
		texcoo = texcoo->next;
		texcoo->valid = 1;
		texcoo->index = 0;
		texcoo->count = 1;
		texcoo->alias = NULL;
		texcoo->next = NULL;

		geo->texcoo = texcoo;
		texcoo->u = geo->vertex->projection.x * 0.001;
		texcoo->v = geo->vertex->projection.y * 0.001;

		geo = geo->next;
	}
	return 0;
}

btg_triangle *build_triangle (btg_border *b0, btg_border *b1, btg_border *b2, btg_base *base, short cnt) {

	btg_object *obj[4] = {NULL};
	btg_element *new_elem = NULL;
	btg_geometry *geo = NULL;
	btg_normal *normal = NULL;
	vector area_vec, geo_vec;
	double len[3], angle;
	short mat;

	geo_vec.x = base->bsphere->coord.x;
	geo_vec.y = base->bsphere->coord.y;
	geo_vec.z = base->bsphere->coord.z;
	geo_vec = vecunique(geo_vec);
	area_vec = area_normal (b0->vertex, b1->vertex, b2->vertex);
	angle = vecphi(geo_vec, area_vec);
	if (angle > M_PI_2) {
		area_vec.x *= -1.0;
		area_vec.y *= -1.0;
		area_vec.z *= -1.0;
	}
	normal = vector2normal(area_vec, base);

// get material with longest edges
	len[0] = pydacoras(b0->vertex, b1->vertex, USE_RELATIVE);
	if (b0->edge)
		obj[0] = b0->edge->tria[0]->object;
	else
		obj[0] = NULL;

	len[1] = pydacoras(b1->vertex, b2->vertex, USE_RELATIVE);
	if (b1->edge)
		obj[1] = b1->edge->tria[0]->object;
	else
		obj[1] = NULL;

	len[2] = pydacoras(b2->vertex, b0->vertex, USE_RELATIVE);
	if (b2->edge)
		obj[2] = b2->edge->tria[0]->object;
	else
		obj[2] = NULL;

	if (obj[0] && obj[1] && strcmp(obj[0]->prop_material, obj[1]->prop_material) == 0) {
		len[0] += len[1];
		len[1] = 0.0;
		obj[1] = NULL;
	}
	if (obj[0] && obj[2] && strcmp(obj[0]->prop_material, obj[2]->prop_material) == 0) {
		len[0] += len[2];
		len[2] = 0.0;
		obj[2] = NULL;
	}
	if (obj[1] && obj[2] && strcmp(obj[1]->prop_material, obj[2]->prop_material) == 0) {
		len[1] += len[2];
		len[2] = 0.0;
		obj[2] = NULL;
	}

	mat = 0;
	if (obj[1] && len[1] > len[mat]) mat = 1;
	if (obj[2] && len[2] > len[mat]) mat = 2;

	if (base->material) {
		btg_triangle *tria = base->triangle;
		while (tria && strcmp(tria->object->prop_material, base->material)) tria = tria->next;
		if (tria) {
			mat = 3;
			obj[mat] = tria->object;
		}
	}

	new_elem = obj[mat]->elem_list;
	while (new_elem->next) new_elem = new_elem->next;
	if ((new_elem->next = malloc(sizeof(*new_elem))) == NULL) {
		fprintf(stderr, "no memory left for element! break.\n");
		return NULL;
	}
	new_elem = new_elem->next;
	new_elem->valid = 1;
	new_elem->count = 3;
	new_elem->num_bytes = 0;
	new_elem->element = NULL;
	new_elem->next = NULL;

	if ((geo = malloc(sizeof(*geo))) == NULL) {
		fprintf(stderr, "no memory left for geometry! break.\n");
		return NULL;
	}
	new_elem->element = geo;

	geo->valid = 1;
	if (obj[mat]->prop_mask & MASK_VERTEX)
		geo->vertex = b0->vertex;
	else
		geo->vertex = NULL;

	if (obj[mat]->prop_mask & MASK_NORMAL)
		geo->normal = normal;
	else
		geo->normal = NULL;

	geo->texcoo = NULL;

	if (obj[mat]->prop_mask & MASK_COLOR)
		geo->color  = get_color_from_neighbor(b0->vertex, b0->edge->tria[0]);
	else
		geo->color  = NULL;

	if ((geo->next = malloc(sizeof(*geo))) == NULL) {
		fprintf(stderr, "no memory left for geometry! break.\n");
		return NULL;
	}
	geo = geo->next;

	geo->valid = 1;
	if (obj[mat]->prop_mask & MASK_VERTEX)
		geo->vertex = b1->vertex;
	else
		geo->vertex = NULL;

	if (obj[mat]->prop_mask & MASK_NORMAL)
		geo->normal = normal;
	else
		geo->normal = NULL;

	geo->texcoo = NULL;

	if (obj[mat]->prop_mask & MASK_COLOR)
		geo->color  = get_color_from_neighbor(b1->vertex, b1->edge->tria[0]);
	else
		geo->color  = NULL;

	if ((geo->next = malloc(sizeof(*geo))) == NULL) {
		fprintf(stderr, "no memory left for geometry! break.\n");
		return NULL;
	}
	geo = geo->next;

	geo->valid = 1;
	if (obj[mat]->prop_mask & MASK_VERTEX)
		geo->vertex = b2->vertex;
	else
		geo->vertex = NULL;

	if (obj[mat]->prop_mask & MASK_NORMAL)
		geo->normal = normal;
	else
		geo->normal = NULL;

	geo->texcoo = NULL;

	if (obj[mat]->prop_mask & MASK_COLOR)
		geo->color  = get_color_from_neighbor(b2->vertex, b2->edge->tria[0]);
	else
		geo->color  = NULL;

	geo->next = NULL;

	if (add_triangle (new_elem, base, obj[mat])) {
		fprintf(stderr, "there was a problem while adding triangle! break.\n");
		return NULL;
	}

	if (obj[mat]->prop_mask & MASK_TEXCOO) {
		if (calc_texcoo (base->triangle_last, obj[mat], base) < 0) {
			fprintf(stderr, "no texture found!\n");
			project_texcoo(base->triangle_last, obj[mat], base);
		}
	}

	return base->triangle_last;
}



void close_hole (btg_fence *fence, btg_base *base) {

	btg_border *now, *prev, *first, *second, *third;
	btg_triangle *t_before, *t_after;
	btg_vertex *v_res, *help;
	int i, mark, cnt, straight;
	double len = 0.0, len_now = 0.0, *len_all;

	straight = check_vectorline(fence, USE_PROJECTION);
// look for edges shorter than 0.25cm
	while (len < 0.25) {
		len = 999999.0;
		first = second = third = prev = NULL;
		now = fence->border;
		cnt = 0;

// search shortest edge
		while (now) {
			if (straight)
				len_now = pydacoras(now->edge->vertex[0], now->edge->vertex[1], USE_PROJECTION);
			else
				len_now = pydacoras(now->edge->vertex[0], now->edge->vertex[1], USE_RELATIVE);
			if (len_now < len) {
				first = prev;
				second = now;
				third = now->next;
				len = len_now;
			}
			cnt++;
			prev = now;
			now = now->next;
		}

		if (len < 0.25) {
// make sure we have first, second and third
			if (!third) third = fence->border;
			if (!first) {
				first = fence->border;
				while (first->next) first = first->next;
			}
// remember affected triangles - second edge will collaps, don't need to remember this triangle (will be invalid)
			t_before = first->edge->tria[0];
			t_after = third->edge->tria[0];
			v_res = collaps_vertices(second->edge->vertex[0], second->edge->vertex[1], base);

			if (first->edge->count == 0) {
				printf("adjust first triangle ...\n");
				for (i = 0 ; i < 3 ; i++) {
					if ((t_before->edge[i]->vertex[0] == first->vertex && t_before->edge[i]->vertex[1] == v_res) ||
					    (t_before->edge[i]->vertex[1] == first->vertex && t_before->edge[i]->vertex[0] == v_res))
					{
						printf("edge found.\n");
						first->edge = t_before->edge[i];
					}
				}
			}

			if (third->edge->count == 0) {
				printf("adjust second triangle ...\n");
				if (third->edge->vertex[0] == third->vertex) help = third->edge->vertex[1];
				if (third->edge->vertex[1] == third->vertex) help = third->edge->vertex[0];
				third->vertex = v_res;
				for (i = 0 ; i < 3 ; i++) {
					if ((t_after->edge[i]->vertex[0] == help && t_after->edge[i]->vertex[1] == v_res) ||
					    (t_after->edge[i]->vertex[1] == help && t_after->edge[i]->vertex[0] == v_res))
					{
						printf("edge found.\n");
						third->edge = t_after->edge[i];
					}
				}
			}

			printf("point first to third ...\n");
			if (first->next) {
				if (first->next->next)
					first->next = first->next->next;
				else
					first->next = NULL;
			}

			if (fence->border == second)
				fence->border = second->next;

			free(second);
		}
	}

	printf("closing hole ...\n");
// close the big hole with 'cnt' segments
	if ((len_all = malloc(sizeof(*len_all) * cnt)) == NULL) {
		fprintf(stderr, "no memory left for lenght! break.\n");
		return;
	}

	while (cnt > 3) {
		first = fence->border;
		second = first->next;
		third = second->next;
		i = 0;
// search shortest edge
		while (first) {
			len_all[i++] = pydacoras(first->vertex, third->vertex, USE_RELATIVE);
			first = first->next;
			if (first) second = first->next;
			if (second == NULL) second = fence->border;
			if (second) third = second->next;
			if (third == NULL) third = fence->border;
		}

// search shortest edge that build a triangle to the appropriate side
		first = NULL;
		while (!first) {
			now = fence->border;
			len = 999999.0;
			for (i = 0 ; i < cnt ; i++) {
				if (len_all[i] < len) {
					len = len_all[i];
					mark = i;
					prev = now;
				}
				now = now->next;
			}

			first = prev;
			if (first) second = first->next;
			if (second == NULL) second = fence->border;
			if (second) third = second->next;
			if (third == NULL) third = fence->border;

			if (get_side(first->vertex, third->vertex, second->vertex) == fence->turn) {
				len_all[mark] = 999999.0;
				first = NULL;
			}
		}

// build a triangle with first, second and third
		t_after = build_triangle (first, second, third, base, 2);
		for (i = 0 ; i < 3 ; i++) {
			if ((t_after->edge[i]->vertex[0] == first->vertex && t_after->edge[i]->vertex[1] == third->vertex) ||
			    (t_after->edge[i]->vertex[1] == first->vertex && t_after->edge[i]->vertex[0] == third->vertex)
		    ) {
				printf("edge found.\n");
				first->edge = t_after->edge[i];
			}
		}

		printf("point first to third ...\n");
		if (first->next) {
			if (first->next->next)
				first->next = first->next->next;
			else
				first->next = NULL;
		}
		else {
			fence->border = third;
		}
		free (second);
		cnt--;
	}

	if (cnt == 3) {
// build the last triangle
		first = fence->border;
		second = first->next;
		third = second->next;
		t_after = build_triangle (first, second, third, base, 3);
	}

	free_border (fence->border);
	free (fence);
	free (len_all);
}
