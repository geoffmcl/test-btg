#define _GNU_SOURCE

#include <stdlib.h>
#include <math.h>

#include "vertex.h"
#include "raw.h"
#include "coord.h"

int read_vertex(FILE *f, btg_base *base, unsigned int ver, btg_element *elem) {

	int index = 0;
	btg_vertex *new = NULL, *last = NULL;

	if (base == NULL) {
		fprintf(stderr, "pointer to base is NULL! break.\n");
		return -1;
	}

	for (index = 0 ; index < elem->count ; index++) {
		if ((new = malloc(sizeof(*new))) == NULL) {
			fprintf(stderr, "No memory left for vertex! break.\n");
			return -1;
		}
		new->next = NULL;
		new->projection.x = 0.0;
		new->projection.y = 0.0;
		new->projection.z = 0.0;
		new->valid = 1;
		new->index = index;
		new->count = 0;
		new->alias = NULL;

		base->vertex_array[index] = new;

		if (read_float(f, &new->relative.x)) printf("float Ooops\n");
		if (read_float(f, &new->relative.y)) printf("float Ooops\n");
		if (read_float(f, &new->relative.z)) printf("float Ooops\n");

		new->absolute.x = base->bsphere->coord.x + new->relative.x;
		new->absolute.y = base->bsphere->coord.y + new->relative.y;
		new->absolute.z = base->bsphere->coord.z + new->relative.z;

		if (last) last->next = new;
		else base->vertex = elem->element = new;
		last = new;
	}

	return index;
}

unsigned int count_vertex (btg_vertex *vertex) {

	unsigned int count = 0;

	while (vertex) {
		if (vertex->valid) count++;
		vertex = vertex->next;
	}

	return count;
}

int write_vertex (FILE *f, btg_vertex *vertex, unsigned int ver) {
	while (vertex) {
		if (vertex->valid) {
			if (write_float(f, &vertex->relative.x)) return 1;
			if (write_float(f, &vertex->relative.y)) return 2;
			if (write_float(f, &vertex->relative.z)) return 3;
		}
		vertex = vertex->next;
	}
	return 0;
}

void free_vertex (btg_vertex *vertex) {
	btg_vertex *temp = NULL;
	while (vertex) {
		temp = vertex->next;
		free(vertex);
		vertex = temp;
	}
}



void remove_unused_vertices (btg_vertex *vertex) {

	size_t cnt = 0, error = 0;

	while (vertex) {
		if (vertex->valid) {
			if (vertex->count == 0) {
				vertex->valid = 0;
				error++;
			}
			else vertex->index = cnt++;
		}
		vertex = vertex->next;
	}

	if (error) printf("%zd unused vertices deleted.\n", error);
}

void check_same_vertices (btg_vertex *vertex) {

	int error = 0, cnt = 0;
	btg_vertex *temp;

	if (vertex == NULL) {
		fprintf(stderr, "no vertex in list! break.\n");
		return;
	}

	while (vertex) {
		cnt++;
		if (vertex->valid) {
			temp = vertex->next;
			while (temp && vertex->valid) {
				if (
			    temp->valid &&
			    fabsf(vertex->relative.x - temp->relative.x) < VERTEX_PRECITION &&
			    fabsf(vertex->relative.y - temp->relative.y) < VERTEX_PRECITION &&
			    fabsf(vertex->relative.z - temp->relative.z) < VERTEX_PRECITION
			    ) {
					if (vertex->count > temp->count) {
						temp->alias = vertex;
						vertex->count += temp->count;
						temp->valid = 0;
					}
					else {
						vertex->alias = temp;
						temp->count += vertex->count;
						vertex->valid = 0;
					}
					error++;
				}
				temp = temp->next;
			}
		}
		vertex = vertex->next;
	}

	if (error) printf("%d duplicated vertices removed.\n", error);
}



btg_vertex *new_vertex (btg_vertex *all) {

	btg_vertex *vertex = NULL;

	if (!all) {
		fprintf(stderr, "pointer to vertex is NULL! break.\n");
		return NULL;
	}

	vertex = all;
	while (vertex->next) vertex = vertex->next;

	if ((vertex->next = malloc(sizeof(*vertex))) == NULL) {
		fprintf(stderr, "no memory left for vertex! break.\n");
		return NULL;
	}

	vertex = vertex->next;
	vertex->valid = 1;
	vertex->index = 0;
	vertex->count = 0;
	vertex->absolute.x = vertex->absolute.y = vertex->absolute.z = 0.0;
	vertex->relative.x = vertex->relative.y = vertex->relative.z = 0.0;
	vertex->projection.x = vertex->projection.y = vertex->projection.z = 0.0;
	vertex->alias = NULL;
	vertex->next = NULL;

	return vertex;
}




