#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "airport.h"
#include "object.h"
#include "element.h"
#include "geometry.h"
#include "vertex.h"
#include "edge.h"
#include "point.h"

#ifndef M_PIl
#define M_PIl M_PI
#endif

const int als1w_size = 92;
const float als1w_head[] = {
	90.0,  90.0,  90.0,
	120.0, 120.0, 120.0,
	150.0, 150.0, 150.0,
	180.0, 180.0, 180.0,
	210.0, 210.0, 210.0,
	240.0, 240.0, 240.0,
	270.0, 270.0, 270.0,
	300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0,
	330.0, 330.0, 330.0,
	360.0, 360.0, 360.0,
	390.0, 390.0, 390.0,
	420.0, 420.0, 420.0,
	450.0, 450.0, 450.0,
	480.0, 480.0, 480.0,
	510.0, 510.0, 510.0,
	540.0, 540.0, 540.0,
	570.0, 570.0, 570.0,
	600.0, 600.0, 600.0,
	630.0, 630.0, 630.0,
	660.0, 660.0, 660.0,
	690.0, 690.0, 690.0,
	720.0, 720.0, 720.0,
	750.0, 750.0, 750.0,
	780.0, 780.0, 780.0,
	810.0, 810.0, 810.0,
	840.0, 840.0, 840.0,
	870.0, 870.0, 870.0,
	900.0, 900.0, 900.0
};
const float als1w_side[] = {
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0, 4.5, 6.0, 7.5, 9.0, 10.5, 12.0, 13.5, 15.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0
};

const int als1r_size = 11;
const float als1r_head[] = {
	30.0, 30.0, 30.0, 30.0, 30.0,
	60.0, 60.0, 60.0, 60.0, 60.0, 60.0
};
const float als1r_side[] = {
	9.0, 10.5, 12.0, 13.5, 15.0,
	0.0, 1.0, 2.0, 9.0, 10.5, 12.0
};

const int als2w_size = 102;
const float als2w_head[] = {
	30.0,  30.0,  30.0,
	60.0,  60.0,  60.0,
	90.0,  90.0,  90.0,
	120.0, 120.0, 120.0,
	150.0, 150.0, 150.0, 150.0, 150.0, 150.0, 150.0,
	180.0, 180.0, 180.0,
	210.0, 210.0, 210.0,
	240.0, 240.0, 240.0,
	270.0, 270.0, 270.0,
	300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0,
	330.0, 330.0, 330.0,
	360.0, 360.0, 360.0,
	390.0, 390.0, 390.0,
	420.0, 420.0, 420.0,
	450.0, 450.0, 450.0,
	480.0, 480.0, 480.0,
	510.0, 510.0, 510.0,
	540.0, 540.0, 540.0,
	570.0, 570.0, 570.0,
	600.0, 600.0, 600.0,
	630.0, 630.0, 630.0,
	660.0, 660.0, 660.0,
	690.0, 690.0, 690.0,
	720.0, 720.0, 720.0,
	750.0, 750.0, 750.0,
	780.0, 780.0, 780.0,
	810.0, 810.0, 810.0,
	840.0, 840.0, 840.0,
	870.0, 870.0, 870.0,
	900.0, 900.0, 900.0
};
const float als2w_side[] = {
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0, 4.5, 6.0, 7.5, 9.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0, 4.5, 6.0, 7.5, 9.0, 10.5, 12.0, 13.5, 15.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0
};

const int als2r_size = 27;
const float als2r_head[] = {
	30.0,  30.0,  30.0,
	60.0,  60.0,  60.0,
	90.0,  90.0,  90.0,
	120.0, 120.0, 120.0,
	150.0, 150.0, 150.0,
	180.0, 180.0, 180.0,
	210.0, 210.0, 210.0,
	240.0, 240.0, 240.0,
	270.0, 270.0, 270.0
};
const float als2r_side[] = {
	12.0, 13.5, 15.0,
	12.0, 13.5, 15.0,
	12.0, 13.5, 15.0,
	12.0, 13.5, 15.0,
	12.0, 13.5, 15.0,
	12.0, 13.5, 15.0,
	12.0, 13.5, 15.0,
	12.0, 13.5, 15.0,
	12.0, 13.5, 15.0
};

