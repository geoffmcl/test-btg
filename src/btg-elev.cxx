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
#include <simgear/debug/logstream.hxx>
// #include <simgear/compiler.h> // define SG_COMPILER_STR
#include <iostream>
#include <fstream>
#include "load-stg.hxx"
#include "load-btg.hxx"
#include "test-btg.hxx" // options, ...
#include "palette.hxx"
#include "sprtf.hxx"
#include "utils.hxx"

#ifndef TEST_BTG_VERSION
#define TEST_BTG_VERSION "1.?.?"
#endif
#ifndef TEST_BTG_DATE
#define TEST_BTG_DATE "1970.01.01"    // Use epoch date as signal var not set! January 1, 1970
#endif

#ifndef ISDIGIT
#define ISDIGIT(a) ((a >= '0') && (a <= '9'))
#endif


static const char *module = "btg-elev";

static const char *usr_input = 0;
// Mount Everest  lat 27.988 lon 86.9253 8848 e080n20/e086n27/4365691
static double usr_lat = 27.988; // -400;
static double usr_lon = 86.9253; // -400;
static double usr_alt = 0.0; //  8848.0; // 0
static int usr_options = opt_add_xg_text | opt_add_xg_bbox;

#ifndef DEF_SCENERY
#ifdef WIN32
#define DEF_SCENERY "G:\\S"
#else
#define DEF_SCENERY "/media/geoff/backup/next/scenery2"
#endif
#endif // DEF_SCENERY
static const char *fg_scenery = DEF_SCENERY;

static int verbosity = 0;

static const char *def_log = "tempbtge.txt";

static const char *app_version = TEST_BTG_VERSION;
static const char *app_date = TEST_BTG_DATE;

const char *xg_file = "tempbuck.xg";

void show_version()
{
    SPRTF("%s: Date %s, Version %s\n", module, app_date, app_version);
}
void give_help( char *name )
{
    show_version();
    SPRTF("%s: usage: [options] lat lon\n", module);
    SPRTF("Options:\n");
    SPRTF(" --help     (-h or -?) = This help and exit(0)\n");
    SPRTF(" --scenery <path> (-s) = Path to FG scenery. (def=%s)\n", fg_scenery);

    // TODO: More help

    SPRTF(" Given a lat,lon location, seek FG index.btg.gz got that Bucket,\n");
    SPRTF(" and show elevation of that point.\n");
}

int parse_args( int argc, char **argv )
{
    int i,i2,c;
    char *arg, *sarg, *pt;
    double tmp_dbl;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        i2 = i + 1;
        if (strcmp(arg, "--version") == 0) {
            show_version();
            return 2;
        }
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
            case 'v':
                verbosity++;
                sarg++;
                while (*sarg) {
                    if (*sarg == 'v') {
                        verbosity++;
                    }
                    else if (ISDIGIT(*sarg)) {
                        verbosity = atoi(sarg);
                        break;
                    }
                }
                if (VERB2(verbosity)) {
                    SPRTF("%s: Set verbosity to %d\n", module, verbosity);
                }
                break;
            case 'l':

                if (i2 < argc) {
                    if (strncmp(sarg, "lat", 3) == 0) {
                        c = 'a';
                    }
                    else if (strncmp(sarg, "lon", 3) == 0) {
                        c = 'o';
                    }
                    else {
                        SPRTF("%s: Expected 'lat' or 'lon', got %s\n", module, sarg);
                        return 1;
                    }
                    pt = GetNxtBuf();
                    strcpy(pt, sarg);
                    // get next argument - should be a double...
                    i++;
                    sarg = argv[i];
                    tmp_dbl = atof(sarg);
                    if (c == 'a')
                        usr_lat = tmp_dbl;
                    else
                        usr_lon = tmp_dbl;
                    if (VERB2(verbosity)) {
                        SPRTF("%s: Set usr '%s' to '%f'.\n", module, pt, tmp_dbl);
                    }
                }
                else {
                    SPRTF("%s: Error: Expected a double to follow %s! Aborting...", module,
                        arg);
                    return 1;
                }
                break;
            case 'x':
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    xg_file = strdup(sarg);
                    usr_options |= opt_add_xg_text;
                    if (VERB2(verbosity)) {
                        SPRTF("%s: Added xg text opt to '%s'.\n", module, xg_file);
                    }
                }
                else {
                    SPRTF("%s: Error: Expected file name to follow %s! Aborting...", module,
                        arg);
                    return 1;
                }
                break;
                // TODO: Other arguments
            default:
                SPRTF("%s: Unknown argument '%s'. Try -? for help...\n", module, arg);
                return 1;
            }
        } else {
            // bear argument
            //if (usr_input) {
            //    SPRTF("%s: Already have input '%s'! What is this '%s'?\n", module, usr_input, arg );
            //    return 1;
            //}
            //usr_input = strdup(arg);
            if (usr_lat == -400.0) {
                usr_lat = atof(arg);
            }
            else {
                usr_lon = atof(arg);
            }
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

int write_xg_file(PMOPTS pmo, load_btg *plb)
{
    int iret = 1;   // set failed
    int options = 0;
    std::string g_xg = "";
    if (pmo) {
        options = pmo->options;
        g_xg = pmo->xg; // copy string - not really needed....
    }
    ///////////////////////////////////////////////////////////////////////////
    if (xg_file && (options & opt_add_xg_text) && g_xg.size()) {
        FILE *fp = fopen(xg_file, "w");
        if (fp) {
            size_t res = 1;
#if 0 // 0000000000000000000000000000000000000000000000000000
            if (add_tri_bbox) {
                char *cp = get_mbbox_xg_stg(&tri_bb);
                if (cp) {
                    std::string msg("color = green\n");
                    msg += cp;
                    msg += "NEXT\n";
                    res = fwrite(msg.c_str(), msg.size(), 1, fp);
                }
            }
#endif // 000000000000000000000000000000000000000000000000000
            if (res == 1)
                res = fwrite(g_xg.c_str(), g_xg.size(), 1, fp);
            fclose(fp);
            if (res != 1) {
                SPRTF("%s: Failed to write xg to '%s'!\n", module, xg_file);
                iret = 3;
            }
            else {
                SPRTF("%s: Written xg file '%s'\n", module, xg_file);
                iret = 0; // success
            }
        }
        else {
            SPRTF("%s: Failed to open xg to '%s'!\n", module, xg_file);
            iret = 2;
        }
    }


    return iret;
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
        pmo->verb = verbosity;
        pmo->options = usr_options;
        ret = plb->load(path, pmo);
        if (!ret) {
            SPRTF("%s: Loaded '%s'\n", module, file.c_str());
            ret = write_xg_file(pmo, plb);
        }


        delete pls;
        delete plb;
        delete pmo;
    }

    return ret;
}

// main() OS entry
int main( int argc, char **argv )
{
    int iret = 0;
    SGPath log(def_log);
    set_log_file((char *)def_log, false);
    sglog().setLogLevels(SG_ALL, SG_DEBUG);
    sglog().logToFile(log, SG_ALL, SG_DEBUG);
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
