#ifndef BTG_AIRPORT_H
#define BTG_AIRPORT_H

#include "struct.h"

typedef struct als_lights_s {
	int size;
	const float *head;
	const float *side;
} als_lights;

void set_runway_info (btg_base *base, runway_info *runway);
void change_als (btg_base *base, btg_object *object, runway_info *runway);

btg_vertex *get_origin (btg_fence *fence);
void get_rwy_vector (threshold_info *from, threshold_info *to, vector *heading, vector *toside);
vector turnvector (vector vec, const double lon, const double lat);

int build_als   (btg_object *obj, btg_vertex *vertex_all, btg_normal *normal, threshold_info *threshold, als_lights lights, vector heading, vector toside, vector heading_p, vector toside_p);
int build_rail  (btg_object *obj, btg_vertex *vertex_all, btg_normal *normal, threshold_info *threshold, vector heading, vector toside, vector heading_p, vector toside_p, float spacing);
int build_rapid (btg_object *obj, btg_vertex *vertex_all, btg_normal *normal, threshold_info *threshold, vector heading, vector toside, vector heading_p, vector toside_p, float spacing);

#endif