double pydacoras (const btg_vertex *v0, const btg_vertex *v1, const short view) {

	double delta_x, delta_y, delta_z, len;

	switch (view) {
		case USE_ABSOLUTE:
			delta_x = v0->absolute.x - v1->absolute.x;
			delta_y = v0->absolute.y - v1->absolute.y;
			delta_z = v0->absolute.z - v1->absolute.z;
			len = sqrt((delta_x * delta_x) + (delta_y * delta_y) + (delta_z * delta_z));
			break;

		case USE_RELATIVE:
			delta_x = v0->relative.x - v1->relative.x;
			delta_y = v0->relative.y - v1->relative.y;
			delta_z = v0->relative.z - v1->relative.z;
			len = sqrt((delta_x * delta_x) + (delta_y * delta_y) + (delta_z * delta_z));
			break;

		case USE_PROJECTION:
			delta_x = v0->projection.x - v1->projection.x;
			delta_y = v0->projection.y - v1->projection.y;
			len = sqrt((delta_x * delta_x) + (delta_y * delta_y));
			break;
	}

	return len;
}



vector *vertex2vector (btg_vertex *vertex) {

	vector *vec;

	if ((vec = malloc(sizeof(*vec))) == NULL) {
		fprintf(stderr, "no memory left for vector! break.\n");
		return NULL;
	}

	vec->x = vertex->relative.x;
	vec->y = vertex->relative.y;
	vec->z = vertex->relative.z;

	return vec;
}

void projection (const btg_bsphere *bsphere, btg_vertex *vertex) {

	double len, phi;
	vector vec, def;

	def.x = MAJOR_AXIS;
	def.y = 0.0;
	def.z = 0.0;

	vec.x = vertex->absolute.x;
	vec.y = vertex->absolute.y;
	vec.z = 0.0;

	len = veclen(vec);
	phi = acosl(vecphi(def, vec));
	if (vec.y < 0.0) phi *= -1.0;

	vertex->projection.y = len * cos(phi - bsphere->lon) * -1.0;
	vertex->projection.x = sqrt((len * len) - (vertex->projection.y * vertex->projection.y));
	if (phi < bsphere->lon) vertex->projection.x *= -1.0;
	vertex->projection.z = vertex->absolute.z;

	def.x = 0.0;
	def.y = -MAJOR_AXIS;
	def.z = 0.0;

	vec.x = 0.0;
	vec.y = vertex->projection.y;
	vec.z = vertex->absolute.z;

	len = veclen(vec);
	phi = acosl(vecphi(def, vec));
	if (vec.z < 0.0) phi *= -1.0;

	vertex->projection.z = len * cos(phi - bsphere->lat);
	vertex->projection.y = sqrt((len * len) - (vertex->projection.z * vertex->projection.z));
	if (phi < bsphere->lat) vertex->projection.y *= -1.0;
}

vector get_vector (const btg_vertex *v0, const btg_vertex *v1, short view) {
	vector vec;
	switch (view) {

		case USE_ABSOLUTE:
			vec.x = v1->absolute.x - v0->absolute.x;
			vec.y = v1->absolute.y - v0->absolute.y;
			vec.z = v1->absolute.z - v0->absolute.z;
			break;

		case USE_RELATIVE:
			vec.x = v1->relative.x - v0->relative.x;
			vec.y = v1->relative.y - v0->relative.y;
			vec.z = v1->relative.z - v0->relative.z;
			break;

		case USE_PROJECTION:
			vec.x = v1->projection.x - v0->projection.x;
			vec.y = v1->projection.y - v0->projection.y;
			vec.z = 0.0;
			break;
	}

	return vec;
}



