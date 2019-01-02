/*\
 * btg-elev.cxx
 *
 * Copyright (c) 2018 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for strdup(), ...
#include <simgear/misc/sg_path.hxx> // SGPath
#include <simgear/io/sg_binobj.hxx> // SGBinObject
#include <simgear/io/lowlevel.hxx>  // reading services
#include <simgear/bucket/newbucket.hxx>
#include <simgear/math/SGMath.hxx>
// #include <simgear/compiler.h> // define SG_COMPILER_STR
#include <iostream>
#include <fstream>
#include "load-stg.hxx"
#include "load-btg.hxx"
// #include "test-btg.hxx"
#include "palette.hxx"
#include "sprtf.hxx"

#ifndef TEST_BTG_VERSION
#define TEST_BTG_VERSION "1.?.?"
#endif
#ifndef TEST_BTG_DATE
#define TEST_BTG_DATE "1970.01.01"    // Use epoch date as signal var not set! January 1, 1970
#endif

static const char *module = "btg-elev";

static const char *usr_input = 0;
// Mount Everest  lat 27.988 lon 86.9253 8848 e080n20/e086n27/4365691
static double usr_lat = 27.988; // -400;
static double usr_lon = 86.9253; // -400;
static double usr_alt = 0.0; //  8848.0; // 0

#ifndef DEF_SCENERY
#ifdef WIN32
#define DEF_SCENERY "G:\\S"
#else
#define DEF_SCENERY "/media/geoff/backup/next/scenery2"
#endif
#endif // DEF_SCENERY
static const char *fg_scenery = DEF_SCENERY;

static const char *def_log = "tempbtge.txt";


void give_help( char *name )
{
    SPRTF("%s: usage: [options] usr_input\n", module);
    SPRTF("Options:\n");
    SPRTF(" --help  (-h or -?) = This help and exit(0)\n");
    // TODO: More help
}

int parse_args( int argc, char **argv )
{
    int i,i2,c;
    char *arg, *sarg;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        i2 = i + 1;
        if (*arg == '-') {
            sarg = &arg[1];
            while (*sarg == '-')
                sarg++;
            c = *sarg;
            switch (c) {
            case 'h':
            case '?':
                give_help(argv[0]);
                return 2;
                break;
            // TODO: Other arguments
            default:
                SPRTF("%s: Unknown argument '%s'. Try -? for help...\n", module, arg);
                return 1;
            }
        } else {
            // bear argument
            if (usr_input) {
                SPRTF("%s: Already have input '%s'! What is this '%s'?\n", module, usr_input, arg );
                return 1;
            }
            usr_input = strdup(arg);
        }
    }
    //if (!usr_input) {
    //    SPRTF    set_log_file(cp,false);
("%s: No user input found in command!\n", module);
    //    return 1;
    //}
    if (usr_lat == -400.0) {
        SPRTF("%s: No user latitude found in command!\n", module);
        return 1;
    }
    if (usr_lon == -400.0) {
        SPRTF("%s: No user longitude found in command!\n", module);
        return 1;
    }
    return 0;
}

int test_bucket()
{
    int ret = 1;
    bool ok = false;
    SGVec3<double> cart;
    cart.x() = usr_lat;
    cart.y() = usr_lon;
    cart.z() = usr_alt;

    SGGeod geod;
    // geod.fromCart(cart);
    ///geod.fromDegM(usr_lon, usr_lat, usr_alt);
    geod.setLongitudeDeg(usr_lon);
    geod.setLatitudeDeg(usr_lat);
    geod.setElevationM(usr_alt);

    SGBucket b(geod);

    std::string bp = b.gen_base_path();
    std::string ind = b.gen_index_str();
    SPRTF("%s: lat %f, lon %f, alt %f = bucket %s/%s\n", module,
        usr_lat, usr_lon, usr_alt,
        bp.c_str(), ind.c_str());

    SGPath path(fg_scenery);

    //path.add("Terrain");
    path.append("Terrain");
    path.append(bp);
    path.append(ind);
    path.concat(".btg.gz");
    std::string file = path.str();
    SPRTF("File %s - ", file.c_str());
    if (path.exists()) {
        SPRTF("ok");
        ok = true;
    }
    else {
        SPRTF("NOT FOUND");
    }
    SPRTF("\n");
    if (ok) {
        load_stg *pls = new load_stg;
        load_btg *plb = new load_btg;
        PMOPTS pmo = new MOPTS;
        memset(pmo, 0, sizeof(MOPTS));
        ret = plb->load(path, pmo);
        if (!ret) {
            SPRTF("%s: Loaded '%s'\n", module, file.c_str());
        }
        
    }

    return ret;
}

// main() OS entry
int main( int argc, char **argv )
{
    int iret = 0;
    set_log_file((char *)def_log, false);
    iret = parse_args(argc,argv);
    if (iret) {
        if (iret == 2)
            iret = 0;
        return iret;
    }

    iret = test_bucket(); // actions of app

    return iret;
}


// eof = btg-elev.cxx
