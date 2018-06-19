// vpf-topology.cxx - program to dump a topology to output.
// This file is released into the Public Domain, and comes with NO WARRANTY!

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;

#include <string>
using std::string;
#ifndef _MSC_VER
#include <stdlib.h> // for atoi(), ...
#endif

#include "vpf.hxx"

static bool gen_color = false;
static bool do_trim = false;
static bool add_index = false;
static size_t next_index = 0;
static int next_color = 0;
static VpfRectangle ubounds;

static int check_me()
{
    int i;
    i = 2;
    return i;
}

ostream &
operator<< (ostream &output, const VpfRectangle &rect)
{
  output << rect.minX << ','
	 << rect.minY << ','
	 << rect.maxX << ','
	 << rect.maxY;
  return output;
}

static bool
point_in_bounds( const VpfPoint &p )
{
    if ((p.x >= ubounds.minX) &&
        (p.x <= ubounds.maxX) &&
        (p.y >= ubounds.minY) &&
        (p.y <= ubounds.maxY) ) 
    {
        return true;
    }
    return false;
}

static void
dump_point (const VpfPoint &p)
{
    cout.precision(15);
    cout << p.x << ' ' << p.y;
    if (add_index) {
        cout << " # " << next_index;
        next_index++;
    }
    cout << endl;
}

static void
dump_point (const VpfPoint &p, const VpfRectangle &bounds)
{

  if (inside(p, bounds)) {
      cout << "# point" << endl;
    dump_point(p);
    cout << endl;
  }
}

// unconditional - dump this whole line
static void
dump_line (const VpfLine &l)
{
    int i, nPoints = l.getPointCount();
    for (i = 0; i < nPoints; i++) {
        dump_point(l.getPoint(i));
    }
}

static bool
any_point_in_bounds(const VpfLine &l, const VpfRectangle &bounds)
{
    int nPoints = l.getPointCount();
    for (int i = 0; i < nPoints; i++) {
        VpfPoint p = l.getPoint(i);
        if ( (p.x < bounds.minX) ||
             (p.x > bounds.maxX) ||
             (p.y < bounds.minY) ||
             (p.y > bounds.maxY) ) 
        {
           continue;
        }
        return true;    // found a point within bounds
    }
    return false;
}

typedef struct tagsPt {
    double x,y,z;
    bool inbnds;
}sPt, *PsPt;

typedef std::vector<sPt> vPTS;


static void
dump_line (const VpfLine &l, const VpfRectangle &bounds)
{
    int cnt = 0;
    VpfRectangle rect = l.getBoundingRectangle();
    next_index = 0;
    if (overlap(rect, bounds)) {
        // check if ANY point on this line is within the bounds
        if (any_point_in_bounds( l, bounds )) {
            if (do_trim) {
                // only add successive points if the point, or any point before or after it, is within the bounds
                vPTS vPts;
                sPt pt, ppt;
                bool had_in = false;
                int i, nPoints = l.getPointCount();
                size_t ii, max, last_in = 0;
                VpfPoint p;
                for (i = 0; i < nPoints; i++) {
                    p = l.getPoint(i);
                    if ( (p.x < bounds.minX) ||
                         (p.x > bounds.maxX) ||
                         (p.y < bounds.minY) ||
                         (p.y > bounds.maxY) ) 
                    {
                        pt.inbnds = false;
                    } else {
                        pt.inbnds = true;
                    }
                    pt.x = p.x;
                    pt.y = p.y;
                    if (had_in || pt.inbnds) {
                        if (!had_in && i) {
                            // if the first within, and had previous, also add previous
                            vPts.push_back(ppt);
                        }
                        vPts.push_back(pt);
                        had_in = true;
                        if (pt.inbnds) {
                            last_in = vPts.size();  // keep index+1 of last point in bounds
                        }
                    }
                    ppt = pt;
                }
                // above would have dropped any leading points that were out of bounds
                // now must check the trailing point
                max = vPts.size();
                if (max && last_in) {
                    if (max == last_in)
                        last_in--;
                    if (gen_color) {
                        next_color++;
                        cout << "color " << next_color << endl;
                    }
                    for (ii = 0; ii <= last_in; ii++) {
                        pt = vPts[ii];
                        p.x = pt.x;
                        p.y = pt.y;
                        dump_point(p);
                        cnt++;
                    }
                }
            } else {
                if (gen_color) {
                    next_color++;
                    cout << "color " << next_color << endl;
                }
                dump_line(l);
                cnt++;
            }
      }
  }
  if (cnt) {
      cout << "NEXT" << endl;
      next_index = 0;
  }
}

