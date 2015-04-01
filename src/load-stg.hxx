/*\
 * load-stg.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _LOAD_STG_HXX_
#define _LOAD_STG_HXX_
#include <simgear/misc/sg_path.hxx> // SGPath
#include "utils.hxx"
#include "test-btg.hxx"

class load_stg {
public:
    load_stg();
    ~load_stg();

    int load( SGPath file, PMOPTS po );

    MBBOX bb;

};



#endif // #ifndef _LOAD_STG_HXX_
// eof - load-stg.hxx
