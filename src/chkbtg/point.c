#include <stdlib.h>

#include "point.h"
#include "geometry.h"

int add_point (btg_element *elem, btg_base *base, btg_object *object) {

	btg_point *point = NULL;
	btg_geometry *geo = NULL;

	if (elem == NULL) {
		fprintf(stderr, "pointer to element is NULL! break.\n");
		return 1;
	}

	if (base == NULL) {
		fprintf(stderr, "pointer to base is NULL! break.\n");
		return 2;
	}

	while (elem) {
		geo = elem->element;
		while (geo) {
			if ((point = malloc(sizeof(*point))) == NULL) {
				fprintf(stderr, "No memory left for point! break.\n");
				return 3;
			}
			point->object = object;
			point->valid = 1;
			point->geo = geo;
			point->next = NULL;
			if (base->point_last) {
				base->point_last->next = point;
				base->point_last = point;
			}
			else base->point = base->point_last = point;
			rec_point (point);
			geo = geo->next;
		}
		elem = elem->next;
	}
	return 0;
}

void free_point (btg_point *point) {
	btg_point *temp = NULL;
	while (point) {
		temp = point->next;
		free(point);
		point = temp;
	}
}

void rec_point (btg_point *point) {
	unalias_geometry(point->geo);
	rec_geometry(point->geo);
	point->valid = 1;
}

void unrec_point (btg_point *point) {
	unrec_geometry(point->geo);
	point->valid = 0;
}

void check_points (btg_point *point_start) {

	size_t error = 0;
	btg_point *point = point_start, *point_temp;
	btg_geometry *geo, *geo_temp;

	while (point) {
		if (point->geo->valid) {
			geo = point->geo;
			while (geo) {
				unalias_geometry (point->geo);
				geo = geo->next;
			}
		}
		point = point->next;
	}

	point = point_start;
	while (point) {
		if (point->geo->valid) {
			geo = point->geo;
			geo_temp = geo->next;
			while (geo_temp) {
				if (
			    geo_temp->valid &&
			    geo_temp->vertex == geo->vertex &&
			    geo_temp->color  == geo->color  &&
			    geo_temp->normal == geo->normal &&
			    geo_temp->texcoo == geo->texcoo
			    ) {
					unrec_geometry(geo_temp);
					geo_temp->valid = 0;
					error++;
				}
				geo_temp = geo_temp->next;
			}
			point_temp = point->next;
			while (point_temp) {
				geo_temp = point_temp->geo;
				while (geo_temp) {
					if (
				    geo_temp->valid &&
				    geo_temp->vertex == geo->vertex &&
				    geo_temp->color  == geo->color  &&
				    geo_temp->normal == geo->normal &&
				    geo_temp->texcoo == geo->texcoo
				    ) {
						unrec_geometry(geo_temp);
						geo_temp->valid = 0;
						error++;
					}
					geo_temp = geo_temp->next;
				}
				point_temp = point_temp->next;
			}
		}
		point = point->next;
	}

	if (error) printf("%zd points deleted.\n", error);
}
