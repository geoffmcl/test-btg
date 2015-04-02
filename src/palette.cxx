/*\
 * palette.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <string>
#ifndef _MSC_VER
#include <string.h> // for strcmp, ...
#endif
#include "sprtf.hxx"
#include "palette.hxx"

static const char *module = "palette";

static bool done_init = false;

// implementation
typedef struct tagPALMAP {
    const char *name;
    double r,g,b,a;
    int used;
    int spare;
} PALMAP, *PPALMAP;

//# Physical feature colours
PALMAP sPalMap[] = {
    //# Water
    { "water", 0.671, 0.737, 0.745, 1.0, 0, 0 },
    // # Cities
    { "city", 1.000, 0.973, 0.459, 1.0, 0 , 0 },
    // # Transportation
    { "road", 0.250, 0.250, 0.250, 1.0, 0 , 0 },
    { "railroad",    1.0, 0.8, 0.8, 1.0, 0 , 0 },
    // # Airports
    { "runway",	     0.9,  0.9, 0.9, 1.0, 0 , 0 },
    { "taxiway",     0.5,  0.5, 0.5, 1.0, 0 , 0 },
    // # Useful colours 
    { "white",      1.0,   1.0,   1.0,  1.0, 0 , 0 },
    { "red",		1.0,   0.0,   0.0,  1.0, 0 , 0 },
    { "brown",      0.75,  0.25,  0.0,  1.0, 0 , 0 },
    { "green",		0.0,   1.0,   0.0,	1.0, 0 , 0 },
    { "blue",		0.0,   0.0,   1.0,  1.0, 0 , 0 },
    { "cyan",		0.0,   1.0,   1.0,	1.0, 0 , 0 },
    { "magenta",	1.0,   0.0,   1.0,	1.0, 0 , 0 },
    { "yellow",		1.0,   1.0,   0.0,  1.0, 0 , 0 },
    // END OF TABLE
    { 0,            0, 0, 0, 0, 0 }
};

typedef struct tagMATMAP {
    const char *mat;
    const char *color;
    int priority;
    int count;
}MATMAP, *PMATMAP;
/* ---------------------------------------------------------
    From terragear default_priorities.txt
    Is this the complete list????
Default				# Default area which can be overridden by
				# raster landcover data (e.g. USGS)
Ocean				# collect slivers as ocean

# Area types in order of descending priority
Hole			hole	# Leave area completely empty
Freeway			road
Road			road
Road-Motorway       road
Road-Trunk          road
Road-Residential	road
Road-Primary	    road
Road-Secondary	    road
Road-Tertiary	    road
Road-Service	    road
Road-Pedestrian	    road
Road-Steps          road
Road-Unclassified   road

Railroad		road
Asphalt			road
Airport			other
Pond			lake
Lake			lake
DryLake			lake
Reservoir		lake
IntermittentLake	lake
Stream			stream
River			stream
IntermittentStream	stream
Watercourse		stream
Canal			stream
Glacier			other	# Solid ice/snow
PackIce			other	# Water with ice packs
PolarIce		other
Ocean			ocean
Urban			other	# Densely-populated city or large town
SubUrban		other	# Densely-populated city or large town
Town			other	# Small town or village
Fishing			other
Construction		other
Industrial		other
Port			other
Dump			other	# Dump Site
FloodLand		other	# Land subject to flooding
Lagoon			other	# Lagoon
Bog			other	# Bog
Marsh			other	# Marshland or swamp
SaltMarsh		other
Sand			other	# Sand-covered area
Saline			other	# Saline
Littoral		other	# Tidal, Sand-covered area
Estuary			other
Dirt			other
Rock			other	# Rock
Lava			other	# Lava-covered area
OpenMining		other	# OpenMining

# USGS Land Covers
# These are low-priority, since known polygons should always win.

BuiltUpCover		other	# Urban and Built-Up Land
Transport		other	# Transport
Cemetery		other	# Cemetery
DryCrop			other	# Dryland Cropland and Pasture
IrrCrop			other	# Irrigated Cropland and Pasture
Rice			other
MixedCrop		other	# Mixed Dryland/Irrigated Cropland and Pasture
Vineyard		other
Bamboo			other
Mangrove		other
ComplexCrop		other	# Complex Cropland
NaturalCrop		other
CropGrass		other	# Cropland/Grassland Mosaic
CropWood		other	# Cropland/Woodland Mosaic
AgroForest		other
Olives			other
GolfCourse		other	# GolfCourse
Greenspace		other
GrassCover		other	# Grassland
Grassland		other
ScrubCover		other	# Scrub
Scrub			other
ShrubGrassCover		other	# Mixed Shrubland/Grassland
SavannaCover		other	# Savanna
Orchard			other	# Orchard
DeciduousForest		other	# Deciduous Forest
EvergreenForest		other	# Evergreen Forest
MixedForest		other	# Mixed Forest
RainForest		other	# Rain Forest
BarrenCover		other	# Barren or Sparsely Vegetated
HerbTundra		other	# Herbaceous Tundra
Sclerophyllous		other
Heath			other
Burnt			other
SnowCover		other	# Snow or Ice

Island			island	# any island area not covered otherwise
Default			landmass # any land area not covered otherwise

Void			other
Null			other
Unknown			other

    ======================================================================= */
