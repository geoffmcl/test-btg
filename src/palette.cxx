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
#include "utils.hxx"
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

/* Atlas default.ap
# Water
Colour water		0.671 0.737 0.745 1.0
# Cities
Colour city		    1.000 0.973 0.459 1.0
# Transportation
Colour road		    0.250 0.250 0.250 1.0
Colour railroad		1.000 0.000 0.000 1.0
# Airports
Colour runway		0.0   0.0   0.0	  1.0
Colour taxiway		0.5   0.5   0.5   1.0
*/

//# Physical feature colours
PALMAP sPalMap[] = {
    //# Water
    { "water", 0.078, 0.750, 1.000, 1.0, 0, 0 },
    // { "water", 0.2, 0.2, 0.745, 1.0, 0, 0 },
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
    { "black",      0.0,   0.0,   0.0,  0.0, 0 , 0 },

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
// #############################################################
//  const char *mat, const char *color, int priority, count;
// #############################################################
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
    { "Gravel",          "brown",   178, 0 },
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
    { "SomeSort",        "brown",    74, 0 }, // other	# Savanna
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
    { "pa_threshold",    "runway",  104, 0 },
    { "pa_1l",           "runway",  105, 0 },
    { "pa_2r",           "runway",  106, 0 },
    { "pa_centerline",   "white",   107, 0 },
    { "pa_aim",          "runway",  108, 0 },
    { "pa_rest",         "runway",  109, 0 },
    { "pa_3l",           "runway",  110, 0 },
    { "pa_0r",           "runway",  111, 0 },
    { "pa_shoulder_f",   "runway",  112, 0 },
    { "lf_runway_hold",  "runway",  113, 0 },
    { "lf_dbl_solid_yellow","yellow",114,0 },
    { "dirt_rwy",        "brown",   115, 0 },
    { "pc_heli",         "yellow",  116, 0 },
    { "pc_threshold",    "runway",  117, 0 },
    { "grass_rwy",       "green",   118, 0 },
    { "pa_0l",           "runway",  119, 0 },
    { "pa_7r",           "runway",  120, 0 },
    { "pa_2l",           "runway",  121, 0 },
    { "pa_5r",           "runway",  122, 0 },
    { "pc_shoulder_f",   "runway",  123, 0 },
    { "pa_heli",         "yellow",  124, 0 },
    { "pc_1l",           "runway",  125, 0 },
    { "pc_0r",           "runway",  126, 0 },
    { "pc_centerline",   "runway",  127, 0 },
    { "pc_aim",          "runway",  128, 0 },
    { "pc_rest",         "runway",  129, 0 },
    { "pc_2l",           "runway",  130, 0 },
    { "pc_8r",           "runway",  131, 0 },
    { "lf_runway_hold_border", "runway", 132, 0 },
    { "lf_dbl_solid_yellow_border", "yellow", 133, 0 },
    { "pa_4r",           "runway",  134, 0 },
    { "pa_dspl_thresh",  "runway",  135, 0 },
    { "pa_dspl_arrows",  "runway",  136, 0 },
    { "pa_L",            "runway",  137, 0 },
    { "pa_3r",           "runway",  138, 0 },
    { "pa_R",            "runway",  139, 0 },
    { "pa_1r",           "runway",  140, 0 },
    { "pc_dspl_thresh",  "runway",  141, 0 },
    { "pc_dspl_arrows",  "runway",  142, 0 },
    { "pc_R",            "runway",  143, 0 },
    { "pc_2r",           "runway",  144, 0 },
    { "pc_tz_three",     "runway",  145, 0 },
    { "pc_tz_two_a",     "runway",  146, 0 },
    { "pc_tz_two_b",     "runway",  147, 0 },
    { "pc_tz_one_a",     "runway",  148, 0 },
    { "pc_tz_one_b",     "runway",  149, 0 },
    { "pc_stopway",      "runway",  150, 0 },
    { "pc_L",            "runway",  151, 0 },
    { "pc_3l",           "runway",  152, 0 },
    { "pa_9r",           "runway",  153, 0 },
    { "pa_shoulder",     "runway",  154, 0 },
    { "lf_safetyzone_centerline", "runway", 155, 0 },
    { "lf_sng_lane_queue_border", "runway", 156, 0 },
    { "lf_sng_solid_white", "white", 157, 0 },
    { "lf_sng_solid_yellow", "yellow", 158, 0 },
    { "pc_7r",           "runway",  159, 0 },
    { "pc_5r",           "runway",  160, 0 },
    { "pa_6r",           "runway",  161, 0 },
    { "pa_stopway",      "runway",  162, 0 },
    { "lf_other_hold",   "runway",  163, 0 },
    { "lf_sng_broken_yellow", "yellow", 164, 0 },
    { "lf_ils_hold",     "runway",  165, 0 },
    { "pa_tz_three",     "runway",  166, 0 },
    { "pa_tz_two_a",     "runway",  167, 0 },
    { "pa_tz_two_b",     "runway",  168, 0 },
    { "pa_tz_one_a",     "runway",  169, 0 },
    { "pa_tz_one_b",     "runway",  170, 0 },
    { "lf_other_hold_border", "runway", 171, 0 },
    { "lf_dbl_lane_queue_border", "runway", 172, 0 },
    { "lf_ils_hold_border", "runway", 173, 0 },
    { "lf_safetyzone_centerline_border", "runway", 174, 0 },
    { "pc_4r",           "runway",  175, 0 },
    { "pa_8r",           "runway",  176, 0 },
    { "lf_dbl_lane_queue", "runway", 177, 0 },
    { "lf_broken_white", "white", 179, 0 },
    { "lf_checkerboard_white", "white", 180, 0 },
    { "lf_sng_broken_yellow_border", "yellow", 181, 0 },
    { "lf_sng_lane_queue", "runway", 182, 0 },
    { "pc_0l",           "runway",  183, 0 },
    { "pc_6r",           "runway",  184, 0 },
    { "DryCropPastureCover", "brown", 185, 0 },
    { "MixedCropPastureCover", "brown", 186, 0 },
    { "DeciduousBroadCover", "green", 187, 0 },
    { "pc_3r",           "runway",  188, 0 },
        /* LAST ENTRY IN TABLE */
    { 0, 0, 0, 0 }
};

