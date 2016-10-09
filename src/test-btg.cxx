/*\
 * test-btg.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <string.h> // for strdup(), ...
#include <simgear/misc/sg_path.hxx> // SGPath
#include <simgear/io/sg_binobj.hxx> // SGBinObject
#include <iostream>
#include <fstream>
#ifdef WIN32
#include <Windows.h>
#endif
#include <dirent.h>
#include "load-stg.hxx"
#include "load-btg.hxx"
#include "test-btg.hxx"
#include "palette.hxx"
#include "sprtf.hxx"

static const char *module = "test-btg";
static bool debug_on = false;
static const char *def_log = "tempbtg.txt";
static unsigned int options = 0;
static const char *usr_input = 0;
static const char *xg_file = 0;
static int verbosity = 0;
static const char *def_input = "X:\\fgdata\\Scenery\\Terrain\\w130n30\\w123n37\\942026.stg";
static const char *def_input2 = "X:\\fgdata\\Scenery\\Terrain\\w130n30\\w123n37\\KSFO.btg.gz";
static bool recursive = false;
static bool add_tri_bbox = false;

static vSTGS vInputs;

void give_help( char *name )
{
    SPRTF("\n%s: usage: [options] usr_input(s)\n", module);
    SPRTF("Options:\n");
    SPRTF(" --help  (-h or -?) = This help and exit(2)\n");
    SPRTF(" --verb[n]     (-v) = Bump or set verbosity. (def=%d)\n", verbosity);
    SPRTF(" --in <file>   (-i) = Input files list to process.\n");
    SPRTF(" --recurs      (-r) = Recursivly process sub-directories. (def=%s)\n",
        recursive ? "On" : "Off");
    SPRTF(" --xg <file>   (-x) = Output xg string to the file. (def=%s)\n",
        (xg_file && (options & opt_add_xg_text)) ? "On" : "Off" );
    SPRTF(" --bbox        (-b) = Add lights and triangle bbox to above xg file.\n");
    SPRTF("\n");
    SPRTF(" --out <html>  (-o) = Output palette html file, and exit. Only for debug.\n");
    SPRTF("\n");
    SPRTF(" In essence, given either a btg.gz file list, or a stg file, or even a\n");
    SPRTF(" directory, load and enumerate the contents of the btg.gz files found.\n\n");
    // TODO: More help
}

///////////////////////////////////////////////////////////
// This is really ONLY FOR DEBUG
/////////////////////////////////////////////////////////
void out_pal_html(const char *pal_file)
{
#if 0 // 0000000000000000000000000000000000000000
    const char *mat = "Freeway";
    std::string s = get_mat_color(mat);
    SPRTF("Mat %s color %s\n", mat, s.c_str());
    mat = "Stream";
    s = get_mat_color(mat);
    SPRTF("Mat %s color %s\n", mat, s.c_str());
    mat = "Grass";
    s = get_mat_color(mat);
    SPRTF("Mat %s color %s\n", mat, s.c_str());
#endif // 000000000000000000000000000000000000000
    std::string html = get_palette_html();
    FILE *fp = fopen(pal_file,"w");
    if (fp) {
        size_t res = fwrite(html.c_str(),1,html.size(),fp);
        fclose(fp);
        SPRTF("%s: Palette written to '%s'.\n", module, pal_file);
    } else {
        fwrite(html.c_str(),1,html.size(),stdout);
        SPRTF("%s: Failed to create '%s' file!\n", module, pal_file);
    }
}

#ifndef ISDIGIT
#define ISDIGIT(a) ((a >= '0') && (a <= '9'))
#endif

int is_btg_or_stg_file(const char *file)
{
    std::string f(file);
    size_t pos = f.rfind('.');
    if (pos > 0) {
        std::string ext = f.substr(pos);
        if (ext == ".stg") {
            return 1;
        } else if (ext == ".gz") {
            return 2;
        }
    }
    return 0;
}


bool is_btg_file(const char *file)
{
    std::string f(file);
    size_t pos = f.rfind('.');
    if (pos > 0) {
        std::string ext = f.substr(pos);
        if (ext == ".stg") {
            return false;
        } else if (ext == ".gz") {
            return true;
        }
    }
    return false;
}

void expand_dir( const char *dir )
{
    DIR *dp = opendir(dir);
    vSTGS dirs;
    int res;
    int count = 0;
    int tot_dirs = 0;
    int tot_files = 0;
    int tot_stg = 0;
    int tot_gz = 0;
    if (dp) {
        if (VERB5(verbosity)) {
            SPRTF("%s: Expanding directory '%s'\n", module, dir );
        }
        struct dirent* de = readdir(dp);
        while (de) {
            if (strcmp(de->d_name,".") && strcmp(de->d_name,"..")) {
                SGPath file(dir);
                file.append(de->d_name);
                DiskType dt = is_file_or_directory(file.c_str());
                if (dt == MDT_DIR) {
                    dirs.push_back(file.c_str());
                    tot_dirs++;
                } else if (dt == MDT_FILE) {
                    tot_files++;
                    res = is_btg_or_stg_file(file.c_str());
                    if (res) {
                        if (res == 1)
                            tot_stg++;
                        else if (res == 2)
                            tot_gz++;
                        if (!string_in_vec(vInputs,file.c_str())) {
                            vInputs.push_back(file.c_str());
                            if (VERB9(verbosity)) {
                                SPRTF("%s: Added file '%s'\n", module, file.c_str() );
                            }
                            count++;
                        }
                    }
                }
            }
            de = readdir(dp);
        }
        if (VERB2(verbosity)) {
            SPRTF("%s: Scan of '%s',\n found %d dirs, %d files, %d stg, %d gz, added %d to input.\n", module,
                dir, tot_dirs, tot_files, tot_stg, tot_gz, count );
        }
        if (recursive) {
            std::string s;
            size_t ii, max = dirs.size();
            for (ii = 0; ii < max; ii++) {
                s = dirs[ii];
                expand_dir( s.c_str() );
            }
        }
    } else {
        SPRTF("%s: Warning: Failed to open directory '%s'!\n", module, dir );
    }
}

int add_input(char *arg)
{
    SGPath sgp(arg);
#ifdef OLD_SIMGEAR
    std::string fp = sgp.realpath();
#else   // !#ifdef OLD_SIMGEAR
    std::string fp = sgp.realpath().str();
#endif  // #ifdef OLD_SIMGEAR y/n
    if (usr_input)
        free((void *)usr_input);
    usr_input = strdup(fp.c_str());
    DiskType dt = is_file_or_directory(usr_input);
    if (dt == MDT_DIR) {
        expand_dir(usr_input);
    } else if (dt == MDT_FILE) {
        if (!string_in_vec( vInputs, usr_input )) {
            vInputs.push_back(usr_input);
        }
    } else {
        SPRTF("%s: Unable to 'stat' file (or dir) '%s'! Aborting...\n", module, usr_input);
        return 1;
    }
    return 0;
}

int process_input_file( char *file )
{
    std::ifstream input(file);
    if (input.fail()) {
        SPRTF("%s: Unable to open input file '%s'!\n", module, file );
        return 1;
    }
    //std::string line;
    char buf[264];
    size_t len;
    // while( std::getline( input, line ) ) {
    // while (input.getline(line))
    while (input.getline(buf,256)) {
        len = strlen(buf);
        while (len) {
            len--;
            if (buf[len] > ' ')
                break;
            buf[len] = 0;
        }
        if (len) {
            //if (add_input((char *)line.c_str())) {
            if (add_input(buf)) {
                input.close();
                return 1;
            }
        }
    }
    input.close();
    return 0;
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
            case 'v':
                verbosity++;
                sarg++;
                while (*sarg) {
                    if (*sarg == 'v') {
                        verbosity++;
                    } else if (ISDIGIT(*sarg)) {
                        verbosity = atoi(sarg);
                        break;
                    }
                }
                if (VERB2(verbosity)) {
                    SPRTF("%s: Set verbosity to %d\n", module, verbosity);
                }
                break;
            case 'r':
                recursive = true;
                break;
            case 'x':
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    xg_file = strdup(sarg);
                    options |= opt_add_xg_text;
                    if (VERB2(verbosity)) {
                        SPRTF("%s: Added xg text opt to '%s'.\n", module, xg_file);
                    }
                } else {
                    SPRTF("%s: Error: Expected file name to follow %s! Aborting...", module,
                        arg );
                }
                break;
            case 'b':
                add_tri_bbox = true;
                break;
            case 'o':
                // mainly for debug
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    out_pal_html(sarg);
                    return 1;
                } else {
                    SPRTF("%s: Error: Expected file name to follow %s! Aborting...", module,  arg );
                    return 1;
                }
                break;
            case 'i':
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    if (process_input_file(sarg))
                        return 1;
                } else {
                    SPRTF("%s: Error: Expected file name to follow %s! Aborting...", module,  arg );
                    return 1;
                }
                break;
            // TODO: Other arguments
            default:
                SPRTF("%s: Unknown argument '%s'. Tyr -? for help...\n", module, arg);
                return 1;
            }
        } else {
            // bear argument
            if (add_input(arg))
                return 1;
        }
    }
    if (debug_on && !usr_input) {
        if (is_file_or_directory(def_input) == MDT_FILE) {
            vInputs.push_back(def_input);
        }
        if (is_file_or_directory(def_input2) == MDT_FILE) {
            vInputs.push_back(def_input2);
        }

    }
    i = (int)vInputs.size();
    if ( i == 0 ) {
        SPRTF("%s: No user input found in command!\n", module);
        return 1;
    } else {
        if (VERB2(verbosity)) {
            SPRTF("%s: Have %d inputs to process...\n", module, (int)i);
        }
    }
    return 0;
}

void do_log_file(char *name)
{
    char *cp = GetNxtBuf();
    *cp = 0;
#ifdef WIN32
    DWORD dwd = GetModuleFileName( NULL, cp, 256 );
    if (dwd == 256)
        strcpy(cp,name);
    size_t len = strlen(cp);
    char c;
    size_t i;
    size_t last;
    last = 0;
    for (i = 0; i < len; i++) {
        c = cp[i];
        if (( c == '/' ) || ( c == '\\' ))
            last = i + 1;
        if ( c == '/' )
            cp[i] = '\\';
    }
    cp[last] = 0;
#else 
    char *hm = getenv("HOME");
    if (hm) {
        strcpy(cp,hm);
        strcat(cp,"/");
    }
#endif
    strcat(cp,def_log);
    // SPRTF("Setting log file to '%s'\n",cp);
    set_log_file(cp,false);
}

// for ZLIB DLL load - now using static library if found
// PATH=X:\3rdParty.x64\bin;%PATH%
// samples: X:/fgdata/Scenery/Terrain/w130n30/w122n37/KSCK.btg.gz - points and tris
// main() OS entry
static MBBOX bb;
static MBBOX tri_bb;
static vSTGS mats;
int main( int argc, char **argv )
{
    size_t ii, max;
    int ret, iret = 0;
    do_log_file(argv[0]);
    iret = parse_args(argc,argv);
    if (iret)
        return iret;
    SGPath file(usr_input);
    load_stg *pls = new load_stg;
    load_btg *plb = new load_btg;
    PMOPTS pmo = new MOPTS;
    pmo->options = options;
    pmo->verb = verbosity;
    std::string xg;
    init_mbbox(bb);
    init_mbbox(tri_bb);
    max = vInputs.size();
    double bgn = get_seconds();
    for (ii = 0; ii < max; ii++) {
        file = vInputs[ii];
        if (string_in_vec( pmo->done, file.file().c_str()))
            continue;
        if (is_btg_file(file.c_str())) {
            ret = plb->load(file, pmo);
            iret |= ret;
            if (ret == 0) {
                merge_mbbox(bb,plb->bb);
                merge_mbbox(tri_bb,plb->tri_bb);
                plb->add_mats(mats);
            }
        } else {
            ret = pls->load(file, pmo); // assume a STG file
            iret |= ret;
            if (ret == 0) {
                merge_mbbox(bb,pls->bb);
                merge_mbbox(tri_bb,pls->tri_bb);
                pls->add_mats(mats);
            }
        }
        xg += pmo->xg;
    }
    ii = pmo->done.size();
    SPRTF("%s: Done %u of %u inputs, BBox %s, in %s secs\n", module, (int)ii, (int)max,
        get_mbbox_stg(&bb),
        get_elapsed_stg(bgn));
    if (VERB2(verbosity)) {
        max = mats.size();
        if (max) {
            SPRTF("%s: Saw %d mats ", module, (int)max);
            for (ii = 0; ii < max; ii++) {
                SPRTF("%s ", mats[ii].c_str());
            }
            SPRTF("\n");
        }
    }
    ///////////////////////////////////////////////////////////////////////////
    if (xg_file && (options & opt_add_xg_text) && xg.size()) {
        FILE *fp = fopen(xg_file,"w");
        if (fp) {
            size_t res = 1;
            if (add_tri_bbox) {
                char *cp = get_mbbox_xg_stg( &tri_bb );
                if (cp) {
                    std::string msg("color = green\n");
                    msg += cp;
                    msg += "NEXT\n";
                    res = fwrite( msg.c_str(), msg.size(), 1, fp );
                }
            }
            if (res == 1)
                res = fwrite( xg.c_str(), xg.size(), 1, fp );
            fclose(fp);
            if (res != 1) {
                SPRTF("%s: Failed to write xg to '%s'!\n", module, xg_file );
            } else {
                SPRTF("%s: Written xg file '%s'\n", module, xg_file);
            }
        } else {
            SPRTF("%s: Failed to open xg to '%s'!\n", module, xg_file );
        }
    }
    delete pls;
    delete plb;
    delete pmo;
    if (VERB5(verbosity)) {
        outout_mat_color_counts();
    }
    close_palette();
    return iret;
}


// eof = test-btg.cxx