MATMAP sMatMap[] = {
    { "Hole",            "magenta",  0, 0 },
    // ========
    { "Freeway",         "road",   1, 0 },
    { "Road",            "road",   2, 0 },
    { "Road-Motorway",   "road",   3, 0 },
    { "Road-Trunk",      "road",   4, 0 },
    { "Road-Residential","road",   5, 0 },
    { "Road-Primary",    "road",   6, 0 },
    { "Road-Secondary",  "road",   7, 0 },
    { "Road-Tertiary",   "road",   8, 0 },
    { "Road-Service",    "road",   9, 0 },
    { "Road-Pedestrian", "road",  10, 0 },
    { "Road-Steps",      "road",  11, 0 },
    { "Road-Unclassified","road", 12, 0 },
    // ========

    { "Railroad",        "railroad", 13, 0 },
    { "Asphalt",         "road",     14, 0 },
    { "Airport",         "red",      15, 0 },
    // ====
    { "Pond",            "water",    16, 0 }, // lake
    { "Lake",            "water",    17, 0 },
    { "DryLake",         "yellow",   18, 0 }, //	undefined 
    { "Reservoir",       "water",    19, 0 },
    { "IntermittentReservoir","water", 20, 0 }, // does this exist?
    { "IntermittentLake","water",    20, 0 },
    { "Stream",          "water",    21, 0 },
    { "River",           "water",    22, 0 }, // stream
    { "IntermittentStream","water",  23, 0 }, // stream
    { "Watercourse",     "water",    24, 0 }, // stream
    { "Canal",           "water",    25, 0 },
    { "Glacier",         "white",    26, 0 }, // undefined # Solid ice/snow
    { "PackIce",         "white",    27, 0 }, // other	# Water with ice packs
    { "PolarIce",        "white",    28, 0 }, // other
    { "Ocean",           "water",    29, 0 }, //# collect slivers as ocean
    // =========
    { "Urban",           "city",     30, 0 }, // # Densely-populated city or large town
    { "SubUrban",        "city",     31, 0 }, // other	# Densely-populated city or large town
    { "Town",            "city",     32, 0 },
    { "BuiltUpCover",    "city",     32, 0 }, // DOES THIS EXIST???
    { "Fishing",         "water",    33, 0 }, // other
    { "Construction",    "city",     34, 0 }, // other
    { "Industrial",      "city",     35, 0 }, // other
    { "Port",            "city",     36, 0 }, // other
    { "Dump",            "city",     37, 0 }, // other	# Dump Site
    { "FloodLand",       "water",    38, 0 }, // other	# Land subject to flooding
    { "Lagoon",          "water",    39, 0 }, // other	# Lagoon
    { "Bog",             "water",    40, 0 }, // other	# Bog
    { "Marsh",           "water",    41, 0 }, // other	# Marshland or swamp
    { "SaltMarsh",       "water",    42, 0 }, // other
    // ==========
    { "Sand",            "yellow",   43, 0 }, // other	# Sand-covered area
    { "Saline",          "water",    44, 0 }, // other	# Saline
    { "Littoral",        "water",    45, 0 }, // other	# Tidal, Sand-covered area
    { "Estuary",         "water",    46, 0 }, // other
    { "Grass",           "green",    46, 0 }, // ??????? undefined
    { "Dirt",            "brown",    47, 0 }, // other
    { "Rock",			 "brown",    48, 0 }, // other	# Rock
    { "Lava",            "brown",    49, 0 }, // other	# Lava-covered area
    { "OpenMining",      "brown",    50, 0 }, // other	# OpenMining
    // # USGS Land Covers
    // # These are low-priority, since known polygons should always win.
    // ========
    { "BuiltUpCover",    "city",     51, 0 }, // other	# Urban and Built-Up Land
    { "Transport",       "city",     52, 0 }, // other	# Transport
    { "Cemetery",        "city",     53, 0 }, // other	# Cemetery
    { "DryCrop",         "brown",    54, 0 }, // other	# Dryland Cropland and Pasture
    { "IrrCrop",         "water",    55, 0 }, // other	# Irrigated Cropland and Pasture
    { "Rice",            "brown",    56, 0 }, // other
    { "MixedCrop",       "brown",    57, 0 }, // other	# Mixed Dryland/Irrigated Cropland and Pasture
    { "Vineyard",        "brown",    58, 0 }, // other
    { "Bamboo",          "brown",    59, 0 }, // other
    { "Mangrove",        "water",    60, 0 }, // other
    { "ComplexCrop",     "brown",    61, 0 }, // other	# Complex Cropland
    { "NaturalCrop",     "brown",    62, 0 }, // other
    { "CropGrass",       "green",    63, 0 }, // other	# Cropland/Grassland Mosaic
    { "CropWood",        "green",    64, 0 }, // other	# Cropland/Woodland Mosaic
    { "AgroForest",      "green",    65, 0 }, // other
    { "Olives",          "green",    66, 0 }, // other
    { "GolfCourse",      "green",    67, 0 }, // other	# GolfCourse
    { "Greenspace",      "green",    68, 0 }, // other
    { "GrassCover",      "green",    69, 0 }, // other	# Grassland
    { "Grassland",       "green",    70, 0 }, // other
    { "ScrubCover",      "green",    71, 0 }, // other	# Scrub
    { "Scrub",           "green",    72, 0 }, // other
    { "ShrubGrassCover", "green",    73, 0 }, // other	# Mixed Shrubland/Grassland
    { "SavannaCover",    "brown",    74, 0 }, // other	# Savanna
    { "Orchard",         "brown",    75, 0 }, // other	# Orchard
    { "DeciduousForest", "green",    76, 0 }, // other	# Deciduous Forest
    { "EvergreenForest", "green",    77, 0 }, // other	# Evergreen Forest
    { "MixedForest",     "green",    78, 0 }, // other	# Mixed Forest
    { "RainForest",      "green",    79, 0 }, // other	# Rain Forest
    { "BarrenCover",     "green",    80, 0 }, // other	# Barren or Sparsely Vegetated
    { "HerbTundra",      "green",    81, 0 }, // other	# Herbaceous Tundra
    { "Sclerophyllous",  "green",    82, 0 }, // other
    { "Heath",           "green",    83, 0 }, // other
    { "Burnt",           "brown",    84, 0 }, // other
    { "SnowCover",       "white",    85, 0 }, // other	# Snow or Ice
    // =================
    { "Island",          "green",    86, 0 }, // island	# any island area not covered otherwise

    { "Default",         "magenta",  87, 0 }, // landmass # any land area not covered otherwise
    // ===========

    { "Void",            "white",    88, 0 }, // other
    { "Null",            "white",    89, 0 }, // other
    { "Unknown",         "white",    90, 0 }, // other

    // ====
    { "pa_taxiway",      "taxiway", 100, 0 },
    { "pa_tiedown",      "taxiway", 101, 0 },
    { "pc_taxiway",      "taxiway", 102, 0 },
    { "pc_tiedown",      "taxiway", 103, 0 },


    // LAST ENTRY IN TABLE
    { 0, 0 }
};

