#include <stdlib.h>

#include "geometry.h"
#include "raw.h"

btg_geometry *read_geometry (FILE *f, btg_base *base, unsigned int ver, unsigned char mask) {

	unsigned short s_temp;
	unsigned int   i_temp;
	btg_geometry *geo = NULL;

	if ((geo = malloc(sizeof(*geo))) == NULL) {
		fprintf(stderr, "No memory left for geometry! break.\n");
		return NULL;
	}
	geo->valid = 1;
	geo->vertex = NULL;
	geo->normal = NULL;
	geo->texcoo = NULL;
	geo->color = NULL;
	geo->next = NULL;

	if (mask & MASK_VERTEX) {
		if (ver == 7) {
			if (read_ushort(f, &s_temp)) printf("short Ooops\n");
			geo->vertex = base->vertex_array[s_temp];
		}
		else if (ver == 10) {
			if (read_uint(f, &i_temp)) printf("int Ooops\n");
			geo->vertex = base->vertex_array[i_temp];
		}
	}

	if (mask & MASK_NORMAL) {
		if (ver == 7) {
			if (read_ushort(f, &s_temp)) printf("short Ooops\n");
			geo->normal = base->normal_array[s_temp];
		}
		else if (ver == 10) {
			if (read_uint(f, &i_temp)) printf("int Ooops\n");
			geo->normal = base->normal_array[i_temp];
		}
	}

	if (mask & MASK_COLOR) {
		if (ver == 7) {
			if (read_ushort(f, &s_temp)) printf("short Ooops\n");
			geo->color = base->color_array[s_temp];
		}
		else if (ver == 10) {
			if (read_uint(f, &i_temp)) printf("int Ooops\n");
			geo->color = base->color_array[i_temp];
		}
	}

	if (mask & MASK_TEXCOO) {
		if (ver == 7) {
			if (read_ushort(f, &s_temp)) printf("short Ooops\n");
			geo->texcoo = base->texcoo_array[s_temp];
		}
		else if (ver == 10) {
			if (read_uint(f, &i_temp)) printf("int Ooops\n");
			geo->texcoo = base->texcoo_array[i_temp];
		}
	}

	return geo;
}

unsigned int count_geometry (btg_geometry *geometry) {

	unsigned int count = 0;

	while (geometry) {
		if (geometry->valid) count++;
		geometry = geometry->next;
	}

	return count;
}

int write_geometry (FILE *f, btg_geometry *geo, unsigned int ver, unsigned char mask) {

	unsigned short temp;

	while (geo) {
		if (geo->valid) {

			if (mask & MASK_VERTEX) {
				if (ver == 7) {
					temp = geo->vertex->index;
					if (write_ushort(f, &temp)) return 1;
				}
				else if (ver == 10) {
					if (write_uint(f, &geo->vertex->index)) return 1;
				}
			}


			if (mask & MASK_NORMAL) {
				if (ver == 7) {
					temp = geo->normal->index;
					if (write_ushort(f, &temp)) return 2;
				}
				else if (ver == 10) {
					if (write_uint(f, &geo->normal->index)) return 2;
				}
			}

			if (mask & MASK_COLOR) {
				if (ver == 7) {
					temp = geo->color->index;
					if (write_ushort(f, &temp)) return 3;
				}
				else if (ver == 10) {
					if (write_uint(f, &geo->color->index)) return 3;
				}
			}

			if (mask & MASK_TEXCOO) {
				if (ver == 7) {
					temp = geo->texcoo->index;
					if (write_ushort(f, &temp)) return 4;
				}
				else if (ver == 10) {
					if (write_uint(f, &geo->texcoo->index)) return 4;
				}
			}

		}
		geo = geo->next;
	}

	return 0;
}

void free_geometry (btg_geometry *geo) {
	btg_geometry *temp = NULL;
	while (geo) {
		temp = geo->next;
		free(geo);
		geo = temp;
	}
}

void rec_geometry (btg_geometry *geo) {
	if (geo->vertex) geo->vertex->count++;
	if (geo->normal) geo->normal->count++;
	if (geo->color) geo->color->count++;
	if (geo->texcoo) geo->texcoo->count++;
	geo->valid = 1;
}

void unrec_geometry (btg_geometry *geo) {
	if (geo->vertex) geo->vertex->count--;
	if (geo->normal) geo->normal->count--;
	if (geo->color) geo->color->count--;
	if (geo->texcoo) geo->texcoo->count--;
	geo->valid = 0;
}

void unalias_geometry (btg_geometry *geo) {

	int use;

	if (geo->vertex) {
		if (geo->vertex->alias) {
			use = geo->vertex->count;
			geo->vertex->valid = 0;
			geo->vertex->count = 0;
			while (geo->vertex->alias) geo->vertex = geo->vertex->alias;
			geo->vertex->count += use;
		}
	}
	if (geo->normal) {
		if (geo->normal->alias) {
			use = geo->normal->count;
			geo->normal->valid = 0;
			geo->normal->count = 0;
			while (geo->normal->alias) geo->normal = geo->normal->alias;
			geo->normal->count += use;
		}
	}
	if (geo->color) {
		if (geo->color->alias) {
			use = geo->color->count;
			geo->color->valid = 0;
			geo->color->count = 0;
			while (geo->color->alias) geo->color = geo->color->alias;
			geo->color->count += use;
		}
	}
	if (geo->texcoo) {
		if (geo->texcoo->alias) {
			use = geo->texcoo->count;
			geo->texcoo->valid = 0;
			geo->texcoo->count = 0;
			while (geo->texcoo->alias) geo->texcoo = geo->texcoo->alias;
			geo->texcoo->count += use;
		}
	}
}



btg_geometry *new_geometry (btg_vertex *vertex, btg_normal *normal, btg_texcoo *texcoo, btg_color *color) {

	btg_geometry *geo = NULL;

	if ((geo = malloc(sizeof(*geo))) == NULL) {
		fprintf(stderr, "No memory left for geometry! break.\n");
		return NULL;
	}
	geo->valid = 0;
	geo->vertex = vertex;
	geo->normal = normal;
	geo->texcoo = texcoo;
	geo->color = color;
	geo->next = NULL;

	return geo;
}
