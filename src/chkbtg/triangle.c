#include <stdlib.h>
#include <string.h>

#include "triangle.h"
#include "geometry.h"
#include "edge.h"

int add_triangle (btg_element *elem, btg_base *base, btg_object *object) {

	int result = 0;
	btg_triangle *triangle = NULL;

	if (elem == NULL) {
		fprintf(stderr, "pointer to element is NULL! break.\n");
		return 1;
	}

	if (base == NULL) {
		fprintf(stderr, "pointer to base is NULL! break.\n");
		return 2;
	}

	if (object == NULL) {
		fprintf(stderr, "pointer to object is NULL! break.\n");
		return 3;
	}

	printf("add triangles from %s\n", object->prop_material);
	while (elem) {
		if ((triangle = malloc(sizeof(*triangle))) == NULL) {
			fprintf(stderr, "No memory left for triangle! break.\n");
			return 3;
		}
		triangle->object = object;
		triangle->elem = elem;
		triangle->edge[0] = NULL;
		triangle->edge[1] = NULL;
		triangle->edge[2] = NULL;
		triangle->next = NULL;

		if (base->triangle_last) {
			base->triangle_last->next = triangle;
			base->triangle_last = triangle;
		}
		else base->triangle = base->triangle_last = triangle;
		result += rec_triangle (base, triangle);
		elem = elem->next;
	}

	if (result) printf("there was %d invalid triangles!\n", result);
	return 0;
}

void free_triangle (btg_triangle *triangle) {
	btg_triangle *temp = NULL;
	while (triangle) {
		temp = triangle->next;
		free(triangle);
		triangle = temp;
	}
}

int rec_triangle (btg_base *base, btg_triangle *triangle) {

	int cnt = 0, result = 0;
	btg_geometry *geo = triangle->elem->element;
	btg_vertex *corner[3];

	while (geo) {
		unalias_geometry(geo);
		corner[cnt++] = geo->vertex;
		geo = geo->next;
	}

	if (corner[0] != corner[1] && corner[1] != corner[2] && corner[2] != corner[0]) {
		geo = triangle->elem->element;
		while (geo) {
			rec_geometry(geo);
			geo = geo->next;
		}
		if (strncmp("lf_", triangle->object->prop_material, 3)) {
			for (cnt = 0 ; cnt < 3 ; cnt++) {
				triangle->edge[cnt] = rec_edge (&base->edge, &base->edge_last, corner[cnt], corner[(cnt + 1) % 3], triangle);
			}
		}
		triangle->elem->valid = 1;
	}
	else {
		triangle->elem->valid = 0;
		result = 1;
	}

	return result;
}

void unrec_triangle (btg_base *base, btg_triangle *triangle) {
	int i;
	btg_geometry *geo = triangle->elem->element;
	for (i = 0 ; i < 3 ; i++) {
		unrec_edge (triangle->edge[i], triangle);
	}
	while (geo) {
		unrec_geometry (geo);
		geo = geo->next;
	}
	triangle->elem->valid = 0;
}

void check_triangle (btg_base *base, btg_triangle *triangle_start) {

	btg_triangle *triangle = triangle_start, *triangle_temp;
	btg_geometry *geo, *geo_temp;
	int cnt, error = 0, flag;

	while (triangle) {
		if (triangle->elem->valid) {
			geo = triangle->elem->element;
			while (geo) {
				unalias_geometry (geo);
				geo = geo->next;
			}
		}
		triangle = triangle->next;
	}

	triangle = triangle_start;
	while (triangle) {
		if (triangle->elem->valid) {
			flag = 0;
			geo = triangle->elem->element;
			while (geo) {
				geo_temp = geo->next;
				while (geo_temp) {
					if (geo->vertex == geo_temp->vertex) flag = 1;
					geo_temp = geo_temp->next;
				}
				geo = geo->next;
			}
			if (flag) {
				unrec_triangle (base, triangle);
				error++;
			}
		}
		triangle = triangle->next;
	}
	if (error) printf("%d triangles deleted.\n", error);

	error = 0;
	triangle = triangle_start;
	while (triangle) {
		if (triangle->elem->valid) {
			triangle_temp = triangle->next;
			while (triangle_temp) {
				if (triangle_temp->elem->valid) {
					cnt = 0;
					geo = triangle->elem->element;
					while (geo) {
						geo_temp = triangle_temp->elem->element;
						while (geo_temp) {
							if (geo->vertex == geo_temp->vertex) cnt++;
							geo_temp = geo_temp->next;
						}
						geo = geo->next;
					}
					if (cnt == 3) {
						unrec_triangle (base, triangle_temp);
						error++;
					}
				}
				triangle_temp = triangle_temp->next;
			}
		}
		triangle = triangle->next;
	}
	if (error) printf("%d dublicated triangles deleted.\n", error);
}
