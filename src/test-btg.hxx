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

typedef struct tagMOPTS {
    int verb;
    unsigned int options;
    std::string xg;
    vSTGS done;
    sprtfstr stg;
}MOPTS, *PMOPTS;


#endif // #ifndef _TEST_BTG_HXX_
// eof - test-btg.hxx
