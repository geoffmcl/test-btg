/*\
 * load-btg.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _LOAD_BTG_HXX_
#define _LOAD_BTG_HXX_
#include <simgear/misc/sg_path.hxx> // SGPath
#include <simgear/io/sg_binobj.hxx> // SGBinObject
#include "utils.hxx"
#include "test-btg.hxx"

// return values
#define btg_doneok  0
#define btg_nostat  1
#define btg_noload  2
#define btg_repeat  -1

class load_btg {
public:
    load_btg();
    ~load_btg();

    int load( SGPath file, PMOPTS po );
    MBBOX bb;
};


#endif // #ifndef _LOAD_BTG_HXX_
// eof - load-btg.hxx