double veclen (const vector vec) {
	return sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

vector vecunique (const vector vec0) {

	double len;
	vector vec;

	len = veclen (vec0);
	vec.x = vec0.x / len;
	vec.y = vec0.y / len;
	vec.z = vec0.z / len;

	return vec;
}

double vecproduct (const vector vec0, const vector vec1) {
	return vec0.x * vec1.x + vec0.y * vec1.y + vec0.z * vec1.z;
}

vector veccross (const vector vec0, const vector vec1) {

	vector cross;

	cross.x = (vec0.y * vec1.z) - (vec0.z * vec1.y);
	cross.y = (vec0.z * vec1.x) - (vec0.x * vec1.z);
	cross.z = (vec0.x * vec1.y) - (vec0.y * vec1.x);

	return cross;
}

double vecphi (const vector vec0, const vector vec1) {

	double angle;

	angle = vecproduct (vec0, vec1) / (veclen(vec0) * veclen(vec1));

	return angle;
}

btg_vertex *intersection (const btg_vertex *v0, const btg_vertex *v1, const btg_vertex *v2, const btg_vertex *v3) {

	btg_vertex *inter = NULL;
	vector vec0, vec1;
	double len0, len1, leninter, lenfull, lenbase, factor;


	if ((inter = malloc(sizeof(*inter))) == NULL) {
		fprintf(stderr, "no memory left for intersection! break.\n");
		return NULL;
	}
	inter->valid = 0;
	inter->index = 0;
	inter->count = 0;
	inter->alias = NULL;
	inter->next = NULL;

	vec0 = get_vector(v0, v1, USE_PROJECTION);
	vec1 = get_vector(v2, v3, USE_PROJECTION);
	lenbase = veclen(vec0);
	len0 = veclen(vec1);
	len1 = len0 * vecphi(vec0, vec1);
	lenfull = sqrt(len0 * len0 - len1 * len1);

	vec1 = get_vector(v0, v3, USE_PROJECTION);
	len0 = veclen(vec1);
	len1 = len0 * vecphi(vec0, vec1);
	leninter = sqrt(len0 * len0 - len1 * len1);

	factor = leninter / lenfull;

	vec0 = get_vector(v3, v2, USE_PROJECTION);
	len0 = veclen(vec0);
	inter->projection.x = v3->relative.x + ((vec0.x / len0) * factor);
	inter->projection.y = v3->relative.y + ((vec0.y / len0) * factor);
	inter->projection.z = v3->relative.z + ((vec0.z / len0) * factor);

	vec0 = get_vector(v0, inter, USE_PROJECTION);
	len0 = veclen(vec0);
	factor = len0 / lenbase;

	inter->relative.x = ((v1->relative.x - v0->relative.x) * factor) + v0->relative.x;
	inter->relative.y = ((v1->relative.y - v0->relative.y) * factor) + v0->relative.y;
	inter->relative.z = ((v1->relative.z - v0->relative.z) * factor) + v0->relative.z;

	return inter;
}



vector area_normal (const btg_vertex *v0, const btg_vertex *v1, const btg_vertex *v2) {

	vector vec0, vec1, vecr;

	vec0 = get_vector(v0, v1, USE_ABSOLUTE);
	vec1 = get_vector(v0, v2, USE_ABSOLUTE);
	vecr = vecunique(veccross(vec0, vec1));

	return vecr;
}

/*
	double len_long, len_short, intersection_x, factor;
	btg_vertex v3;
	const btg_vertex *temp_v;
	vector tria_vec[3], temp_vec;
	normal n0, n1, n2;

	tria_vec[0].vertex[0] = v0;
	tria_vec[0].vertex[1] = v1;
	tria_vec[0].len = pydacoras(v0, v1);

	tria_vec[1].vertex[0] = v1;
	tria_vec[1].vertex[1] = v2;
	tria_vec[1].len = pydacoras(v1, v2);

	tria_vec[2].vertex[0] = v2;
	tria_vec[2].vertex[1] = v0;
	tria_vec[2].len = pydacoras(v2, v0);

// sort vectors by length
	if ((tria_vec[1].len > tria_vec[0].len) && (tria_vec[1].len > tria_vec[2].len)) {
		temp_vec = tria_vec[0];
		tria_vec[0] = tria_vec[1];
		tria_vec[1] = temp_vec;
	}
	if ((tria_vec[2].len > tria_vec[0].len) && (tria_vec[2].len > tria_vec[1].len)) {
		temp_vec = tria_vec[0];
		tria_vec[0] = tria_vec[2];
		tria_vec[2] = temp_vec;
	}
	if (tria_vec[2].len > tria_vec[1].len) {
		temp_vec = tria_vec[1];
		tria_vec[1] = tria_vec[2];
		tria_vec[2] = temp_vec;
	}

// correct order of vertex
	if (tria_vec[0].vertex[0] == tria_vec[1].vertex[0] || tria_vec[0].vertex[0] == tria_vec[1].vertex[1]) {
		temp_v = tria_vec[0].vertex[0];
		tria_vec[0].vertex[0] = tria_vec[0].vertex[1];
		tria_vec[0].vertex[1] = temp_v;
	}
	if (tria_vec[1].vertex[0] == tria_vec[2].vertex[0] || tria_vec[1].vertex[0] == tria_vec[2].vertex[1]) {
		temp_v = tria_vec[1].vertex[0];
		tria_vec[1].vertex[0] = tria_vec[1].vertex[1];
		tria_vec[1].vertex[1] = temp_v;
	}

	// intersection of two circles with different radius
	len_long  = (tria_vec[1].len / tria_vec[0].len) * tria_vec[1].len;
	len_short = (tria_vec[2].len / tria_vec[0].len) * tria_vec[2].len;
	intersection_x = ((tria_vec[0].len - (len_long + len_short)) / 2.0) + len_long;
//	intersection_y = sqrt((tria_vec[1].len * tria_vec[1].len) - (intersection_x * intersection_x));
	factor = intersection_x / tria_vec[0].len;

	v3.x = ((tria_vec[0].vertex[0]->x - tria_vec[0].vertex[1]->x) * factor) + tria_vec[0].vertex[1]->x;
	v3.y = ((tria_vec[0].vertex[0]->y - tria_vec[0].vertex[1]->y) * factor) + tria_vec[0].vertex[1]->y;
	v3.z = ((tria_vec[0].vertex[0]->z - tria_vec[0].vertex[1]->z) * factor) + tria_vec[0].vertex[1]->z;

	n0 = vector2normal (tria_vec[0].vertex[0], tria_vec[0].vertex[1]);
	n1 = vector2normal (&v3, tria_vec[1].vertex[1]);
//	n2 = area_normal(n0, n1);
	n2.x = (n0.y * n1.z) - (n0.z * n1.y);
	n2.y = (n0.z * n1.x) - (n0.x * n1.z);
	n2.z = (n0.x * n1.y) - (n0.y * n1.x);

	return n2;
}
*/
