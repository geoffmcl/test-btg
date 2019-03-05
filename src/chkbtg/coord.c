#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "coord.h"

coord_geo cart2geo (coord_cart cart) {

	coord_geo geo;

	long double length, angle1, angle2;
	coord_cart want, init, temp, dist;
	int count = 0;

	length = sqrtl((cart.x * cart.x) + (cart.y * cart.y));

	want.x = length;
	want.y = 0.0;
	want.z = cart.z;

	angle1 = atanl((want.z * (1.0 / RECIP_FLATTERING) * (1.0 / RECIP_FLATTERING)) / want.x);
	init.x = cosl(angle1) * MAJOR_AXIS;
	init.z = sinl(angle1) * MAJOR_AXIS;
	angle2 = angle1 + 5.0;

	while (fabsl(angle1 - angle2) > 0.0005 && count < 10) {

		temp = init;

		temp.z *= RECIP_FLATTERING;
		angle1 = atanl(temp.z / temp.x);
		temp.x = cosl(angle1) * MAJOR_AXIS;
		temp.z = sinl(angle1) * MAJOR_AXIS;
		temp.z *= RECIP_FLATTERING;

		dist.x = want.x - temp.x;
		dist.z = want.z - temp.z;
		angle2 = atanl(dist.z / dist.x);

		init.x += dist.x;
		init.z += dist.z;
		angle1 = atanl(init.z / init.x);

		init.x = cosl(angle1) * MAJOR_AXIS;
		init.z = sinl(angle1) * MAJOR_AXIS;

		count++;
	}

	geo.lat = angle1;
	geo.lon = acosl(cart.x / length);
	if (cart.y < 0.0) geo.lon *= -1.0;
	geo.msl = sqrtl((dist.x * dist.x) + (dist.z * dist.z));
	if (dist.x < 0.0) geo.msl *= -1.0;

	return geo;
}



coord_cart geo2cart (coord_geo geo) {

	coord_cart cart;
	long double temp;

//move to latitude
// first run
	cart.x = cos(geo.lat);
	cart.z = sin(geo.lat) * RECIP_FLATTERING;
// second run
	temp = atan(cart.z / cart.x);
	cart.x = cos(temp);
	cart.z = sin(temp) * RECIP_FLATTERING;
// result
	cart.x = cart.x * MAJOR_AXIS;
	cart.z = cart.z * MAJOR_AXIS;

// above sealevel
	if (geo.msl) {
		cart.x = cart.x + (cos(geo.lat) * geo.msl);
		cart.z = cart.z + (sin(geo.lat) * geo.msl);
	}

// move to longitude
	cart.y = cart.x * sin(geo.lon);
	cart.x = cart.x * cos(geo.lon);

	return cart;
}

coord_geo index2geo (int index) {

	coord_geo geo;

	int base_x, slice_x, base_y, slice_y;
	float slices;

	slice_x = index & 0x00000007;
	index >>= 3;
	slice_y = index & 0x00000007;
	index >>= 3;
	base_y = index & 0x000000ff;
	index >>= 8;
	base_x = index;

	base_x -= 180;
	base_y -= 90;

	if      (abs(base_y) < 22.0) slices = 8.0;
	else if (abs(base_y) < 62.0) slices = 4.0;
	else if (abs(base_y) < 76.0) slices = 2.0;
	else if (abs(base_y) < 83.0) slices = 1.0;
	else if (abs(base_y) < 86.0) slices = 0.5;
	else if (abs(base_y) < 88.0) slices = 0.25;
	else if (abs(base_y) < 89.0) slices = 0.125;
	else slices = 0.000001f;

	geo.lat = ((base_y + slice_y / 8.0) * M_PI) / 180.0;
	geo.lon = ((base_x + slice_x / slices) * M_PI) / 180.0;
	geo.msl = 0.0;

	return geo;
}

