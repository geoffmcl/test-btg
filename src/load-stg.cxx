/*\
 * load-stg.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <sstream>
#include <fstream>
#include "load-btg.hxx"
#include "load-stg.hxx"
#include "sprtf.hxx"
#include "sprtfstr.hxx"

static const char *module = "load-stg";

load_stg::load_stg()
{
}

load_stg::~load_stg()
{
}

int load_stg::load( SGPath file, PMOPTS po )
{
    int res, iret = 0;
    int tot_objs = 0;
    int loaded = 0;
    int already = 0;
    int entries = 0;
    std::string buf;
    int verb = 0;
    unsigned int options = 0;
    if (po) {
        verb = po->verb;
        options = po->options;
        std::string fn = file.file();
        if (string_in_vec(po->done,fn.c_str())) 
            return 0;
        po->done.push_back(fn);
    }
    std::ifstream in(file.c_str());
    if (in.bad()) {
        SPRTF("%s: Failed to open file '%s'\n", module, file.c_str());
        return 1;
    }

    SPRTF("\n%s: Processing STG file '%s'\n", module, file.c_str());
    load_btg *pbtg = 0;
    init_mbbox(bb);
    init_mbbox(tri_bb);
    mats.clear();
    double bgn = get_seconds();
    std::string xg;
    while (getline(in, buf)) {
    	std::istringstream str(buf);
	    std::string type, data;
    	str >> type >> data;
        entries++;
	    if ((type == "OBJECT_BASE") || (type == "OBJECT")) {
            tot_objs++;
            if (verb) {
                SPRTF("%s: type '%s' data '%s'\n", module, type.c_str(), data.c_str());
            }
            SGPath btg(file.dir());
            btg.append(data);
            btg.concat(".gz");  // TODO: Is this ALWAYS TRUE?????????
            if (!pbtg)
                pbtg = new load_btg;
            res = pbtg->load(btg,po);
            if (res == btg_doneok) {
                loaded++;
                merge_mbbox(bb, pbtg->bb);
                merge_mbbox(tri_bb, pbtg->tri_bb);
                pbtg->add_mats( mats );
                if (po && (options & opt_add_xg_text)) {
                    xg += po->xg;
                }
            } else if (res == btg_repeat) {
                already++;
            } else {
                iret |= 1;
            }
        }
    }
    in.close();

    // SPRTF("%s: Loaded %d objs, BBOX %s, in %s secs.\n", module, tot_objs, get_mbbox_stg(&bb), get_elapsed_stg(bgn));
    const char *cp = po->stg.printf("%s: Found %d ents, %d objs", module, entries, tot_objs);
    if (already)
        cp = po->stg.appendf(", %d repeats", already);
    if (pbtg) {
        res = pbtg->add_mats( mats );
        if (res)
            cp = po->stg.appendf(", mats %d", res);
    }
    if (loaded) {
        cp = po->stg.appendf(", %d loaded, bbox %s, in %s secs", loaded, get_mbbox_stg(&bb), get_elapsed_stg(bgn));
        if (po && (options & opt_add_xg_text)) {
            po->xg = xg;
        }
    }
    SPRTF("%s\n",cp);

    if (pbtg)
        delete pbtg;
    return iret;
}

int load_stg::add_mats( vSTGS &m )
{
    int count = 0;
    size_t ii, max = mats.size();
    std::string s;
    for (ii = 0; ii < max; ii++) {
        s = mats[ii];
        if (!string_in_vec(m, s.c_str())) {
            count++;
            m.push_back(s);
        }
    }
    return count;
}


// eof = load-stg.cxx