static void
dump_contour (const VpfContour &c)
{
    int nPoints = c.getPointCount();
    cerr << "# dumping contour with " << nPoints << " points" << endl;
    for (int i = 0; i < nPoints; i++) {
        dump_point(c.getPoint(i));
    }
}

static void
dump_polygon (const VpfPolygon &poly)
{
    int nContours = poly.getContourCount();
    for (int i = 0; i < nContours; i++) {
        VpfContour c = poly.getContour(i);
        int nPoints = c.getPointCount();
        if (nPoints >= 3) {
             if (gen_color) {
                 next_color++;
                 cout << "color " << next_color << endl;
             }
            dump_contour(c);
            cout << "NEXT" << endl;
            next_index = 0;
        } else {
            cerr << "# contour with less than 3 points! " << nPoints << endl;
        }
    }
    // cout << endl;
}

static bool poly_in_bounds( const VpfPolygon &poly, const VpfRectangle &bounds )
{
    int i, j, nPoints, nContours = poly.getContourCount();
    for (i = 0; i < nContours; i++) {
        VpfContour c = poly.getContour(i);
        nPoints = c.getPointCount();
        for (j = 0; j < nPoints; j++) {
            VpfPoint p = c.getPoint(j);
            if ( (p.x < bounds.minX) ||
                 (p.x > bounds.maxX) ||
                 (p.y < bounds.minY) ||
                 (p.y > bounds.maxY) ) 
            {
               return false;    // takes just ONE point outside boands to reject whole poly
            }
        }
    }
    return true;
}

static void
dump_polygon (const VpfPolygon &poly, const VpfRectangle &bounds)
{
    if (overlap(poly.getBoundingRectangle(), bounds)) {
        if (!do_trim || poly_in_bounds( poly, bounds )) {
            dump_polygon(poly);
        } else if (do_trim) {
            int nPoints = 0;
            int i, nContours = poly.getContourCount();
            for (i = 0; i < nContours; i++) {
                VpfContour c = poly.getContour(i);
                nPoints += c.getPointCount();
            }
            cerr << "# due to 'trim' skipping polygon with " << nPoints << " points." << endl;
        }
    }
}

static void
dump_label (const VpfLabel &label)
{
  const VpfPoint p = label.getPoint();
  cout << p.x << ' ' << p.y << ' ' << label.getText() << endl;
}

static void
dump_label (const VpfLabel &label, const VpfRectangle &bounds)
{
  if (inside(label.getPoint(), bounds))
    dump_label(label);
}

void show_usage( char *name )
{
    const char *ind = "   ";
    cerr << endl;
    cerr << ind  << "Usage: " << name << " [Options] <database dir> <library> <coverage> <feature>" << endl;
    cerr << endl;
    cerr << ind << "Options: Default bounds -180 180 90 -90" << endl;
    cerr << ind << "  --minx=<degs>  - Set minimum longitude." << endl;
    cerr << ind << "  --maxx=<degs>  - Set maximum longitude." << endl;
    cerr << ind << "  --maxy=<degs>  - Set maximum latitude." << endl;
    cerr << ind << "  --miny=<degs>  - Set minimum latitude." << endl;
    cerr << ind << "  --index        - Add index as trailing comment to point." << endl;
    cerr << ind << "  --color        - Generate a color sequence." << endl;
    cerr << ind << "  --trim         - Agressively trim a line to be within the bounds." << endl;
    cerr << ind << "  For a polygon, like say veg treesa, the whole polygon must be in bounds.\n" << endl;
    cerr << endl;
    cerr << ind << "Examples:" << endl;
    cerr << ind << "--minx=148 --maxx=149 --maxy=-32 --miny=-31 v0sas_5\\vmaplv0 sasaus trans roadl 2>nul" << endl;
    cerr << ind << "  to output the road (lines) around Gilgandra, Australia. Or" << endl;
    cerr << ind << "--trim --minx=147 --maxx=150 --maxy=-32 --miny=-31 D:\\VMAP\\v0sas_5\\vmaplv0 sasaus hydro watrcrsl > ygil.xg" << endl;
    cerr << ind << "  to output the river/stream (lines) around Gilgandra, Australia." << endl;
    cerr << endl;
    cerr << ind << "Data information is output to stdout, and all other message to stderr" << endl;

}