static void init_used() 
{
    done_init = true;
    PPALMAP ppm = &sPalMap[0];
    while ( ppm->name ) {
        ppm->used = 0;
        ppm++;
    }
    PMATMAP pmm = &sMatMap[0];
    while ( pmm->mat ) {
        pmm->count = 0;
        pmm++;
    }
}

static std::string get_pal_color( const char *name )
{
    PPALMAP ppm = &sPalMap[0];
    char buf[256];
    double c;
    char *cp = buf;
    int i;
    std::string s("#f00000");
    while ( ppm->name ) {
        if (strcmp(ppm->name, name) == 0) {
            c = ppm->r * 255.0;
            i = (int)(c + 0.5);
            strcpy(cp,"#");
            sprintf(EndBuf(cp),"%02x", i);
            c = ppm->g * 255.0;
            i = (int)c;
            sprintf(EndBuf(cp),"%02x", i);
            c = ppm->b * 255.0;
            i = (int)c;
            sprintf(EndBuf(cp),"%02x", i);
            s = cp;
            ppm->used++;
            return s;
        }
        ppm++;
    }
    printf("%s: oops local service called with %s'! NOT FOUND FIX ME ***\n", module, name );
    return s;
}


std::string get_mat_color( const char *mat )
{
    PMATMAP pmm = &sMatMap[0];
    if (!done_init) {
        done_init = true;
        init_used();
    }
    while( pmm->mat ) {
        if (strcmp(pmm->mat,mat) == 0) {
            pmm->count++;
            return get_pal_color(pmm->color);
        }
        pmm++;
    }
    return "#ff0000";
}

