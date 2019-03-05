#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raw.h"
#include "object.h"
#include "property.h"
#include "element.h"

btg_object *read_object (FILE *f, btg_base *base, unsigned int ver) {

	int i;
	unsigned int prop_cnt;
	unsigned short x;
	btg_object *obj = NULL;
	btg_element *elem = NULL, *last = NULL;

	if ((obj = malloc(sizeof(*obj))) == NULL) {
		fprintf(stderr, "No memory left for object! break.\n");
		return NULL;
	}
	obj->elem_cnt = 0;
	obj->prop_mask = 0;
	obj->prop_material = NULL;
	obj->elem_list = NULL;
	obj->next = NULL;

	if (read_uchar(f, &obj->obj_type)) printf("char Ooops\n");
	if (ver == 7) {
		if (read_ushort(f, &x)) printf("short Ooops\n");
		prop_cnt = x;
		if (read_ushort(f, &x)) printf("short Ooops\n");
		obj->elem_cnt = x;
	}
	else if (ver == 10) {
		if (read_uint(f, &prop_cnt)) printf("int Ooops\n");
		if (read_uint(f, &obj->elem_cnt)) printf("int Ooops\n");
	}

	for (i = 0 ; i < prop_cnt ; i++) {
		read_property (f, obj, ver);
	}

	for (i = 0 ; i < obj->elem_cnt ; i++) {
		elem = read_element (f, base, ver, obj->obj_type, obj->prop_mask, obj->prop_material);
		if (last) {
			last->next = elem;
			last = last->next;
		}
		else {
			obj->elem_list = last = elem;
		}
	}

	return obj;
}

unsigned int count_object (btg_object *object) {

	unsigned int count = 0;

	while (object) {
		object->elem_cnt = count_element (object->elem_list, object->obj_type);
		if (object->elem_cnt || object->obj_type == OBJ_BS ||
		    object->obj_type == OBJ_VERTEX || object->obj_type == OBJ_NORMAL ||
		    object->obj_type == OBJ_COLOR || object->obj_type == OBJ_TEXCOO
		    ) count++;
		object = object->next;
	}

	return count;
}

int write_object (FILE *f, btg_object *obj, unsigned int ver) {

	unsigned short us;
	unsigned int pcnt;

	while (obj) {
		if (obj->elem_cnt || obj->obj_type == OBJ_BS ||
		    obj->obj_type == OBJ_VERTEX || obj->obj_type == OBJ_NORMAL ||
		    obj->obj_type == OBJ_COLOR || obj->obj_type == OBJ_TEXCOO
	    )
		{
			pcnt = 0;
			if (obj->prop_mask) pcnt++;
			if (obj->prop_material) pcnt++;

			printf("object has %d elements\n", obj->elem_cnt);
			if (write_uchar(f, &obj->obj_type)) {
				fprintf(stderr, "problem while writing object type! break.\n");
				return 1;
			}
			if (ver == 7) {
				us = pcnt;
				if (write_ushort(f, &us)) return 2;
				us = obj->elem_cnt;
				if (write_ushort(f, &us)) return 2;
			}
			else if (ver == 10) {
				if (write_uint(f, &pcnt)) return 3;
				if (write_uint(f, &obj->elem_cnt)) return 3;
			}
			if (pcnt) {
				write_property (f, obj, ver);
			}
			if (write_element (f, obj->elem_list, ver, obj->obj_type, obj->prop_mask)) return 4;
		}
		obj = obj->next;
	}

	return 0;
}

void free_object (btg_object *obj) {
	btg_object *temp = NULL;
	if (obj) {
		while (obj) {
			temp = obj->next;
			if (obj->elem_cnt) free_element (obj->elem_list, obj->obj_type);
			free(obj);
			obj = temp;
		}
	}
}


btg_object *new_object (btg_object *object_all, unsigned char type, unsigned char mask, char *material) {

	btg_object *object = NULL;
	char *prop_material = NULL;

	if (!object_all) {
		fprintf(stderr, "pointer to object is NULL! break.\n");
		return NULL;
	}

	object = object_all;
	while (object->next) object = object->next;

	if ((object->next = malloc(sizeof(*object))) == NULL) {
		fprintf(stderr, "No memory left for object! break.\n");
		return NULL;
	}
	if ((prop_material = calloc(1, strlen(material) + 1)) == NULL) {
		fprintf(stderr, "No memory left for material! break.\n");
		free (object->next);
		object->next = NULL;
		return NULL;
	}
	strncpy (prop_material, material, strlen(material));

	object = object->next;
	object->obj_type = type;
	object->elem_cnt = 0;
	object->prop_mask = mask;
	object->prop_material = prop_material;
	object->elem_list = NULL;
	object->next = NULL;

	return object;
}
