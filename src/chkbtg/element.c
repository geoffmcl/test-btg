#include <stdlib.h>

#include "raw.h"
#include "element.h"
#include "bsphere.h"
#include "vertex.h"
#include "normal.h"
#include "color.h"
#include "texcoo.h"
#include "point.h"
#include "triangle.h"
#include "geometry.h"

btg_element *read_element (FILE *f, btg_base *base, unsigned int ver, unsigned char type, unsigned char mask, char *material) {

	int i, num, res;
	btg_element  *elem    = NULL;
	btg_geometry *geo     = NULL, *last_g = NULL;

	if ((elem = malloc(sizeof(*elem))) == NULL) {
		fprintf(stderr, "No memory left for element! break.\n");
		return NULL;
	}
	elem->next = NULL;
	elem->element = NULL;
	elem->valid = 1;
	elem->count = 0;

	if (read_uint(f, &elem->num_bytes)) {
		printf("problem while reading byte counter! break.\n");
		free_element(elem, type);
		return NULL;
	}

	num = 0;
	for (i = 0 ; i < 4 ; i++) {
		if (mask & (1 << i)) num++;
	}

	switch (type) {

		case OBJ_BS:
			printf("read BS ... (%d)\n", elem->num_bytes);
			if (elem->num_bytes % 28) {
				fprintf(stderr, "byte counter in bounding sphere isn't correct! break.\n");
				free_element(elem, type);
				return NULL;
			}
			elem->count = elem->num_bytes / 28;
			printf("read BS ...\n");
			res = read_bsphere (f, base, ver, elem);
			printf("%d bounding sphere ...\n", res);
			break;

		case OBJ_VERTEX:
			if (elem->num_bytes % 12) {
				fprintf(stderr, "byte counter in vertex isn't correct! break.\n");
				free_element(elem, type);
				return NULL;
			}
			elem->count = elem->num_bytes / 12;
			if ((base->vertex_array = calloc(sizeof(*base->vertex_array), elem->count)) == NULL) {
				fprintf(stderr, "No memory left for vertex array! break.\n");
				free_element(elem, type);
				return NULL;
			}
			res = read_vertex(f, base, ver, elem);
			printf("%d vertex ...\n", res);
			break;

		case OBJ_NORMAL:
			if (elem->num_bytes % 3) {
				fprintf(stderr, "byte counter in normal isn't correct! break.\n");
				free_element(elem, type);
				return NULL;
			}
			elem->count = elem->num_bytes / 3;
			if ((base->normal_array = calloc(sizeof(*base->normal_array), elem->count)) == NULL) {
				fprintf(stderr, "No memory left for normal array! break.\n");
				free_element(elem, type);
				return NULL;
			}

			res = read_normal(f, base, ver, elem);
			printf("%d normal ...\n", res);
			break;

		case OBJ_COLOR:
			if (elem->num_bytes % 16) {
				fprintf(stderr, "byte counter in color isn't correct! exit.\n");
				free_element(elem, type);
				return NULL;
			}
			elem->count = elem->num_bytes / 16;
			if ((base->color_array = calloc(sizeof(*base->color_array), elem->count)) == NULL) {
				fprintf(stderr, "No memory left for color array! break.\n");
				free_element(elem, type);
				return NULL;
			}
			res = read_color(f, base, ver, elem);
			printf("%d color ...\n", res);
			break;

		case OBJ_TEXCOO:
			if (elem->num_bytes % 8) {
				fprintf(stderr, "byte counter in texture coordinate isn't correct! exit.\n");
				free_element(elem, type);
				return NULL;
			}
			elem->count = elem->num_bytes / 8;
			if ((base->texcoo_array = calloc(sizeof(*base->texcoo_array), elem->count)) == NULL) {
				fprintf(stderr, "No memory left for texcoo array! break.\n");
				free_element(elem, type);
				return NULL;
			}
			res = read_texcoo(f, base, ver, elem);
			printf("%d texture coordinate ...\n", res);
			break;

		case OBJ_POINTS:
		case OBJ_TRIS:
		case OBJ_STRIPE:
		case OBJ_FAN:
			if (ver == 7) {
				if (elem->num_bytes % (2 * num)) {
					fprintf(stderr, "byte counter in geomentry isn't correct! exit.\n");
					free_element(elem, type);
					return NULL;
				}
				elem->count = elem->num_bytes / (2 * num);
			}
			if (ver == 10) {
				if (elem->num_bytes % (4 * num)) {
					fprintf(stderr, "byte counter in geomentry isn't correct! exit.\n");
					free_element(elem, type);
					return NULL;
				}
				elem->count = elem->num_bytes / (4 * num);
			}
			for (i = 0 ; i < elem->count ; i++) {
				if ((geo = read_geometry(f, base, ver, mask)) == NULL) {
					free_element(elem, type);
					return NULL;
				}
				if (last_g) {
					last_g->next = geo;
					last_g = last_g->next;
				}
				else {
					elem->element = last_g = geo;
				}
			}
			break;

			default:
				fprintf(stderr, "unknown object type %d\n", type);
				break;
	}
	return elem;
}