bool is_mat_in_list( const char *mat )
{
    PMATMAP pmm = &sMatMap[0];
    while( pmm->mat ) {
        if (strcmp(pmm->mat,mat) == 0) {
            return true;
        }
        pmm++;
    }
    return false;
}

void outout_mat_color_counts()
{
    PPALMAP ppm = &sPalMap[0];
    int tot_cnt = 0;
    int tot_cols = 0;
    int tot_use = 0;
    while ( ppm->name ) {
        tot_cols++;
        if (ppm->used) {
            tot_use++;
            tot_cnt += ppm->used;
        }
        ppm++;
    }
    if (tot_use) {
        SPRTF("%s: Internal %d colors used %d, total %d\n", module, tot_cols, tot_use, tot_cnt);
        ppm = &sPalMap[0];
        while ( ppm->name ) {
            if (ppm->used)
                SPRTF("color %s used %d\n", ppm->name, ppm->used);
            ppm++;
        }
    }

    tot_cnt = 0;
    tot_cols = 0;
    tot_use = 0;
    PMATMAP pmm = &sMatMap[0];
    while ( pmm->mat ) {
        tot_cols++;
        if (pmm->count) {
            tot_use++;
            tot_cnt += pmm->count;
        }
        pmm++;
    }
    if (tot_use) {
        SPRTF("%s: Internal %d materials used %d, total %d\n", module, tot_cols, tot_use, tot_cnt);
        pmm =  &sMatMap[0];
        while ( pmm->mat ) {
            if (pmm->count)
                SPRTF("mat %s used %d\n", pmm->mat, pmm->count);
            pmm++;
        }
    }

}

// eof = palette.cxx
