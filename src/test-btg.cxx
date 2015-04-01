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
#ifdef WIN32
#include <Windows.h>
#endif
#include <dirent.h>
#include "load-stg.hxx"
#include "load-btg.hxx"
#include "test-btg.hxx"
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

static vSTGS vInputs;

void give_help( char *name )
{
    printf("%s: usage: [options] usr_input(s)\n", module);
    printf("Options:\n");
    printf(" --help  (-h or -?) = This help and exit(2)\n");
    printf(" --verb[n]     (-v) = Bump or set verbosity. (def=%d)\n", verbosity);
    printf(" --recurs      (-r) = Recursivly process sub-directories. (def=%s)\n",
        recursive ? "On" : "Off");
    printf(" --xg <file>   (-x) = Output xg string to the file. (def=%s)\n",
        (xg_file && (options & opt_add_xg_text)) ? "On" : "Off" );
    printf("\n");
    printf(" In essence, given either a btg.gz file list, or a stg file, or even a\n");
    printf(" directory, load and enumerate the contents of the btg.gz files found.\n");
    // TODO: More help
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
            // TODO: Other arguments
            default:
                SPRTF("%s: Unknown argument '%s'. Tyr -? for help...\n", module, arg);
                return 1;
            }
        } else {
            // bear argument
            //if (usr_input) {
            //    printf("%s: Already have input '%s'! What is this '%s'?\n", module, usr_input, arg );
            //    return 1;
            //}
            SGPath sgp(arg);
            std::string fp = sgp.realpath();
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
    // printf("Setting log file to '%s'\n",cp);
    set_log_file(cp,false);
}

// for ZLIB DLL load - now using static library if found
// PATH=X:\3rdParty.x64\bin;%PATH%
// samples: X:/fgdata/Scenery/Terrain/w130n30/w122n37/KSCK.btg.gz - points and tris
// main() OS entry
static MBBOX bb;
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
            }
        } else {
            ret = pls->load(file, pmo); // assume a STG file
            iret |= ret;
            if (ret == 0) {
                merge_mbbox(bb,pls->bb);
            }
        }
        xg += pmo->xg;
    }
    ii = pmo->done.size();
    SPRTF("%s: Done %u of %u inputs, BBox %s, in %s secs\n", module, (int)ii, (int)max,
        get_mbbox_stg(&bb),
        get_elapsed_stg(bgn));
    if (xg_file && (options & opt_add_xg_text) && xg.size()) {
        FILE *fp = fopen(xg_file,"w");
        if (fp) {
            size_t res = fwrite( xg.c_str(), xg.size(), 1, fp );
            fclose(fp);
            if (res != 1) {
                SPRTF("%s: Failed to write xg to '%s'!\n", module,
                    xg_file );
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
    return iret;
}


// eof = test-btg.cxx