unsigned int count_element (btg_element *elem, unsigned char type) {

	unsigned int count = 0;

	while (elem) {
		if (elem->valid) {
			elem->count = 0;
			switch (type) {
				case OBJ_BS:
					if ((elem->count = count_bsphere (elem->element))) count++;
					break;
				case OBJ_VERTEX:
					if ((elem->count = count_vertex (elem->element))) count++;
					break;
				case OBJ_NORMAL:
					if ((elem->count = count_normal (elem->element))) count++;
					break;
				case OBJ_COLOR:
					if ((elem->count = count_color (elem->element))) count++;
					break;
				case OBJ_TEXCOO:
					if ((elem->count = count_texcoo (elem->element))) count++;
					break;
				case OBJ_POINTS:
				case OBJ_TRIS:
				case OBJ_STRIPE:
				case OBJ_FAN:
					if ((elem->count = count_geometry (elem->element))) count++;
					break;
			}
		}
		elem = elem->next;
	}

	return count;
}

int write_element (FILE *f, btg_element *elem, unsigned int ver, unsigned char type, unsigned char mask) {

	int i = 0, num;

	while (elem) {
		if (elem->valid) {
			switch (type) {
				case OBJ_BS:
//					printf("bsphere num: %d\n", elem->count);
					if (elem->count) {
						elem->num_bytes = elem->count * 28;
						if (write_uint(f, &elem->num_bytes)) return 1;
						if (write_bsphere (f, elem->element, ver)) return 2;
					}
					break;

				case OBJ_VERTEX:
//					printf("vertex num: %d\n", elem->count);
					if (elem->count) {
						elem->num_bytes = elem->count * 12;
						if (write_uint(f, &elem->num_bytes)) return 1;
						if (write_vertex(f, elem->element, ver)) return 2;
					}
					break;

				case OBJ_NORMAL:
//					printf("normal num: %d\n", elem->count);
					if (elem->count) {
						elem->num_bytes = elem->count * 3;
						if (write_uint(f, &elem->num_bytes)) return 1;
						if (write_normal(f, elem->element, ver)) return 2;
					}
					break;

				case OBJ_COLOR:
//					printf("color num: %d\n", elem->count);
					if (elem->count) {
						elem->num_bytes = elem->count * 16;
						if (write_uint(f, &elem->num_bytes)) return 1;
						if (write_color(f, elem->element, ver)) return 2;
					}
					break;

				case OBJ_TEXCOO:
//					printf("texcoo num: %d\n", elem->count);
					if (elem->count) {
						elem->num_bytes = elem->count * 8;
						if (write_uint(f, &elem->num_bytes)) return 1;
						if (write_texcoo(f, elem->element, ver)) return 2;
					}
					break;

				case OBJ_POINTS:
				case OBJ_TRIS:
				case OBJ_STRIPE:
				case OBJ_FAN:
					num = 0;
					for (i = 0 ; i < 4 ; i++) {
						if (mask & (1 << i)) num++;
					}
//					printf("geometry num: %d\n", elem->count);
					if (elem->count) {
						if (ver == 7 ) elem->num_bytes = elem->count * (2 * num);
						if (ver == 10) elem->num_bytes = elem->count * (4 * num);
						if (write_uint(f, &elem->num_bytes)) return 1;
						if (write_geometry(f, elem->element, ver, mask)) return 2;
					}
					break;

				default:
					fprintf(stderr, "unknown object type %d\n", type);
					break;
			}
		}
		elem = elem->next;
	}

	return 0;
}

void free_element(btg_element *elem, unsigned char type) {
	btg_element *temp = NULL;
	while (elem) {
		temp = elem->next;
		if (elem->element) {
			switch (type) {

				case OBJ_BS:
					free_bsphere(elem->element);
					break;

				case OBJ_VERTEX:
					free_vertex(elem->element);
					break;

				case OBJ_NORMAL:
					free_normal(elem->element);
					break;

				case OBJ_COLOR:
					free_color(elem->element);
					break;

				case OBJ_TEXCOO:
					free_texcoo(elem->element);
					break;

// TODO: free points and triangles

			}
		}
		free(elem);
		elem = temp;
	}
}



btg_element *new_element (void) {

	btg_element *elem = NULL;

	if ((elem = malloc(sizeof(*elem))) == NULL) {
		fprintf(stderr, "No memory left for element! break.\n");
		return NULL;
	}

	elem->valid = 1;
	elem->num_bytes = 0;
	elem->count = 0;
	elem->element = NULL;
	elem->next = NULL;

	return elem;
}
