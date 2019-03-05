#include <stdlib.h>

#include "color.h"
#include "raw.h"

int read_color (FILE *f, btg_base *base, unsigned int ver, btg_element *elem) {

	int index;
	btg_color *new = NULL, *last = NULL;

	if (base == NULL) {
		fprintf(stderr, "pointer to base is NULL! break.\n");
		return -1;
	}

	for (index = 0 ; index < elem->count ; index++) {
		if ((new = malloc(sizeof(*new))) == NULL) {
			fprintf(stderr, "No memory left for color! break.\n");
			return -1;
		}

		new->next = NULL;
		new->valid = 1;
		new->index = index;
		new->count = 0;
		new->alias = NULL;

		base->color_array[index] = new;

		if (read_float(f, &new->r)) printf("float Ooops\n");
		if (read_float(f, &new->g)) printf("float Ooops\n");
		if (read_float(f, &new->b)) printf("float Ooops\n");
		if (read_float(f, &new->a)) printf("float Ooops\n");

		if (last) last->next = new;
		else base->color = elem->element = new;
		last = new;
	}

	return index;
}

unsigned int count_color (btg_color *color) {

	int count = 0;

	while (color) {
		if (color->valid) count++;
		color = color->next;
	}

	return count;
}

int write_color (FILE *f, btg_color *color, unsigned int ver) {
	while (color) {
		if (color->valid) {
			if (write_float(f, &color->r)) return 1;
			if (write_float(f, &color->g)) return 2;
			if (write_float(f, &color->b)) return 3;
			if (write_float(f, &color->a)) return 4;
		}
		color = color->next;
	}
	return 0;
}

void free_color (btg_color *color) {
	btg_color *temp = NULL;
	while (color) {
		temp = color->next;
		free(color);
		color = temp;
	}
}



void remove_unused_colors (btg_color *color) {

	size_t cnt = 0, error = 0;

	while (color) {
		if (color->valid) {
			if (color->count == 0) {
				color->valid = 0;
				error++;
			}
			else color->index = cnt++;
		}
		color = color->next;
	}

	if (error) printf("%zd unused colors deleted.\n", error);
}

void check_same_colors (btg_color *color) {

	int error = 0;
	btg_color *temp;

	while (color) {
		if (color->valid) {
			temp = color->next;
			while (temp && color->valid) {
				if (
			    temp->valid &&
			    color->r == temp->r &&
			    color->g == temp->g &&
			    color->b == temp->b &&
			    color->a == temp->a
			    ) {
					if (color->count > temp->count) {
						temp->alias = color;
						color->count += temp->count;
						temp->valid = 0;
					}
					else {
						color->alias = temp;
						temp->count += color->count;
						color->valid = 0;
					}
					error++;
				}
				temp = temp->next;
			}
		}
		color = color->next;
	}

	if (error) printf("%d duplicated colors removed.\n", error);
}
