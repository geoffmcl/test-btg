#include <stdlib.h>
#include <string.h>

#include "property.h"
#include "raw.h"

int read_property (FILE *f, btg_object *obj, unsigned int ver) {

	unsigned char type;
	unsigned int size;

	if (obj == NULL) {
		fprintf(stderr, "pointer to object is NULL! break.\n");
		return 1;
	}

	if (read_uchar(f, &type)) {
		fprintf(stderr, "problem while reading property type! break.\n");
		return 2;
	}

	if (read_uint(f, &size)) {
		fprintf(stderr, "problem while reading property size! break.\n");
		return 3;
	}

	switch (type) {

		case PROP_MAT:
			if ((obj->prop_material = calloc(size + 1, 1)) == NULL) {
				fprintf(stderr, "no memory left for prop-material! break.\n");
				return 4;
			}
			if (fread(obj->prop_material, 1, size, f) != size) {
				fprintf(stderr, "problem while reading prop-material! break.\n");
				free(obj->prop_material);
				return 5;
			}
			printf("Prop Material: '%s'\n", obj->prop_material);
			break;

		case PROP_INDEX:
			if (size != 1) {
				fprintf(stderr, "property size isn't 1 (%d)! break.\n", size);
				return 6;
			}
			if (read_uchar(f, &obj->prop_mask)) {
				fprintf(stderr, "problem while reading property index! break.\n");
				return 7;
			}
			printf("Prop Index   : %02x\n", obj->prop_mask);
			break;

		default:
			fprintf(stderr, "unknown property type! break.\n");
			return 8;
			break;
	}

	return 0;
}

int write_property (FILE *f, btg_object *obj, unsigned int ver) {

	unsigned char type;
	unsigned int size;

	if (obj == NULL) {
		fprintf(stderr, "pointer to object is NULL! break.\n");
		return 1;
	}

	if (obj->prop_material) {
		type = PROP_MAT;
		size = strlen(obj->prop_material);
		if (write_uchar(f, &type)) {
			fprintf(stderr, "problem while writing property type! break.\n");
			return 2;
		}
		if (write_uint(f, &size)) {
			fprintf(stderr, "problem while writing property size! break.\n");
			return 3;
		}
		if (fwrite(obj->prop_material, 1, size, f) != size) {
			fprintf(stderr, "problem while writing property material! break.\n");
			return 4;
		}
	}

	if (obj->prop_mask) {
		type = PROP_INDEX;
		size = 1;
		if (write_uchar(f, &type)) {
			fprintf(stderr, "problem while writing property type! break.\n");
			return 2;
		}
		if (write_uint(f, &size)) {
			fprintf(stderr, "problem while writing property size! break.\n");
			return 3;
		}
		if (write_uchar(f, &obj->prop_mask)) {
			fprintf(stderr, "problem while writing property index! break.\n");
			return 4;
		}
	}

	return 0;
}
