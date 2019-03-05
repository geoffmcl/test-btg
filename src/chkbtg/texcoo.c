#include <stdlib.h>
#include <math.h>

#include "texcoo.h"
#include "raw.h"

int read_texcoo (FILE *f, btg_base *base, unsigned int ver, btg_element *elem) {

	int index;
	btg_texcoo *new = NULL, *last = NULL;

	if (base == NULL) {
		fprintf(stderr, "pointer to base is NULL! break.\n");
		return -1;
	}

	for (index = 0 ; index < elem->count ; index++) {
		if ((new = malloc(sizeof(*new))) == NULL) {
			fprintf(stderr, "No memory left for texcoo! break.\n");
			return -1;
		}
		new->next = NULL;
		new->valid = 1;
		new->index = index;
		new->count = 0;
		new->alias = NULL;

		base->texcoo_array[index] = new;

		if (read_float(f, &new->u)) printf("float Ooops\n");
		if (read_float(f, &new->v)) printf("float Ooops\n");

		if (last) last->next = new;
		else base->texcoo = elem->element = new;
		last = new;
	}
	return index;
}

unsigned int count_texcoo (btg_texcoo *texcoo) {

	int count = 0;

	while (texcoo) {
		if (texcoo->valid) count++;
		texcoo = texcoo->next;
	}

	return count;
}

int write_texcoo (FILE *f, btg_texcoo *texcoo, unsigned int ver) {
	while (texcoo) {
		if (texcoo->valid) {
			if (write_float(f, &texcoo->u)) return 1;
			if (write_float(f, &texcoo->v)) return 2;
		}
		texcoo = texcoo->next;
	}
	return 0;
}

void free_texcoo (btg_texcoo *texcoo) {
	btg_texcoo *temp = NULL;
	while (texcoo) {
		temp = texcoo->next;
		free(texcoo);
		texcoo = temp;
	}
}



void remove_unused_texcoos (btg_texcoo *texcoo) {

	size_t cnt = 0, error = 0;

	while (texcoo) {
		if (texcoo->valid) {
			if (texcoo->count == 0) {
				texcoo->valid = 0;
				error++;
			}
			else texcoo->index = cnt++;
		}
		texcoo = texcoo->next;
	}

	if (error) printf("%zd unused texture coordinates deleted.\n", error);
}

void check_same_texcoos (btg_texcoo *texcoo) {

	int error = 0;
	btg_texcoo *temp;

	while (texcoo) {
		if (texcoo->valid) {
			temp = texcoo->next;
			while (temp && texcoo->valid) {
				if (
			    temp->valid &&
			    fabsf(texcoo->u - temp->u) < TEXCOO_PRECITION &&
			    fabsf(texcoo->v - temp->v) < TEXCOO_PRECITION
			    ) {
					if (texcoo->count > temp->count) {
						temp->alias = texcoo;
						texcoo->count += temp->count;
						temp->valid = 0;
					}
					else {
						texcoo->alias = temp;
						temp->count += texcoo->count; 
						texcoo->valid = 0;
					}
					error++;
				}
				temp = temp->next;
			}
		}
		texcoo = texcoo->next;
	}

	if (error) printf("%d duplicated texture coordinates removed.\n", error);
}