static vSTGS vMissed;

static int get_last_priority()
{
    int last = 0;
    PMATMAP pmm = &sMatMap[0];
    while ( pmm->mat ) {
        last = pmm->priority;
        pmm++;
    }
    return last;
}

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

static void get_text_color(char *cp, PPALMAP ppm)
{
    double c;
    unsigned int i;
    strcpy(cp,"#");
    c = 255.0 - (ppm->r * 255.0);
    i = (unsigned int)(c + 0.5);
    sprintf(EndBuf(cp),"%02x", i);
    c = 255.0 - (ppm->g * 255.0);
    i = (unsigned int)(c + 0.5);
    sprintf(EndBuf(cp),"%02x", i);
    c = 255.0 - (ppm->b * 255.0);
    i = (unsigned int)(c + 0.5);
    sprintf(EndBuf(cp),"%02x", i);
}

static void get_color(char *cp, PPALMAP ppm)
{
    double c;
    unsigned int i;
    strcpy(cp,"#");
    c = ppm->r * 255.0;
    i = (unsigned int)(c + 0.5);
    sprintf(EndBuf(cp),"%02x", i);
    c = ppm->g * 255.0;
    i = (unsigned int)(c + 0.5);
    sprintf(EndBuf(cp),"%02x", i);
    c = ppm->b * 255.0;
    i = (unsigned int)(c + 0.5);
    sprintf(EndBuf(cp),"%02x", i);
}


