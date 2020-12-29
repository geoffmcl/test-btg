/*\
 * load-btg.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include "sprtf.hxx"
#include "sprtfstr.hxx"
#include "utils.hxx"
#include "palette.hxx"
#include "load-btg.hxx"

#ifndef SPRTF
#define SPRTF printf
#endif

static const char *module = "load-btg";

// 
// The scenery of http://wiki.flightgear.org/FlightGear, built by 
// http://wiki.flightgear.org/TerraGear - https://sourceforge.net/p/flightgear/terragear/ci/master/tree/
// Documentation on the BTG file format can be found at:
// http://wiki.flightgear.org/index.php/BTG_File_Format
// The primary calss is http://api-docs.freeflightsim.org/simgear/classSGBinObject.html
// See http://api-docs.freeflightsim.org/simgear/sg__binobj_8cxx.html
// And http://api-docs.freeflightsim.org/simgear/sg__binobj_8hxx.html
//
//
// implementation
load_btg::load_btg()
{
    init_mbbox(bb);
    init_mbbox(tri_bb);
}

load_btg::~load_btg()
{
}

int load_btg::load( SGPath file, PMOPTS po )
{
    size_t v, n, k, i, j, kn, m;
    size_t ui, u0, u1, u2;
    double lat,lon,elev;
    SGVec3d gbs_p, nd, node;
    SGGeod geodCent, geod;
    SGBinObject _chunk;
    SGBucket bucket;
    int verb = 0;
    unsigned int options = 0;
    sprtfstr stg;
    const char *cp;
    bool collect_xg = false;
    std::string s;
    std::string first;
    if (po) {
        verb = po->verb;
        options = po->options;
        collect_xg = (options & opt_add_xg_text) ? true : false;
        po->xg = "";    // start XG string
        s = file.file();
        if (string_in_vec(po->done,s.c_str())) 
            return btg_repeat;
        po->done.push_back(s);
    }
    if (is_file_or_directory(file.c_str()) !=  MDT_FILE) {
        SPRTF("\n%s: Unable to 'stat' file '%s'\n", module, file.c_str());
        return btg_nostat;
    }
    long long fs = get_last_file_size();
    SPRTF("\n%s: Loading file '%s', %s bytes.\n", module, file.c_str(), get_nice_number64(fs));
    double bgn = get_seconds();
#ifdef OLD_SIMGEAR
    if (!_chunk.read_bin(file.c_str())) {
    	return btg_noload;
    }
#else // !#ifdef OLD_SIMGEAR
    if (!_chunk.read_bin(file)) {
        return btg_noload;
    }
#endif // #ifdef OLD_SIMGEAR
    unsigned short us = _chunk.get_version(); // actually unsigned short
    SPRTF("%s: Loaded BTG v%u, in %s secs...\n", module, (unsigned short)us, get_elapsed_stg(bgn));
    init_mbbox(bb);
    init_mbbox(tri_bb);
    mats.clear();
    const std::vector<SGVec3<double> >& wgs84_nodes = _chunk.get_wgs84_nodes();

    //const SGVec3<double>& gbs_p = _chunk.get_gbs_center();
    gbs_p = _chunk.get_gbs_center();
    geodCent = SGGeod::fromCart(gbs_p);
    bucket.set_bucket(geodCent);

    lat = geodCent.getLatitudeDeg();
    lon = geodCent.getLongitudeDeg();
    elev = geodCent.getElevationM();
    SPRTF("%s: Center %lf,%lf,%lf (%lf,%lf,%lf)\n", module, lon, lat, elev, gbs_p[0], gbs_p[1], gbs_p[2]);
    if (collect_xg) {
        cp = stg.printf("# %s: Center %lf,%lf,%lf\n", file.c_str(), lon, lat, elev);
        po->xg += cp;
    }
    set_mbbox(bb,lat,lon,elev);

    SPRTF("%s: Bucket: %s/%d\n", module,
        bucket.gen_base_path().c_str(),
        bucket.gen_index());

    kn = wgs84_nodes.size();
    n = _chunk.get_normals().size();
    SPRTF("%s: wgs84 nodes %u, normals %u\n", module, (int)kn, (int)n);
    if (kn) {
        if (VERB9(verb) && (options & opt_show_wgs84_nodes)) {
            SPRTF("latitude,longitude,elevation (cartesian co-ords)\n");
        }
    }
    for (ui = 0; ui < kn; ui++) {
        nd = wgs84_nodes[ui];
    	node = ( nd + gbs_p);
        geod = SGGeod::fromCart(node);
        lat = geod.getLatitudeDeg();
        lon = geod.getLongitudeDeg();
        elev = geod.getElevationM();
        if (VERB9(verb) && (options & opt_show_wgs84_nodes)) {
            SPRTF("%lf,%lf,%lf (%lf,%lf,%lf)\n", lat, lon, elev, nd[0], nd[1], nd[2] );
        }
        set_mbbox(bb,lat,lon,elev);
    }
    SPRTF("%s: BBOX %s\n", module, get_mbbox_stg(&bb));
    if (collect_xg && (options & opt_add_xg_bbox)) {
        char *cp = get_mbbox_xg_stg( &bb );
        if (cp) {
            po->xg += "color = green\n";
            po->xg += cp;
            po->xg += "NEXT\n";
        }
    }
    //const std::vector<SGVec4f>& get_colors() const { return colors; }
    v = _chunk.get_colors().size();
    //const std::vector<SGVec2f>& get_texcoords() const { return texcoords; }
    n = _chunk.get_texcoords().size();
    SPRTF("%s: Colors %u, texcoords %u\n", module, (int)v, (int)n );

    // seems these are the airport LIGHTS?
    v = _chunk.get_pts_v().size();
    n = _chunk.get_pts_n().size();
    if (v || VERB5(verb)) {
        SPRTF("%s: points v %u, n %u %s\n", module, (int)v, (int)n,
            ((v == n) ? "" : "***CHECK ME!***"));
        if (v && collect_xg)
            po->xg += "color = white\n";
        for (i = 0; i < v; i++) {
            int_list il = _chunk.get_pts_v().at(i);
            k = il.size();
            for (j = 0; j < k; j++) {
                ui = il.at(j);
                if (ui < kn) {
                    nd = wgs84_nodes[ui];
    	            node = ( nd + gbs_p);
                    geod = SGGeod::fromCart(node);
                    lat = geod.getLatitudeDeg();
                    lon = geod.getLongitudeDeg();
                    elev = geod.getElevationM();
                    set_mbbox(tri_bb,lat,lon,elev);
                    cp = stg.printf("%lf %lf ; %lf\nNEXT\n", lon, lat, elev );
                    if (VERB9(verb)) {
                        SPRTF("%s",cp);
                    }
                    if (collect_xg) {
                        po->xg += cp;
                    }
                } else {
                    SPRTF("; %s: Trouble: Index %u out of range %u\n", module,
                        (int)ui, (int)kn );
                }
            }
            //if (po && (options & opt_add_xg_text)) {
            //    if (VERB9(verb)) {
            //        SPRTF("NEXT\n");
            //    }
            //    po->xg += "NEXT\n";
            //}
        }
    }

    // Terrain/Airport tris
    v = _chunk.get_tris_v().size();
    n = _chunk.get_tris_n().size();
    const string_list& tri_mats = _chunk.get_tri_materials();
    m = tri_mats.size();
    if (v || n || m || VERB5(verb)) {
        SPRTF("%s: tris v %u, n %u mats %u %s\n", module, (int)v, (int)n, (int)m,
            ((v == n) ? "" : "***CHECK ME!***"),
            ((v == m) ? "" : "***CHECK MATS!***"));
        if (v && collect_xg) {
            if (!m) {
                po->xg += "color = yellow\n"; // WHAT! no materials - just use yellow
            }
        }
        for (i = 0; i < v; i++) {
            int_list il = _chunk.get_tris_v().at(i);
            s = "";
            if (i < m) {
                s = tri_mats[i];
                if (!string_in_vec(mats,s.c_str())) {
                    if (!is_mat_in_list(s.c_str())) {
                        SPRTF("%s: Warning: material '%s' MISSING! *** FIX ME ***\n", module, s.c_str());
                        // mats.push_back("MISSING");
                    }
                    mats.push_back(s);
                }
                s = get_mat_color(s.c_str());
                po->xg += "color = ";
                po->xg += s;
                po->xg += "\n";
            }
            k = il.size();  // this should ALWAYS be 3, but
            first = "";
            for (j = 0; j < k; j++) {
                ui = il.at(j);
                if (ui < kn) {
                    nd = wgs84_nodes[ui];
    	            node = ( nd + gbs_p);
                    geod = SGGeod::fromCart(node);
                    lat = geod.getLatitudeDeg();
                    lon = geod.getLongitudeDeg();
                    elev = geod.getElevationM();
                    cp = stg.printf("%lf %lf ; %lf %s\n", lon, lat, elev, s.c_str() );
                    if (VERB9(verb)) {
                        SPRTF("%s",cp);
                    }

                    if (collect_xg) {
                        po->xg += cp;
                        if (j == 0)
                            first = cp; // keep first point to close polygon (tringles here)
                    }

                    // add k terrain/airport tris to csv text
                    if (options & opt_add_csv_text) {
                        if (k == 3) {
                            po->csv += cp;
                        }
                        else {
                            static int dn_warn = 0;
                            if (!dn_warn)
                                SPRTF("; %s: Trouble: tri not set of 3 = %u\n", module, (int)k);
                            dn_warn++;
                        }
                    }

                    set_mbbox(tri_bb,lat,lon,elev);
                } else {
                    SPRTF("; %s: Trouble: Index %u out of range %u\n", module,
                        (int)ui, (int)kn );
                    break;
                }
            }
            if (collect_xg) {
                if (VERB9(verb)) {
                    SPRTF("NEXT\n");
                }
                if (first.size())
                    po->xg += first;
                po->xg += "NEXT\n";
            }
        }
    }

    // show materials
    i = mats.size();
    if (m && i && VERB2(verb)) {
        cp = stg.printf("%s: mats %u ", module, (int)i);
        for (m = 0; m < i; m++) {
            s = mats[m];
            cp = stg.appendf("%s ", s.c_str());
        }
        SPRTF("%s\n",cp);
    }

    // Terrain/Airport strips
    v = _chunk.get_strips_v().size();
    n =_chunk.get_strips_n().size();
    if (v || VERB5(verb)) {
        SPRTF("%s: strips v %u, n %u %s\n", module, (int)v, (int)n,
            ((v == n) ? "" : "***CHECK ME!***"));
        if (v) {
            SPRTF("%s: TODO: Output of 'strips' not yet coded!\n", module);
        }
    }

    // Terrain/Airport fans
    v = _chunk.get_fans_v().size();
    n = _chunk.get_fans_n().size();
    if (v || VERB5(verb)) {
        SPRTF("%s: fans v %u, n %u %s\n", module, (int)v, (int)n,
            ((v == n) ? "" : "***CHECK ME!***"));
        for (i = 0; i < v; i++) {
            // int_list il = _chunk.get_tris_v().at(i);
            int_list il = _chunk.get_fans_v().at(i);
            s = "";
            s = _chunk.get_fan_materials()[i];
            if (s.size()) {
                if (!string_in_vec(mats, s.c_str())) {
                    if (!is_mat_in_list(s.c_str())) {
                        SPRTF("%s: Warning: material '%s' MISSING! *** FIX ME ***\n", module, s.c_str());
                        // mats.push_back("MISSING");
                    }
                    mats.push_back(s);
                }
                s = get_mat_color(s.c_str());
                po->xg += "color = ";
                po->xg += s;
                po->xg += "\n";
            }
            k = il.size();  // this should ALWAYS be 3, or more, but
            if (k < 3) {
                continue;
            }

            // get first two offsets into wsg84 points - u0 u1
            u0 = il.at(0);
            if (u0 >= kn) {
                SPRTF("; %s: Trouble: Index %u out of range %u\n", module,
                    (int)u0, (int)kn);
                continue;
            }
            u1 = il.at(1);
            if (u1 >= kn) {
                SPRTF("; %s: Trouble: Index %u out of range %u\n", module,
                    (int)u1, (int)kn);
                continue;
            }
            // Have 2 offsets, so start at third
            for (j = 2; j < k; j++) {
                u2 = il.at(j);
                if (u2 < kn) {
                    // add three points of this fan
                    nd = wgs84_nodes[u0];
                    node = (nd + gbs_p);
                    geod = SGGeod::fromCart(node);
                    lat = geod.getLatitudeDeg();
                    lon = geod.getLongitudeDeg();
                    elev = geod.getElevationM();
                    set_mbbox(tri_bb, lat, lon, elev);
                    cp = stg.printf("%lf %lf ; %lf %s\n", lon, lat, elev, s.c_str());
                    if (VERB9(verb)) {
                        SPRTF("%s", cp);
                    }
                    if (collect_xg) {
                        po->xg += cp;
                        first = cp;
                    }
                    // 1 add terrain/airport tris to csv text
                    if (options & opt_add_csv_text)
                        po->csv += cp;

                    nd = wgs84_nodes[u1];
                    node = (nd + gbs_p);
                    geod = SGGeod::fromCart(node);
                    lat = geod.getLatitudeDeg();
                    lon = geod.getLongitudeDeg();
                    elev = geod.getElevationM();
                    set_mbbox(tri_bb, lat, lon, elev);
                    cp = stg.printf("%lf %lf ; %lf %s\n", lon, lat, elev, s.c_str());
                    if (VERB9(verb)) {
                        SPRTF("%s", cp);
                    }
                    if (collect_xg) {
                        po->xg += cp;
                    }
                    // 2 add terrain/airport tris to csv text
                    if (options & opt_add_csv_text)
                        po->csv += cp;

                    nd = wgs84_nodes[u2];
                    node = (nd + gbs_p);
                    geod = SGGeod::fromCart(node);
                    lat = geod.getLatitudeDeg();
                    lon = geod.getLongitudeDeg();
                    elev = geod.getElevationM();
                    set_mbbox(tri_bb, lat, lon, elev);
                    cp = stg.printf("%lf %lf ; %lf %s\n", lon, lat, elev, s.c_str());
                    if (VERB9(verb)) {
                        SPRTF("%s", cp);
                    }
                    if (collect_xg) {
                        po->xg += cp;
                        po->xg += first;
                        po->xg += "NEXT\n";
                    }
                    // 3 add terrain/airport tris to csv text
                    if (options & opt_add_csv_text)
                        po->csv += cp;
                }
                else {
                    SPRTF("; %s: Trouble: Index %u out of range %u\n", module,
                        (int)u2, (int)kn);
                }
                u1 = u2;    // update the 2nd with third point
            }
        }
    }
    return btg_doneok;
}

int load_btg::add_mats( vSTGS &m )
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

// eof = load-btg.cxx