const int sals_size = 26;
const float sals_head[] = {
	60.0, 60.0, 60.0,
	120.0, 120.0, 120.0,
	180.0, 180.0, 180.0,
	240.0, 240.0, 240.0,
	300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0,
	360.0, 360.0, 360.0,
	420.0, 420.0, 420.0
};
const float sals_side[] = {
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0, 4.5, 6.0, 7.5, 9.0, 10.5,
	0.0, 1.0, 2.0,
	0.0, 1.0, 2.0,
};

const int mals_size = 26;
const float mals_head[] = {
	60.0, 60.0, 60.0,
	120.0, 120.0, 120.0,
	180.0, 180.0, 180.0,
	240.0, 240.0, 240.0,
	300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0, 300.0,
	360.0, 360.0, 360.0,
	420.0, 420.0, 420.0
};
const float mals_side[] = {
	0.00, 0.75, 1.50,
	0.00, 0.75, 1.50,
	0.00, 0.75, 1.50,
	0.00, 0.75, 1.50,
	0.00, 0.75, 1.50, 7.00, 7.75, 8.50, 9.25, 10.00,
	0.00, 0.75, 1.50,
	0.00, 0.75, 1.50,
};



void set_runway_info (btg_base *base, runway_info *runway) {

	int cnt;
	double len, len_now;
	long double msl = 0.0;
	btg_triangle *tria = NULL;
	btg_edge *all = NULL, *last = NULL;
	btg_fence *border = NULL, *fence = NULL, *prev = NULL, *now;
	btg_vertex *origin = NULL, *origin_last = NULL, *new, tmp;
	runway_info *rwy = NULL;
	coord_geo  g_coord;
	coord_cart c_coord;

	if (base == NULL) {
		fprintf(stderr, "pointer to base is NULL! break.\n");
		return;
	}

	if (runway == NULL) {
		fprintf(stderr, "pointer to runway is NULL! break.\n");
		return;
	}

// build a edge list only from thresholds
	tria = base->triangle;
	while (tria) {
		if (tria->elem->valid && (
	    strncmp(&tria->object->prop_material[3], "threshold", 9) == 0 ||
	    strncmp(&tria->object->prop_material[3], "no_threshold", 12) == 0)
	    )
		{
			for (cnt = 0 ; cnt < 3 ; cnt++) {
				rec_edge(&all, &last, tria->edge[cnt]->vertex[0], tria->edge[cnt]->vertex[1], tria);
			}
		}
		tria = tria->next;
	}
	if (!all) return;

// collect all border edges
	border = collect_border (&all);
	free_edges (all);

// sort fences (every runway should have 2 thresholds)
	while (border && border->border && (now = find_fence (border))) {
		if (prev) prev->next = now;
		else fence = now;
		prev = now;
	}
	free_border(border->border);
	free (border);
	border = NULL;

// get the origin of all thresholds (intersection between threshold and centerline)
	now = fence;
	while (now) {
		if ((new = get_origin (now))) {
			if (origin_last) origin_last->next = new;
			else origin = new;
			origin_last = new;
		}
		else {
			fprintf(stderr, "there is a problem with a threshold!\n");
		}
		now = now->next;
	}

// get average MSL from all origins
	cnt = 0;
	new = origin;
	while (new) {
		c_coord.x = new->absolute.x;
		c_coord.y = new->absolute.y;
		c_coord.z = new->absolute.z;
		g_coord = cart2geo(c_coord);
		msl += g_coord.msl;
		cnt++;
		new = new->next;
	}
	msl /= cnt;

// assign every threshold a origin
	rwy = runway;
	while (rwy) {
		for (cnt = 0 ; cnt < 2 ; cnt++) {
			g_coord.lon = (rwy->threshold[cnt].lon * M_PI) / 180.0;
			g_coord.lat = (rwy->threshold[cnt].lat * M_PI) / 180.0;
			g_coord.msl = msl;
			c_coord = geo2cart(g_coord);
			tmp.absolute.x = c_coord.x;
			tmp.absolute.y = c_coord.y;
			tmp.absolute.z = c_coord.z;
			len = 100.0;
			new = origin;
			while (new) {
				len_now = pydacoras(&tmp, new, USE_ABSOLUTE);
				if (len_now < 50.0 && len_now < len) {
					rwy->threshold[cnt].origin = new;
					len = len_now;
				}
				new = new->next;
			}
		}
		rwy = rwy->next;
	}

}



