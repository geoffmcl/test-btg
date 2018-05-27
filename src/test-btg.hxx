/*\
 * test-btg.hxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _TEST_BTG_HXX_
#define _TEST_BTG_HXX_
#include <string>
#include <vector>
#include "utils.hxx"
#include "sprtfstr.hxx"

#define opt_show_wgs84_nodes    0x00000001

#define opt_add_xg_text         0x00000010
#define opt_add_xg_bbox         0x00000020
#define opt_add_csv_text        0x00000040


typedef struct tagMOPTS {
    int verb;
    unsigned int options;
    std::string xg;
    std::string csv;    // comma separated list
    vSTGS done;
    sprtfstr stg;
}MOPTS, *PMOPTS;


#endif // #ifndef _TEST_BTG_HXX_
// eof - test-btg.hxx
