#include <stdlib.h>

#include "bsphere.h"
#include "coord.h"
#include "raw.h"

int read_bsphere (FILE *f, btg_base *base, unsigned int ver, btg_element *elem) {

	int index = 0;
	btg_bsphere *new = NULL, *last = NULL;
	coord_geo geo;
	coord_cart cart;

	if (base == NULL) {
		fprintf(stderr, "pointer to base is NULL! break.\n");
		return -1;
	}

	for (index = 0 ; index < elem->count ; index++) {
		if ((new = malloc(sizeof(*new))) == NULL) {
			fprintf(stderr, "No memory left for bounding sphere! break.\n");
			return -1;
		}
		new->next = NULL;
		new->valid = 1;
		new->index = index;
		new->count = 1;

		if (read_double(f, &new->coord.x)) printf("double Ooops\n");
		if (read_double(f, &new->coord.y)) printf("double Ooops\n");
		if (read_double(f, &new->coord.z)) printf("double Ooops\n");
		if (read_float (f, &new->r))       printf("float Ooops\n");

		cart.x = new->coord.x;
		cart.y = new->coord.y;
		cart.z = new->coord.z;
		geo = cart2geo (cart);
		new->lat = geo.lat;
		new->lon = geo.lon;

		if (last) last->next = new;
		else base->bsphere = elem->element = new;
		last = new;
	}
	return index;
}

unsigned int count_bsphere (btg_bsphere *bsphere) {

	int count = 0;

	while (bsphere) {
		if (bsphere->valid) count++;
		bsphere = bsphere->next;
	}

	return count;
}

int write_bsphere (FILE *f, btg_bsphere *bsphere, unsigned int ver) {
	while (bsphere) {
		if (bsphere->valid) {
			printf("write bsphere ...\n");
			if (write_double(f, &bsphere->coord.x)) return 1;
			if (write_double(f, &bsphere->coord.y)) return 2;
			if (write_double(f, &bsphere->coord.z)) return 3;
			if (write_float (f, &bsphere->r)) return 4;
		}
		bsphere = bsphere->next;
	}
	return 0;
}

void free_bsphere (btg_bsphere *bsphere) {
	btg_bsphere *temp = NULL;
	while (bsphere) {
		temp = bsphere->next;
		free(bsphere);
		bsphere = temp;
	}
}



void remove_unused_bspheres (btg_bsphere *bsphere) {

	size_t cnt = 0, error = 0;

	while (bsphere) {
		if (bsphere->valid) {
			if (bsphere->count == 0) {
				bsphere->valid = 0;
				error++;
			}
			else bsphere->index = cnt++;
		}
		bsphere = bsphere->next;
	}

	if (error) printf("%zd unused bounding spheres deleted.\n", error);
}

void check_same_bspheres (btg_bsphere *bsphere) {

	int error = 0;
	btg_bsphere *temp;

	while (bsphere) {
		if (bsphere->valid) {
			temp = bsphere->next;
			while (temp && bsphere->valid) {
				if (
			    temp->valid &&
			    bsphere->coord.x == temp->coord.x &&
			    bsphere->coord.y == temp->coord.y &&
			    bsphere->coord.z == temp->coord.z &&
			    bsphere->r == temp->r
			    ) {
					if (bsphere->count > temp->count) {
						temp->valid = 0;
					}
					else {
						bsphere->valid = 0;
					}
					error++;
				}
				temp = temp->next;
			}
		}
		bsphere = bsphere->next;
	}

	if (error) printf("%d duplicated bounding spheres removed.\n", error);
}