int main (int ac, char ** av)
{
    int nTopologies = 0;
    ubounds.minX = -180.0;
    ubounds.minY = -90.0;
    ubounds.maxX = 180.0;
    ubounds.maxY = 90.0;
    int args_start = 1;
    if (ac < 5) {
        show_usage(av[0]);
        cerr << "Need a minimum of 4 arguments! Got " << (ac - args_start) << "! Aborting..." << endl;
        return 2;
    }

    for (int i = 1; i < ac; i++) {
        string opt = av[i];
        if (opt.find("--minx=") == 0) {
            ubounds.minX = atof(opt.substr(7).c_str());
        } else if (opt.find("--miny=") == 0) {
            ubounds.minY = atof(opt.substr(7).c_str());
        } else if (opt.find("--maxx=") == 0) {
            ubounds.maxX = atof(opt.substr(7).c_str());
        } else if (opt.find("--maxy=") == 0) {
            ubounds.maxY = atof(opt.substr(7).c_str());
        } else if (opt.find("--color") == 0) {
            gen_color = true;
        } else if (opt.find("--trim") == 0) {
            do_trim = true;
        } else if (opt.find("--index") == 0) {
            add_index = true;
            next_index = 0;
        } else if (opt.find("--") == 0) {
            show_usage(av[0]);
            cerr << "Unrecognized option: " << opt << "! Aborting..." << endl;
            return 2;
        } else {
            break;
        }
        args_start++;
    }
  
    if (ac - args_start != 4) {
        show_usage(av[0]);
        cerr << "invalid argument count! Expected 4, got " << (ac - args_start) << "! Aborting..." << endl;
        return 2;
    }

    cerr << "# Bounds: " << ubounds << endl;
    if ((ubounds.maxX <= ubounds.minX)||
        (ubounds.maxY <= ubounds.minY)) {
        if (ubounds.maxX <= ubounds.minX) {
            cerr << "err: maxx LSE minx! ";
        }
        if (ubounds.maxY <= ubounds.minY) {
            cerr << "err: maxy LSE miny! ";
        }
        cerr << "fix bounds! Aborting..." << endl;
        return 1;
    }

    try {

        VpfDataBase db(av[args_start]);
        VpfLibrary library = db.getLibrary(av[args_start+1]);
        VpfFeature feature = library.getCoverage(av[args_start+2]).getFeature(av[args_start+3]);
        
        const VpfRectangle rect = library.getBoundingRectangle();
        if (!overlap(rect, ubounds)) {
            cerr << "Library coverage does not overlap with area" << endl;
            cerr << "Library: " << rect << endl;
            cerr << "Requested: " << ubounds << " Aborting..." << endl;
            return 1;
        }

        nTopologies = feature.getTopologyCount();
        int type = feature.getTopologyType();
        cerr << "# Searching through " << nTopologies << " topologies" << endl;
        for (int i = 0; i < nTopologies; i++) {
            if ( i && ((i % 10000) == 0) ) {
                cerr << "# Topo: " << i << endl;
            }
            if (feature.isTiled()) {
	            VpfRectangle rect = feature.getTile(i).getBoundingRectangle();
	            if (!overlap(rect, ubounds))
	                continue;
            }
	  
            switch (type)
            {
            case VpfFeature::POINT:
	            dump_point(feature.getPoint(i), ubounds);
	            break;
            case VpfFeature::LINE:
	            dump_line(feature.getLine(i), ubounds);
	            break;
            case VpfFeature::POLYGON:
	            dump_polygon(feature.getPolygon(i), ubounds);
	            break;
            case VpfFeature::LABEL:
	            dump_label(feature.getLabel(i), ubounds);
	            break;
            default:
	            throw VpfException("Unsupported topology type");
            }
        }
    } catch (const VpfException &e) {
        cerr << "# Died with exception: " << e.getMessage() << endl;
        return 1;
    }

    cerr << "# Bounds: " << ubounds << endl;
    if (nTopologies)
        cerr << "# Scanned " << nTopologies << " topologies..." << endl;
    if (gen_color) {
        cerr << "# last color " << next_color << endl;
    }
    return 0;
}

// end of vpf-topology.cxx
