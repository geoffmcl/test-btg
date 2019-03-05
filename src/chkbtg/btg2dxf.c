/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2017 Sascha Rei??ner <reiszner@novaplan.at>
 * 
 * btg2dxf is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * btg2dxf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "raw.h"
#include "btgio.h"



int main(int argc, char *argv[])
{

	FILE *file = NULL;
	char name[256], file_name[256], *p;
	btg_header *header = NULL;
	btg_object *obj;
	btg_element *elem;
	btg_geometry *geo;

	if (argc < 2) {
		fprintf(stderr, "No file provited! exit.\n");
		return 1;
	}

	strcpy(name, argv[1]);
	if ((p = strpbrk (name, ".")) == NULL) {
		fprintf(stderr, "Can not find a period in name '%s'! exit.\n", name);
		return 1;
	}
	*p = '\0';
	sprintf(file_name, "%.251s.geo", name);

	if ((file = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "File '%s' doesn't exist! exit.\n", argv[1]);
		return 2;
	}

	if (read_btg (file, &header)) {
		fprintf(stderr, "Problem while reading btg file! exit.\n");
		return 3;
	}
	fclose (file);

	if ((file = fopen(file_name, "w")) == NULL) {
		fprintf(stderr, "Can't open file '%s'! exit.\n", file_name);
		return 2;
	}


	obj = header->object;
	while (obj) {

		switch (obj->obj_type) {

			case 0:
				printf("Bounding Sphere\n");
				btg_bsphere *bs = header->object->elem_list->element;
				fprintf(file, "LAYER\n%s_bsphere\n", name);
				fprintf(file, "BSPHERE\n");
				fprintf(file, "%.18f,%.18f,%.18f,%.18f\n", bs->coord.x, bs->coord.y, bs->coord.z, bs->r);
				break;

			case 9:
			case 10:
			case 11:
			case 12:
				if      (obj->obj_type == 9 ) printf("Points\n");
				else if (obj->obj_type == 10) printf("Individual Triangles\n");
				else if (obj->obj_type == 11) printf("Triangle Strips\n");
				else if (obj->obj_type == 12) printf("Triangle Fans\n");

				fprintf(file, "LAYER\n");
				if (obj->prop_material)
					fprintf(file, "%s_%s\n", name, obj->prop_material);
				else
					fprintf(file, "%s_0\n", name);

				if      (obj->obj_type == 9 ) fprintf(file, "POINTS\n");
				else if (obj->obj_type == 10) fprintf(file, "TRIANGLES\n");
				else if (obj->obj_type == 11) fprintf(file, "STRIPS\n");
				else if (obj->obj_type == 12) fprintf(file, "FANS\n");

				elem = obj->elem_list;
				while (elem) {
					geo = elem->element;
					while (geo) {
						fprintf(file, "%.18f,%.18f,%.18f\n", geo->vertex->relative.x, geo->vertex->relative.y, geo->vertex->relative.z);
						//fprintf(file, "%.18f,%.18f,%.18f\n", geo->vertex->projection.x, geo->vertex->projection.y, geo->vertex->projection.z);
						geo = geo->next;
					}
					elem = elem->next;
				}
				break;

		}
		obj = obj->next;
	}

	fclose (file);
	free(header->object);

	return (0);
}