void change_als (btg_base *base, btg_object *object, runway_info *runway) {

	int cnt, area_cnt, inside;
	runway_info *rwy = NULL;
	vector heading_r, toside_r, heading_p, toside_p;
	btg_vertex area[4];
	btg_normal *light_normal = NULL;
	btg_point *point = NULL;
	btg_object *new_obj = NULL;
	coord_geo  g_coord;
	coord_cart c_coord;
	als_lights lights;

	rwy = runway;
	while (rwy) {
		for (cnt = 0 ; cnt < 2 ; cnt++) {
			if (rwy->threshold[cnt].als_layout != ALS_NOOP) {
				if (cnt)
					get_rwy_vector (&rwy->threshold[1], &rwy->threshold[0], &heading_p, &toside_p);
				else
					get_rwy_vector (&rwy->threshold[0], &rwy->threshold[1], &heading_p, &toside_p);

				area[0].projection.x = area[1].projection.x = rwy->threshold[cnt].origin->projection.x - (-20.0 * heading_p.x);
				area[0].projection.y = area[1].projection.y = rwy->threshold[cnt].origin->projection.y - (-20.0 * heading_p.y);

				area[0].projection.x = area[0].projection.x - (100.0 * toside_p.x);
				area[0].projection.y = area[0].projection.y - (100.0 * toside_p.y);

				area[1].projection.x = area[1].projection.x - (-100.0 * toside_p.x);
				area[1].projection.y = area[1].projection.y - (-100.0 * toside_p.y);

				area[2].projection.x = area[3].projection.x = rwy->threshold[cnt].origin->projection.x - (-1000.0 * heading_p.x);
				area[2].projection.y = area[3].projection.y = rwy->threshold[cnt].origin->projection.y - (-1000.0 * heading_p.y);

				area[2].projection.x = area[2].projection.x - (-100.0 * toside_p.x);
				area[2].projection.y = area[2].projection.y - (-100.0 * toside_p.y);

				area[3].projection.x = area[3].projection.x - (100.0 * toside_p.x);
				area[3].projection.y = area[3].projection.y - (100.0 * toside_p.y);

				printf("clear area:\n");
				for (area_cnt = 0 ; area_cnt < 4 ; area_cnt++ ) {
					printf("%f,%f\n", area[area_cnt].projection.x, area[area_cnt].projection.y);
				}

				point = base->point;
				while (point) {
					inside = 0;
					for (area_cnt = 0 ; area_cnt < 4 ; area_cnt++ ) {
						if (get_side(&area[area_cnt], &area[(area_cnt + 1) % 4], point->geo->vertex) == 1) inside++;
					}
					if (inside == 4) {
//						printf("delete light on %f,%f,%f\n", point->geo->vertex->absolute.x, point->geo->vertex->absolute.y, point->geo->vertex->absolute.z);
						unrec_point(point);
					}
					point = point->next;
				}

// build new ALS
// get exact angles from the threshold
				c_coord.x = rwy->threshold[cnt].origin->absolute.x;
				c_coord.y = rwy->threshold[cnt].origin->absolute.y;
				c_coord.z = rwy->threshold[cnt].origin->absolute.z;
				g_coord = cart2geo(c_coord);
// turn vectors from projected runway to real geografic
				heading_r = turnvector(heading_p, g_coord.lon, g_coord.lat);
				toside_r = turnvector(toside_p, g_coord.lon, g_coord.lat);
				light_normal = vector2normal (heading_r, base);

				switch (rwy->threshold[cnt].als_layout) {

					case ALS_CLEAR:
						printf("clear, do nothing ...\n");
						break;

					case ALS_ALSF1:
						printf("build ALSF-I (%dm) ...\n", rwy->threshold[cnt].als_len);
						lights.size = als1w_size;
						lights.head = als1w_head;
						lights.side = als1w_side;
						new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_WHITE_LIGHTS");
						build_als (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], lights, heading_r, toside_r, heading_p, toside_p);
						lights.size = als1r_size;
						lights.head = als1r_head;
						lights.side = als1r_side;
						new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_RED_LIGHTS");
						build_als (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], lights, heading_r, toside_r, heading_p, toside_p);
						new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_SEQUENCED_LIGHTS");
						build_rapid (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], heading_r, toside_r, heading_p, toside_p, 30.0);
						break;

					case ALS_ALSF2:
						printf("build ALSF-II (%dm) ...\n", rwy->threshold[cnt].als_len);
						lights.size = als2w_size;
						lights.head = als2w_head;
						lights.side = als2w_side;
						new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_WHITE_LIGHTS");
						build_als (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], lights, heading_r, toside_r, heading_p, toside_p);
						lights.size = als2r_size;
						lights.head = als2r_head;
						lights.side = als2r_side;
						new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_RED_LIGHTS");
						build_als (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], lights, heading_r, toside_r, heading_p, toside_p);
						new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_SEQUENCED_LIGHTS");
						build_rapid (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], heading_r, toside_r, heading_p, toside_p, 30.0);
						break;

					case ALS_SSALS:
					case ALS_SSALF:
					case ALS_SSALR:
						printf("build SSALS (%dm) ...\n", rwy->threshold[cnt].als_len);
						lights.size = sals_size;
						lights.head = sals_head;
						lights.side = sals_side;
						new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_WHITE_LIGHTS");
						build_als (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], lights, heading_r, toside_r, heading_p, toside_p);

						if (rwy->threshold[cnt].als_layout == ALS_SSALF) {
							printf("add rapid ...\n");
							new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_SEQUENCED_LIGHTS");
							build_rapid (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], heading_r, toside_r, heading_p, toside_p, 60.0);
						}

						if (rwy->threshold[cnt].als_layout == ALS_SSALR) {
							printf("add RAIL ...\n");
							new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_SEQUENCED_LIGHTS");
							build_rail (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], heading_r, toside_r, heading_p, toside_p, 60.0);
						}
						break;

					case ALS_MALS:
					case ALS_MALSF:
					case ALS_MALSR:
						printf("build MALS (%dm) ...\n", rwy->threshold[cnt].als_len);
						lights.size = mals_size;
						lights.head = mals_head;
						lights.side = mals_side;
						new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_WHITE_MEDIUM_LIGHTS");
						build_als (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], lights, heading_r, toside_r, heading_p, toside_p);

						if (rwy->threshold[cnt].als_layout == ALS_MALSF) {
							printf("add rapid ...\n");
							new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_SEQUENCED_LIGHTS");
							build_rapid (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], heading_r, toside_r, heading_p, toside_p, 60.0);
						}

						if (rwy->threshold[cnt].als_layout == ALS_MALSR) {
							printf("add RAIL ...\n");
							new_obj = new_object(object, OBJ_POINTS, (MASK_VERTEX | MASK_NORMAL), "RWY_SEQUENCED_LIGHTS");
							build_rail (new_obj, base->vertex, light_normal, &rwy->threshold[cnt], heading_r, toside_r, heading_p, toside_p, 60.0);
						}
						break;
				}
			}
		}
		rwy = rwy->next;
	}
}



