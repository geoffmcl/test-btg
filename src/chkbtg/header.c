#include <stdlib.h>

#include "raw.h"
#include "header.h"
#include "object.h"

int read_header (FILE *f, btg_header **head) {

	unsigned int cr;
	unsigned short cnt;
	btg_header *new = NULL;

	if (head == NULL) {
		fprintf(stderr, "double pointer to header is NULL! exit.\n");
		return 1;
	}

	if (*head != NULL) {
		fprintf(stderr, "pointer to header isn't NULL! exit.\n");
		return 2;
	}

	if ((new = malloc(sizeof(*new))) == NULL) {
		fprintf(stderr, "No memory left for header! exit.\n");
		return 3;
	}
	*head = new;

	new->base.min_x = 0.0;
	new->base.max_x = 0.0;
	new->base.min_y = 0.0;
	new->base.max_y = 0.0;
	new->base.holesize = 0.0;
	new->base.material = NULL;
	new->base.bsphere = NULL;
	new->base.vertex = NULL;
	new->base.vertex_array = NULL;
	new->base.normal = NULL;
	new->base.normal_array = NULL;
	new->base.texcoo = NULL;
	new->base.texcoo_array = NULL;
	new->base.color = NULL;
	new->base.color_array = NULL;
	new->base.point = NULL;
	new->base.point_last = NULL;
	new->base.triangle = NULL;
	new->base.triangle_last = NULL;
	new->base.edge = NULL;
	new->base.edge_last = NULL;
	new->base.fence = NULL;
	new->base.fence_last = NULL;
	new->object = NULL;
	new->runway = NULL;

	printf("read Header ...\n");

	read_ushort(f, &new->version);
	if (new->version != 7 && new->version != 10) {
		fprintf(stderr, "unknown version (%d)! exit.\n", new->version);
		return 4;
	}
	printf("Version: %hd\n", new->version);

	read_ushort(f, &new->mag_num);
	if (new->mag_num != 0x5347) {
		fprintf(stderr, "Magic Number isn't 'SG'! exit.\n");
		return 5;
	}
	printf("Magic Number: %c%c\n", (new->mag_num >> 8), (new->mag_num & 0x00ff));

	read_uint(f, &cr);
	new->creation = cr;
	printf("Creation Time: %s", ctime(&new->creation));

	if (new->version == 7) {
		read_ushort(f, &cnt);
		new->num_object = cnt;
	}
	else if (new->version == 10) {
		read_uint(f, &new->num_object);
	}
	else {
		fprintf(stderr, "Unknown Version %hd! exit.\n", new->version);
		return 6;
	}

	if (new->num_object == 0) {
		fprintf(stderr, "Toplevel Objects is 0! exit.\n");
		return 7;
	}

	return 0;
}



int write_header (FILE *f, btg_header *head) {

	unsigned int cr;
	unsigned short cnt = 0;

	if (head == NULL) {
		fprintf(stderr, "pointer to header is NULL! exit.\n");
		return 1;
	}

	head->num_object = count_object(head->object);
	printf("write Header ...\n");
	printf("header has %d objects ...\n", head->num_object);

	if (write_ushort(f, &head->version)) return 1;
	if (write_ushort(f, &head->mag_num)) return 2;
	cr = head->creation;
	if (write_uint(f, &cr)) return 3;
	if (head->version == 7) {
		cnt = head->num_object;
		if (write_ushort(f, &cnt)) return 4;
	}
	else if (head->version == 10) {
		if (write_uint(f, &head->num_object)) return 4;
	}
	else {
		fprintf(stderr, "Unknown Version %hd! exit.\n", head->version);
		return 5;
	}

	return 0;
}

void free_header (btg_header *head) {
	if (head) {
		if (head->object)        free_object (head->object);
		if (head->base.vertex)   free (head->base.vertex);
		if (head->base.normal)   free (head->base.normal);
		if (head->base.color )   free (head->base.color);
		if (head->base.texcoo)   free (head->base.texcoo);
		if (head->base.point)    free (head->base.point);
		if (head->base.triangle) free (head->base.triangle);
		if (head->base.edge)     free (head->base.edge);
		free(head);
	}
}
