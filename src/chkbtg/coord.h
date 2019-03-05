#ifndef BTG_COORD_H
#define BTG_COORD_H

// Values of WGS84
// Flattening                                 (0.00335281066474748072)
#define FLATTERING (1.0 / 298.257223563)
// Reciprocal of flattening                   (0.99664718933525251928)
#define RECIP_FLATTERING (1.0 - FLATTERING)
// Semi-major axis
#define MAJOR_AXIS 6378137.00000000
// Semi-minor axis                      (6356752.31424517949756396660)
#define MINOR_AXIS (MAJOR_AXIS * RECIP_FLATTERING)

#include "struct.h"

typedef struct coord_geo_s {
	long double lat;
	long double lon;
	long double msl;
} coord_geo;

typedef struct coord_cart_s {
	long double x;
	long double y;
	long double z;
} coord_cart;

coord_geo  cart2geo (coord_cart cart);
coord_cart geo2cart (coord_geo  geo);

coord_geo index2geo (int index);
int geo2index (coord_geo geo);

void occupied_tiles (int index[], coord_cart coo, btg_bsphere *bs);
void find_maxima (int index[], btg_header *head);
void find_neighbours (int index[]);

#endif