btg_vertex *get_origin (btg_fence *fence) {

	int found;
	btg_edge edge;
	btg_vertex *corner[2] = { NULL }, *first = NULL, *second = NULL, *origin = NULL;
	btg_border *tmp, *prev_tmp;
	btg_geometry *geo;

	edge = get_longest(fence->border);
	corner[0] = edge.vertex[0];
	corner[1] = edge.vertex[1];

	tmp = fence->border;
	prev_tmp = NULL;
	while (tmp) {
		found = 0;
		if (tmp->vertex == corner[0] || tmp->vertex == corner[1]) {
			geo = tmp->edge->tria[0]->elem->element;
			while (geo) {
				if (geo->vertex == corner[0]) {
					if (geo->texcoo->v > 0.5) corner[0] = NULL;
				}
				if (geo->vertex == corner[1]) {
					if (geo->texcoo->v > 0.5) corner[1] = NULL;
				}
				geo = geo->next;
			}
			found++;
		}
		if (found) {
			if (prev_tmp) prev_tmp->next = tmp->next;
			else fence->border = tmp->next;
		}
		else prev_tmp = tmp;
		tmp = tmp->next;
	}

	if (!(first = corner[0])) first = corner[1];

	edge = get_longest(fence->border);
	corner[0] = edge.vertex[0];
	corner[1] = edge.vertex[1];

	tmp = fence->border;
	prev_tmp = NULL;
	while (tmp) {
		found = 0;
		if (tmp->vertex == corner[1] || tmp->vertex == corner[3]) {
			geo = tmp->edge->tria[0]->elem->element;
			while (geo) {
				if (geo->vertex == corner[0]) {
					if (geo->texcoo->v > 0.5) corner[0] = NULL;
				}
				if (geo->vertex == corner[1]) {
					if (geo->texcoo->v > 0.5) corner[1] = NULL;
				}
				geo = geo->next;
			}
			found++;
		}
		if (found) {
			if (prev_tmp) prev_tmp->next = tmp->next;
			else fence->border = tmp->next;
		}
		else prev_tmp = tmp;
		tmp = tmp->next;
	}

	if (!(second = corner[0])) second = corner[1];

	if (first && second) {
		if ((origin = malloc(sizeof(*origin))) == NULL) {
			fprintf(stderr, "no memory left for origin!\n");
			return NULL;
		}

		origin->valid = 1;
		origin->index = 0;
		origin->count = 0;
		origin->alias = NULL;
		origin->next = NULL;

		origin->absolute.x = first->absolute.x - ((first->absolute.x - second->absolute.x) / 2.0);
		origin->absolute.y = first->absolute.y - ((first->absolute.y - second->absolute.y) / 2.0);
		origin->absolute.z = first->absolute.z - ((first->absolute.z - second->absolute.z) / 2.0);

		origin->relative.x = first->relative.x - ((first->relative.x - second->relative.x) / 2.0);
		origin->relative.y = first->relative.y - ((first->relative.y - second->relative.y) / 2.0);
		origin->relative.z = first->relative.z - ((first->relative.z - second->relative.z) / 2.0);

		origin->projection.x = first->projection.x - ((first->projection.x - second->projection.x) / 2.0);
		origin->projection.y = first->projection.y - ((first->projection.y - second->projection.y) / 2.0);
		origin->projection.z = first->projection.z - ((first->projection.z - second->projection.z) / 2.0);

	}

	return origin;
}