int geo2index (coord_geo geo) {

	int index, base_x, slice_x, base_y, slice_y;
	float slices;

	geo.lon = (geo.lon * 180.0) / M_PI;
	geo.lat = (geo.lat * 180.0) / M_PI;

// avoid N90
	if (geo.lat == 90.0) geo.lat = 89.99999999;
// overshooting pole
	if (geo.lat > 90.0) {
		geo.lat = 90.0 - (geo.lat - 90.0);
		geo.lon += 180.0;
	}
	if (geo.lat < -90.0) {
		geo.lat = (-90.0) - (geo.lat - (-90.0));
		geo.lon += 180.0;
	}

// crossing dateline
	if (geo.lon >= 180.0) geo.lon -= 360.0;
	if (geo.lon < -180.0) geo.lon += 360.0;

	if      (fabsl(geo.lat) < 22.0) slices = 8.0;
	else if (fabsl(geo.lat) < 62.0) slices = 4.0;
	else if (fabsl(geo.lat) < 76.0) slices = 2.0;
	else if (fabsl(geo.lat) < 83.0) slices = 1.0;
	else if (fabsl(geo.lat) < 86.0) slices = 0.5;
	else if (fabsl(geo.lat) < 88.0) slices = 0.25;
	else if (fabsl(geo.lat) < 89.0) slices = 0.125;
	else { slices = 0.00001f; geo.lon = 0.0; }

	base_y = (int) floorl(geo.lat);
	slice_y = (int) truncl((geo.lat - base_y) * 8.0);

	base_x = (int) floorl(floorl(geo.lon * slices) / slices);
	if (base_x < -180) base_x = -180;
	slice_x = (int) floorl((geo.lon - base_x) * slices);

	index = ((base_x + 180) << 14) + ((base_y + 90) << 6) + (slice_y << 3) + slice_x;

	return index;
}


void occupied_tiles(int index[], coord_cart coo, btg_bsphere *bs) {

	int ind, i;
	coord_geo geo;

	coo.x += bs->coord.x;
	coo.y += bs->coord.y;
	coo.z += bs->coord.z;
	geo = cart2geo (coo);
	ind = geo2index(geo);

	for (i = 0 ; i < 5 ; i++) {
		if (index[i] == ind) break;
		if (index[i] == 0) {
			index[i] = ind;
			break;
		}
	}
}

void find_maxima (int index[], btg_header *head) {

	btg_bsphere *bs = head->base.bsphere;
	long double sin_lat, cos_lat, sin_lon, cos_lon;
	coord_cart center, upper, right, uple, upre, lole, lore;
	coord_geo geo;

	center.x = bs->coord.x;
	center.y = bs->coord.y;
	center.z = bs->coord.z;
	geo = cart2geo (center);

#ifdef _MSC_VER /* replace 'sincosl' */
    sin_lat = sin(geo.lat);
    cos_lat = cos(geo.lat);
    sin_lon = sin(geo.lon);
    cos_lon = cos(geo.lon);
#else
	sincosl(geo.lat, &sin_lat, &cos_lat);
	sincosl(geo.lon, &sin_lon, &cos_lon);
#endif /* _MSC_VER y/n */

	upper.x = (bs->r * sin_lat * cos_lon * -1.0);
	upper.y = (bs->r * sin_lat * sin_lon * -1.0);
	upper.z = bs->r * cos_lat;

	right.x = bs->r * sin_lon * -1.0;
	right.y = bs->r * cos_lon;
	right.z = 0;

	uple.x = upper.x + right.x * -1.0;
	uple.y = upper.y + right.y * -1.0;
	uple.z = upper.z + right.z * -1.0;

	upre.x = upper.x + right.x;
	upre.y = upper.y + right.y;
	upre.z = upper.z + right.z;

	lole.x = (right.x + upper.x) * -1.0;
	lole.y = (right.y + upper.y) * -1.0;
	lole.z = (right.z + upper.z) * -1.0;

	lore.x = right.x + upper.x * -1.0;
	lore.y = right.y + upper.y * -1.0;
	lore.z = right.z + upper.z * -1.0;

	occupied_tiles(index, uple, bs);
	occupied_tiles(index, upre, bs);
	occupied_tiles(index, lole, bs);
	occupied_tiles(index, lore, bs);
}



void find_neighbours(int index[]) {

	int ind;
	coord_geo geo, tmp;

	geo = index2geo(index[0]);

// avoid north pole
	if (geo.lat < 89.0) {
		tmp = geo;
		do {
		tmp.lat += 0.1;
		ind = geo2index(tmp);
	} while (ind == index[0]);
		index[1] = ind;
	}
	else index[1] = 0;

	tmp = geo;
	do {
		tmp.lon += 0.1;
		ind = geo2index(tmp);
	} while (ind == index[0]);
	index[2] = ind;

// avoid south pole
	if (geo.lat > -89.0) {
	tmp = geo;
	do {
		tmp.lat -= 0.1;
		ind = geo2index(tmp);
	} while (ind == index[0]);
	index[3] = ind;
	}
	else index[3] = 0;

	tmp = geo;
	do {
		tmp.lon -= 0.1;
		ind = geo2index(tmp);
	} while (ind == index[0]);
	index[4] = ind;
}
