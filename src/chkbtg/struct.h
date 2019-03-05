#ifndef BTG_STRUCT_H
#define BTG_STRUCT_H

#include <time.h>

typedef struct fcoord_s {
	float x;
	float y;
	float z;
} fcoord;

typedef struct dcoord_s {
	double x;
	double y;
	double z;
} dcoord;

typedef struct btg_bsphere_s {
	short valid;
	unsigned int index;
	unsigned int count;
	dcoord coord;
	float  r;
	double lon;
	double lat;
	struct btg_bsphere_s *next;
} btg_bsphere;

typedef struct btg_vertex_s {
	short valid;
	unsigned int index;
	unsigned int count;
	struct btg_vertex_s *alias;
	fcoord relative;
	dcoord absolute;
	dcoord projection;
	struct btg_vertex_s *next;
} btg_vertex;

typedef struct btg_normal_s {
	short valid;
	unsigned int index;
	unsigned int count;
	struct btg_normal_s *alias;
	unsigned char x;
	unsigned char y;
	unsigned char z;
	struct btg_normal_s *next;
} btg_normal;

typedef struct btg_color_s {
	short valid;
	unsigned int index;
	unsigned int count;
	struct btg_color_s *alias;
	float r;
	float g;
	float b;
	float a;
	struct btg_color_s *next;
} btg_color;

typedef struct btg_texcoo_s {
	short valid;
	unsigned int index;
	unsigned int count;
	struct btg_texcoo_s *alias;
	float u;
	float v;
	struct btg_texcoo_s *next;
} btg_texcoo;

typedef struct btg_geometry_s {
	short valid;
	btg_vertex *vertex;
	btg_normal *normal;
	btg_texcoo *texcoo;
	btg_color *color;
	struct btg_geometry_s *next;
} btg_geometry;

typedef struct btg_element_s {
	short valid;
	unsigned int num_bytes;
	unsigned int count;
	void *element;
	struct btg_element_s *next;
} btg_element;

#define OBJ_BS            0    // Bounding Sphere
#define OBJ_VERTEX        1    // Vertex List
#define OBJ_NORMAL        2    // Normal List
#define OBJ_TEXCOO        3    // Texture Coordinates List
#define OBJ_COLOR         4    // Color List
#define OBJ_POINTS        9    // Points
#define OBJ_TRIS         10    // Individual Triangles
#define OBJ_STRIPE       11    // Triangle Strips
#define OBJ_FAN          12    // Triangle Fans
#define OBJ_UNSET       255    // not set

#define PROP_MAT          0    // Material
#define PROP_INDEX        1    // Index Types

#define MASK_UNSET     0x00
#define MASK_VERTEX    0x01
#define MASK_NORMAL    0x02
#define MASK_COLOR     0x04
#define MASK_TEXCOO    0x08
#define MASK_POINTS    0x10
#define MASK_TRIS      0x20
#define MASK_STRIPE    0x40
#define MASK_FAN       0x80

typedef struct btg_object_s {
	unsigned char obj_type;
	unsigned int  elem_cnt;
	unsigned char prop_mask;
	char *prop_material;
	btg_element *elem_list;
	struct btg_object_s *next;
} btg_object;

typedef struct vector_s {
	double x;
	double y;
	double z;
} vector;

typedef struct btg_point_s {
	short valid;
	btg_object *object;
	btg_geometry *geo;
	struct btg_point_s *next;
} btg_point;

typedef struct btg_triangle_s btg_triangle;

typedef struct btg_edge_s {
	int count;
	int mark;
	btg_vertex *vertex[2];
	btg_triangle *tria[10];
	struct btg_edge_s* next;
} btg_edge;

typedef struct btg_triangle_s {
	btg_object  *object;
	btg_element *elem;
	btg_edge    *edge[3];
	struct btg_triangle_s *next;
} btg_triangle;

typedef struct btg_trialist_s {
	btg_triangle *tria;
	struct btg_trialist_s *next;
} btg_trialist;

#define ALS_NOOP     -1   // leave it untouched
#define ALS_CLEAR     0   // delete ALS lights
// long ALS (default length: 3000ft / 900m)
#define ALS_ALSF1     1   // strong lights (PAR56) with flasher (PAR56)
#define ALS_ALSF2     2   // strong lights (PAR56) with flasher (PAR56)
// medium ALS (default length: 2400ft / 720m)
#define ALS_SSALS     3   // strong lights (PAR56)
#define ALS_SSALR     4   // strong lights (PAR56) with RAIL (PAR56)
#define ALS_SSALF     5   // strong lights (PAR56) with flasher (PAR56)
// short ALS (default length: 1400ft / 420m)
#define ALS_MALS      6   // medium lights (PAR38)
#define ALS_MALSR     7   // medium lights (PAR38) with RAIL (PAR56)
#define ALS_MALSF     8   // medium lights (PAR38) with flasher (PAR56)


typedef struct threshold_info_s {
	double lon;
	double lat;
	double heading;
	double displacement;
	double stopw;
	char rwy_num;
	char rwy_ord;
	int als_layout;
	int als_len;
	btg_vertex *origin;
} threshold_info;

typedef struct runway_info_s {
	threshold_info threshold[2];
	struct runway_info_s *next;
} runway_info;

typedef struct btg_fence_s btg_fence;

typedef struct btg_base_s {
	double min_x;
	double max_x;
	double min_y;
	double max_y;
	double holesize;
	char *material;
	btg_bsphere  *bsphere;
	btg_vertex   *vertex;
	btg_vertex   **vertex_array;
	btg_normal   *normal;
	btg_normal   **normal_array;
	btg_color    *color;
	btg_color    **color_array;
	btg_texcoo   *texcoo;
	btg_texcoo   **texcoo_array;
	btg_point    *point;
	btg_point    *point_last;
	btg_triangle *triangle;
	btg_triangle *triangle_last;
	btg_edge     *edge;
	btg_edge     *edge_last;
	btg_fence    *fence;
	btg_fence    *fence_last;
} btg_base;

typedef struct btg_header_s {
	time_t creation;
	unsigned short version;
	unsigned short mag_num;
	unsigned int num_object;
	runway_info *runway;
	btg_object *object;
	btg_base base;
} btg_header;

typedef struct btg_border_s {
	short side;
	btg_edge *edge;
	btg_vertex *vertex;
	btg_vertex *neighbour;
	btg_header *tile;
	struct btg_border_s *next;
} btg_border;

typedef struct btg_fence_s {
	short turn;
	double min_x;
	double max_x;
	double min_y;
	double max_y;
	btg_border *border;
	struct btg_fence_s *next;
} btg_fence;

#endif