void get_rwy_vector (threshold_info *from, threshold_info *to, vector *heading, vector *toside) {
	heading->x = from->origin->projection.x - to->origin->projection.x;
	heading->y = from->origin->projection.y - to->origin->projection.y;
	heading->z = 0.0;
	*heading = vecunique(*heading);
	toside->x = heading->y * -1.0;
	toside->y = heading->x;
	toside->z = 0.0;
}



vector turnvector (vector vec, const double lon, const double lat) {

	double len, angle;

// turn vector from projection into real
	len = vec.z;
	vec.z = vec.y;
	vec.y = vec.x;
	vec.x = len;

	len = sqrt((vec.x * vec.x) + (vec.z * vec.z));
	angle = asin(vec.z / len);
	if (vec.x < 0.0) angle = M_PIl - angle;
	angle += lat;
	vec.x = cos(angle) * len;
	vec.z = sin(angle) * len;

	len = sqrt((vec.x * vec.x) + (vec.y * vec.y));
	angle = asin(vec.y / len);
	if (vec.x < 0.0) angle = M_PIl - angle;
	angle += lon;
	vec.x = cos(angle) * len;
	vec.y = sin(angle) * len;

	return vec;
}



btg_vertex *new_vertex_from_origin (
    btg_vertex *vertex_all, btg_vertex *origin,
    vector heading, vector toside,
    vector heading_proj, vector toside_proj,
    float head, float side
    ) {

	btg_vertex *vertex = NULL;

	if ((vertex = new_vertex(vertex_all)) == NULL) {
		fprintf(stderr, "can't build new vertex! break.\n");
		return NULL;
	}

	vertex->absolute.x = origin->absolute.x + (heading.x * head) + (toside.x * side);
	vertex->absolute.y = origin->absolute.y + (heading.y * head) + (toside.y * side);
	vertex->absolute.z = origin->absolute.z + (heading.z * head) + (toside.z * side);
	vertex->relative.x = origin->relative.x + (heading.x * head) + (toside.x * side);
	vertex->relative.y = origin->relative.y + (heading.y * head) + (toside.y * side);
	vertex->relative.z = origin->relative.z + (heading.z * head) + (toside.z * side);
	vertex->projection.x = origin->projection.x + (heading_proj.x * head) + (toside_proj.x * side);
	vertex->projection.y = origin->projection.y + (heading_proj.y * head) + (toside_proj.y * side);
	vertex->projection.z = origin->projection.z + (heading_proj.z * head) + (toside_proj.z * side);

	return vertex;
}



