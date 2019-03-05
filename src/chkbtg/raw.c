#include <string.h>

#include "raw.h"

int read_uchar (FILE *f, unsigned char *uc) {

	if (fread(uc, 1, 1, f) != 1) {
		fprintf(stderr, "problem while reading! exit.\n");
		return 1;
	}

	return 0;
}

int write_uchar (FILE *f, unsigned char *uc) {

	if (fwrite(uc, 1, 1, f) != 1) {
		fprintf(stderr, "problem while writing! exit.\n");
		return 1;
	}

	return 0;
}

int read_ushort (FILE *f, unsigned short *us) {

	char buffer[2];

	if (fread(buffer, 2, 1, f) != 1) {
		fprintf(stderr, "problem while reading! exit.\n");
		return 1;
	}

	memcpy(us, buffer, 2);
	return 0;
}

int write_ushort (FILE *f, unsigned short *us) {

	char buffer[2];
	memcpy(buffer, us, 2);

	if (fwrite(buffer, 2, 1, f) != 1) {
		fprintf(stderr, "problem while writing! exit.\n");
		return 1;
	}

	return 0;
}

int read_uint(FILE *f, unsigned int *ui) {

	char buffer[4];

	if (fread(buffer, 4, 1, f) != 1) {
		fprintf(stderr, "problem while reading! exit.\n");
		return 1;
	}

	memcpy(ui, buffer, 4);
	return 0;
}

int write_uint(FILE *f, unsigned int *ui) {

	char buffer[4];
	memcpy(buffer, ui, 4);

	if (fwrite(buffer, 4, 1, f) != 1) {
		fprintf(stderr, "problem while writing! exit.\n");
		return 1;
	}

	return 0;
}

int read_ulong(FILE *f, unsigned long *ul) {

	char buffer[8];

	if (fread(buffer, 8, 1, f) != 1) {
		fprintf(stderr, "problem while reading! exit.\n");
		return 1;
	}

	memcpy(ul, buffer, 8);
	return 0;
}

int write_ulong(FILE *f, unsigned long *ul) {

	char buffer[8];
	memcpy(buffer, ul, 8);

	if (fwrite(buffer, 8, 1, f) != 1) {
		fprintf(stderr, "problem while writing! exit.\n");
		return 1;
	}

	return 0;
}

int read_float(FILE *f, float *fl) {

	char buffer[4];

	if (fread(buffer, 4, 1, f) != 1) {
		fprintf(stderr, "problem while reading! exit.\n");
		return 1;
	}

	memcpy(fl, buffer, 4);
	return 0;
}

int write_float(FILE *f, float *fl) {

	char buffer[4];
	memcpy(buffer, fl, 4);

	if (fwrite(buffer, 4, 1, f) != 1) {
		fprintf(stderr, "problem while writing! exit.\n");
		return 1;
	}

	return 0;
}

int read_double(FILE *f, double *db) {

	char buffer[8];

	if (fread(buffer, 8, 1, f) != 1) {
		fprintf(stderr, "problem while reading! exit.\n");
		return 1;
	}

	memcpy(db, buffer, 8);
	return 0;
}

int write_double(FILE *f, double *db) {

	char buffer[8];
	memcpy(buffer, db, 8);

	if (fwrite(buffer, 8, 1, f) != 1) {
		fprintf(stderr, "problem while writing! exit.\n");
		return 1;
	}

	return 0;
}