static std::string get_pal_color( const char *name )
{
    PPALMAP ppm = &sPalMap[0];
    char buf[256];
    char *cp = buf;
    std::string s("#f00000");
    while ( ppm->name ) {
        if (strcmp(ppm->name, name) == 0) {
            get_color(cp,ppm);
            s = cp;
            ppm->used++;
            return s;
        }
        ppm++;
    }
    SPRTF("%s: oops local service called with %s'! *** NOT FOUND FIX ME ***\n", module, name );
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
    if (!string_in_vec(vMissed,mat)) {
        SPRTF("%s: OOPS mat '%s'! *** NOT FOUND FIX ME ***\n", module, mat );
        vMissed.push_back(mat);
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

static const char *html_head = "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<meta charset=\"UTF-8\">\n"
    "<title>Current Default Palette</title>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Current Default Palette</h1>\n";

static const char *table =
    "<table border=\"2\" summary=\"Default Palette Colors\" cellpadding=\"3\">\n";

    // "<table summary=\"Default Palette Colors\" cellpadding=\"3\" cellspacing=\"3\">\n";


std::string get_palette_html()
{
    std::string html(html_head);
    PPALMAP ppm = sPalMap;
    char buf[256];
    char *cp = buf;
    std::string td, ft;
    if (!done_init) {
        done_init = true;
        init_used();
    }

    PMATMAP pmm = sMatMap;
    html += "<p>Current Materials</p>\n";
    html += table;
    int i;
    int max = 3;
    int wrap = 0;
    int cols = 4;
    html += "<tr>\n";
    while (wrap < max) {
        html += "<th>Ord</th>";
        html += "<th>Material</th>";
        html += "<th>Type</th>";
        html += "<th>Color</th>";
        wrap++;
    }
    html += "<tr/>\n";
    wrap = 0;
    while ( pmm->mat ) {

        std::string s = get_pal_color(pmm->color);
        td = "<td bgcolor=\"";
        td += s;
        td += "\">";

        if (wrap == 0)
            html += "<tr>\n";

        // col 1
        sprintf(cp,"%d", pmm->priority);
        html += "<td>";
        html += cp;
        html += "</td>\n";

        // col 2
        html += td;
        html += pmm->mat;
        html += "</td>";

        // col 3
        html += td;
        html += pmm->color;
        html += "</td>";

        // col 4
        html += td;
        html += s;
        html += "</td>";

        wrap++;
        if (wrap >= max) {
            html += "</tr>\n";
            wrap = 0;
        }
        pmm++;
    }
    if (wrap) {
        while (wrap < max) {
            for (i = 0; i < cols; i++)
                html += "<td>&nbsp;</td>";
            wrap++;
        }
        html += "</tr>\n";
    }
    html += "</table>\n";

    ///////////////////////////////////////////////////////////////////////////////////
    html += "<p>Current Palette</p>\n";
    html += table;
    html += "<tr>\n";
    html += "<th>Type</th>";
    html += "<th>Color</th>";
    html += "<th>RGB</th>";
    html += "<th>Cnt</th>";
    html += "<tr/>\n";
    while (ppm->name) {
        // setup font color
        if (strcmp(ppm->name,"taxiway")) {
            get_text_color(cp, ppm);
            ft = "<font color=\"";
            ft += cp;
            ft += "\">";
        } else {
            ft = "<font color=\"white\">";
        }

        // setup background color
        get_color(cp, ppm);
        td = "<td bgcolor=\"";
        td += cp;
        td += "\">";

        html += "<tr>\n";

        html += td;
        html += "<tt>";
        html += ft;
        html += ppm->name;
        html += "</font>";
        html += "</tt>";
        html += "</td>\n";

        html += td;
        html += "<tt>";
        html += ft;
        html += cp;
        html += "</font>";
        html += "</tt>";
        html += "</td>\n";

        sprintf(cp,"r=%.3lf, g=%.3lf, b=%.3lf", ppm->r, ppm->g, ppm->b);
        html += td;
        html += "<tt>";
        html += ft;
        html += cp;
        html += "</font>";
        html += "</tt>";
        html += "</td>\n";

        sprintf(cp,"%d",ppm->used);
        html += "<td>";
        html += cp;
        html += "</td>\n";

        html += "</tr>\n";
        ppm++;
    }
    html += "</table>\n";
    ///////////////////////////////////////////////////////////////////////////////////


    html += "<p>EOF</p>\n";

    html += "</body>\n";
    html += "</html>\n";
    return html;
}

void close_palette()
{
    size_t ii, max = vMissed.size();
    if (max) {
        int last = get_last_priority();
        SPRTF("%s: WARNING: Following %d materials NOT in table! *** FIX ME ***\n", module, (int)max);
        std::string mat;
        for (ii = 0; ii < max; ii++) {
            mat = vMissed[ii];
            last++;
            SPRTF("    { \"%s\", \"UNKNOWN\", %d, 0 },\n", mat.c_str(), last );
        }
    }
    vMissed.clear();
}
// eof = palette.cxx