int build_als (btg_object *obj, btg_vertex *vertex_all, btg_normal *normal, threshold_info *threshold, als_lights lights, vector heading, vector toside, vector heading_p, vector toside_p) {

	int cnt;
	btg_vertex *vertex = NULL, *origin = NULL;
	btg_geometry *geo = NULL, *last = NULL;

	origin = threshold->origin;

	obj->elem_list = new_element();
	obj->elem_cnt++;

	for (cnt = 0 ; cnt < lights.size ; cnt++) {

		vertex = new_vertex_from_origin (vertex_all, origin, heading, toside, heading_p, toside_p, lights.head[cnt], lights.side[cnt]);
		geo = new_geometry (vertex, normal, NULL, NULL);
		rec_geometry (geo);
		if (last) last->next = geo;
		else obj->elem_list->element = geo;
		last = geo;
		obj->elem_list->count++;

		if (lights.side[cnt] != 0.0) {
			vertex = new_vertex_from_origin (vertex_all, origin, heading, toside, heading_p, toside_p, lights.head[cnt], (lights.side[cnt] * -1.0));
			geo = new_geometry (vertex, normal, NULL, NULL);
			rec_geometry (geo);
			if (last) last->next = geo;
			else obj->elem_list->element = geo;
			last = geo;
			obj->elem_list->count++;
		}
	}

	return 0;
}



int build_rail (btg_object *obj, btg_vertex *vertex_all, btg_normal *normal, threshold_info *threshold, vector heading, vector toside, vector heading_p, vector toside_p, float spacing) {

	btg_vertex *vertex = NULL, *origin = NULL;
	btg_geometry *geo = NULL, *last = NULL;

	float dist = 420.0;

	origin = threshold->origin;
	obj->elem_list = new_element();
	obj->elem_cnt++;

	while (dist <= threshold->als_len) {
		vertex = new_vertex_from_origin (vertex_all, origin, heading, toside, heading_p, toside_p, dist, 0.0);
		geo = new_geometry (vertex, normal, NULL, NULL);
		rec_geometry (geo);
		if (last) last->next = geo;
		else obj->elem_list->element = geo;
		last = geo;
		obj->elem_list->count++;
		dist += spacing;
	}

	return 0;
}



int build_rapid (btg_object *obj, btg_vertex *vertex_all, btg_normal *normal, threshold_info *threshold, vector heading, vector toside, vector heading_p, vector toside_p, float spacing) {

	btg_vertex *vertex = NULL, *origin = NULL;
	btg_geometry *geo = NULL, *last = NULL;

	float dist = 300.0;

	origin = threshold->origin;
	obj->elem_list = new_element();
	obj->elem_cnt++;

	while (dist <= threshold->als_len) {
		vertex = new_vertex_from_origin (vertex_all, origin, heading, toside, heading_p, toside_p, dist, 0.0);
		geo = new_geometry (vertex, normal, NULL, NULL);
		rec_geometry (geo);
		if (last) last->next = geo;
		else obj->elem_list->element = geo;
		last = geo;
		obj->elem_list->count++;
		dist += spacing;
	}

	return 0;
}
