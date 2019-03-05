#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "check.h"
#include "bsphere.h"
#include "vertex.h"
#include "normal.h"
#include "color.h"
#include "texcoo.h"
#include "point.h"
#include "triangle.h"
#include "edge.h"
#include "airport.h"

void remove_unused (btg_header *head) {
	remove_unused_bspheres (head->base.bsphere);
	remove_unused_vertices (head->base.vertex);
	remove_unused_normals  (head->base.normal);
	remove_unused_colors   (head->base.color);
	remove_unused_texcoos  (head->base.texcoo);
}

void check_ojects (btg_header *head) {
	check_points (head->base.point);
	check_triangle (&head->base, head->base.triangle);
}

void check_base (btg_header *head) {
	check_same_vertices (head->base.vertex);
	check_same_normals (head->base.normal);
	check_same_colors (head->base.color);
	check_same_texcoos (head->base.texcoo);
}

int usage (btg_header *header) {

	btg_object *obj = header->object;

	while (obj) {
		if (obj->obj_type == OBJ_POINTS) {
			if (add_point (obj->elem_list, &header->base, obj)) {
				fprintf(stderr, "problem while add points! break.\n");
				return 1;
			}
		}
		if (obj->obj_type == OBJ_TRIS) {
			if (add_triangle (obj->elem_list, &header->base, obj)) {
				fprintf(stderr, "problem while add triangles! break.\n");
				return 1;
			}
		}
		obj = obj->next;
	}
	return 0;
}

void check (btg_header *head) {

//	btg_fence *outside = NULL, *airport = NULL;
	btg_fence *border = NULL, *found = NULL;
	btg_border *now;

	fprintf(stdout, "********** check base **********\n");
	fprintf(stderr, "********** check base **********\n");
	check_base (head);

	fprintf(stdout, "********** build usage **********\n");
	fprintf(stderr, "********** build usage **********\n");
	usage (head);

	fprintf(stdout, "********** check objects **********\n");
	fprintf(stderr, "********** check objects **********\n");
	check_ojects (head);

	fprintf(stdout, "********** check edges **********\n");
	fprintf(stderr, "********** check edges **********\n");
	check_edges (&head->base, head->base.edge);

	fprintf(stdout, "********** collect border **********\n");
	fprintf(stderr, "********** collect border **********\n");
	border = collect_border (&head->base.edge);

	fprintf(stdout, "********** search fence **********\n");
	fprintf(stderr, "********** search fence **********\n");
	while (border->border) {
		if ((found = find_fence (border))) {
			if ((found = examine_fence (found, &head->base))) {
				if (head->base.fence_last) {
					head->base.fence_last->next = found;
				}
				else {
					head->base.fence = found;
				}
				head->base.fence_last = found;
				now = found->border;
				while (now) {
					printf("%.9f,%.9f,%.9f\n", now->vertex->relative.x, now->vertex->relative.y, now->vertex->relative.z);
					now = now->next;
				}
			}
		}
	}

	free_border (border->border);
	free (border);
	border = NULL;

	if (head->runway) {
		fprintf(stdout, "********** update runway info **********\n");
		fprintf(stderr, "********** update runway info **********\n");
		set_runway_info (&head->base, head->runway);
		change_als (&head->base, head->object, head->runway);
	}

	fprintf(stdout, "********** remove unused **********\n");
	fprintf(stderr, "********** remove unused **********\n");
	remove_unused (head);
}
