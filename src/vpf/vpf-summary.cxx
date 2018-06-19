// vpf-summary.cxx - print a summary of a VPF database's structure.
// This file is released into the Public Domain, and comes with NO WARRANTY!

////////////////////////////////////////////////////////////////////////
// Summarize the structure of a VPF database.
//
// Usage: 
//
// vpf-summary <database root> [library [coverage [feature [property]]]]
//
// This utility can drill down fairly far into a VPF database.  It can
// display anything from the schema for a whole database to the
// schema for a single feature property, depending on what appears
// on the command line.  Here's an example to dump the whole schema
// for all libraries on the vmap0 North America CD:
//
//   vpf-summary /cdrom/vmaplv0
//
// Here's a second example to show just the allowed values for the
// f_code feature property on the roadline feature in the transportation
// coverage of the North America library:
//
//   vpf-summary /cdrom/vmaplv0 noamer trans roadl f_code
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::string;

using std::cout;
using std::cerr;
using std::endl;

#include "vpf.hxx"

static std::string s_database;
static std::string s_library;
static std::string s_coverage;
static std::string s_feat;
static std::string s_prop;
typedef std::vector<std::string> vSTG;

static int verbosity = 0;

#define VERB1 (verbosity >= 1)
#define VERB2 (verbosity >= 2)
#define VERB5 (verbosity >= 5)
#define VERB9 (verbosity >= 9)


static double minlon = 9999;
static double minlat = 9999;
static double maxlon = -9999;
static double maxlat = -9999;

static vSTG vCommands;

/**
* Add to bounds.
*/
static void add_lon_lat_to_bounds(double lon, double lat)
{
    if (lon < minlon)
        minlon = lon;
    if (lat < minlat)
        minlat = lat;
    if (lon > maxlon)
        maxlon = lon;
    if (lat > maxlat)
        maxlat = lat;
}


/**
 * Get a printable name for a value type.
 */
static const char *
get_value_type_name (VpfValue::Type type)
{
  switch (type) {
  case VpfValue::EMPTY:
    return "empty";
  case VpfValue::TEXT:
    return "text";
  case VpfValue::INT:
    return "integer";
  case VpfValue::REAL:
    return "real number";
  case VpfValue::POINTS:
    return "coordinate array";
  case VpfValue::DATE:
    return "date and time";
  case VpfValue::CROSSREF:
    return "cross-tile reference";
  default:
    throw VpfException("Unknown value type");
  }
}


/**
 * Get a printable name for a topology type.
 */
static const char *
get_topology_name (VpfFeature::TopologyType type)
{
  switch (type) {
  case VpfFeature::UNKNOWN:
    return "unknown";
  case VpfFeature::POINT:
    return "point";
  case VpfFeature::LINE:
    return "line";
  case VpfFeature::POLYGON:
    return "polygon";
  case VpfFeature::LABEL:
    return "text";
  default:
    return "unknown";
  }
}


/**
 * Print an indentation.
 */
static string
indent (int level)
{
  string result;
  for (int i = 0; i < level; i++)
    result += ' ';
  return result;
}


/**
 * Print a summary of a property declaration.
 */
static void
dump_property_decl (const VpfPropertyDecl &decl, int level)
{
    s_prop = decl.getName();
    std::stringstream s;
    s << s_database << " " << s_library << " ";
    s << s_coverage << " ";
    s << s_feat << " ";
    s << s_prop;
    vCommands.push_back(s.str());

    std::stringstream info;
    info << indent(level) << "Name: " << decl.getName() << endl;
    info << indent(level) << "Description: " << decl.getDescription() << endl;
    info << indent(level) << "Type: "
       << get_value_type_name(decl.getValueType()) << endl;
  if (decl.getValueCount() > 0) {
    info << indent(level) << "Number of allowed values: "
	 << decl.getValueCount() << endl;
    for (int i = 0; i < decl.getValueCount(); i++) {
      info << indent(level) << "Allowed Value " << i << ':' << endl;
      info << indent(level+2) << "Value: " << decl.getValue(i) << endl;
      info << indent(level+2) << "Description: "
	   << decl.getValueDescription(i) << endl;
    }
  } else {
    info << indent(level) << "No value restrictions." << endl;
  }
  if (VERB2) {
      cout << info.str();
  }
}


/**
 * Print a summary of a feature.
 */
static void
dump_feature (const VpfFeature &feature, int level)
{
    s_feat = feature.getName();
    std::stringstream s;
    s << s_database << " " << s_library << " ";
    s << s_coverage << " ";
    s << s_feat;
    vCommands.push_back(s.str());

    size_t max = feature.getTopologyCount();
    std::stringstream info;
    info << indent(level) << "Name: " << s_feat << endl;
    info << indent(level) << "Description: " << feature.getDescription() << endl;
    info << indent(level) << "Topology type: "
       << get_topology_name(feature.getTopologyType()) << endl;
    info << indent(level) << "Number of topologies: " << max << endl;
  if (feature.getTopologyType() == VpfFeature::POLYGON &&
      max > 0) {
    info << indent(level) << "Bounding rectangle of first polygon: ";
    VpfRectangle bounds = feature.getPolygon(0).getBoundingRectangle();
    info << bounds.minX << ','
	 << bounds.minY << ','
	 << bounds.maxX << ','
	 << bounds.maxY << endl;
    add_lon_lat_to_bounds(bounds.minX, bounds.minY);
    add_lon_lat_to_bounds(bounds.maxX, bounds.maxY);
  }
  info << indent(level) << "Number of feature properties: "
       << feature.getPropertyDeclCount() << endl;
  for (int i = 0; i < feature.getPropertyDeclCount(); i++) {
    info << indent(level) << "Feature property " << i << ':' << endl;
    if (VERB2) {
        cout << info.str();
        info.str("");
    }
    dump_property_decl(feature.getPropertyDecl(i), level+2);
  }
}


/**
 * Print a summary of a coverage.
 */
static void
dump_coverage (const VpfCoverage &cov, int level)
{
    std::stringstream s;
    s << s_database << " " << s_library << " ";
    s_coverage = cov.getName();
    s << s_coverage;
    vCommands.push_back(s.str());

    std::stringstream info;
    info << indent(level) << "Coverage name: " << s_coverage << endl;
    info << indent(level) << "Coverage description: "
       << cov.getDescription() << endl;
    info << indent(level) << "Coverage path: " << cov.getPath() << endl;
    info << indent(level) << "Topological level: " << cov.getLevel() << endl;
    info << indent(level) << "Number of features: "
       << cov.getFeatureCount() << endl;

  for (int i = 0; i < cov.getFeatureCount(); i++) {
    info << indent(level) << "Feature " << i << ':' << endl;
    if (VERB1) {
        cout << info.str();
        info.str("");
    }
    dump_feature(cov.getFeature(i), level+2);
  }
}


/**
 * Print a summary of a library.
 */
static void
dump_library (const VpfLibrary &lib, int level)
{
    s_library = lib.getName();
    std::stringstream s;
    s << s_database << " " << s_library;
    vCommands.push_back(s.str());

    std::stringstream info;
    info << indent(level) << "Library name: " << s_library << endl;
    info << indent(level) << "Library description: "
       << lib.getDescription() << endl;
    info << indent(level) << "Library path: " << lib.getPath() << endl;
    info << indent(level) << "Minimum bounding rectangle: ";
  VpfRectangle bounds = lib.getBoundingRectangle();
  info << bounds.minX << ','
       << bounds.minY << ','
       << bounds.maxX << ','
       << bounds.maxY << endl;
  add_lon_lat_to_bounds(bounds.minX, bounds.minY);
  add_lon_lat_to_bounds(bounds.maxX, bounds.maxY);
  info << indent(level) << "Number of coverages: "
       << lib.getCoverageCount() << endl;
  for (int i = 0; i < lib.getCoverageCount(); i++) {
    info << indent(level) << "Coverage " << i << ':' << endl;
    // if (VERB2) {
        cout << info.str();
        info.str("");
    // }
    dump_coverage(lib.getCoverage(i), level+2);
  }
}


/**
 * Print a summary of a database.
 */
static void
dump_database (const VpfDataBase &db, int level)
{
  cout << indent(level) << "Database name: " << db.getName() << endl;
  cout << indent(level) << "Database description: "
       << db.getDescription() << endl;
  cout << indent(level) << "Database path: " << db.getPath() << endl;
  cout << indent(level) << "Number of libraries: "
       << db.getLibraryCount() << endl;
  for (int i = 0; i < db.getLibraryCount(); i++) {
    cout << "Library " << (i+1) << ':' << endl;
    dump_library(db.getLibrary(i), level+2);
  }
}

void show_usage()
{
    const char *ind = "   ";
    cerr << endl;
    cerr << ind << "Usage:" << endl;
    cerr << endl;
    cerr << ind << "vpf-summary database_root [library [coverage [feature [property]]]]" << endl;
    cerr << endl;
    cerr << ind << "This utility can drill down fairly far into a VPF database.  It can" << endl;
    cerr << ind << "display anything from the schema for a whole database to the" << endl;
    cerr << ind << "schema for a single feature property, depending on what appears" << endl;
    cerr << ind << "on the command line.  Here's an example to dump the whole schema" << endl;
    cerr << ind << "for all libraries on the vmap0 North America CD:" << endl;
    cerr << endl;
    cerr << ind << "vpf-summary /cdrom/vmaplv0" << endl;
    cerr << endl;
    cerr << ind << "Here's a second example to show just the allowed values for the" << endl;
    cerr << ind << "f_code feature property on the roadline feature in the transportation" << endl;
    cerr << ind << "coverage of the North America library:" << endl;
    cerr << endl;
    cerr << ind << "vpf-summary /cdrom/vmaplv0 noamer trans roadl f_code" << endl;
}

void dump_commands()
{
    size_t ii, max = vCommands.size();
    cout << endl << "Potential inputs: " << max << endl;
    cout << "database_root library [coverage [feature [property]]]" << endl;
    for (ii = 0; ii < max; ii++) {
        cout << vCommands[ii] << endl;
    }
    cout << "Those with 4 commands can also be used with vpf-topology" << endl;
    cout << "where min/max lat/lon can be added to get specific topology data." << endl;
}

int
main (int ac, char ** av)
{
    
    switch (ac) {
    case 1:
        show_usage();
        return 1;
    case 2:
        s_database = av[1];
        break;
    case 3:
        s_database = av[1];
        s_library  = av[2];
        break;
    case 4:
        s_database = av[1];
        s_library  = av[2];
        s_coverage = av[3];
        break;
    case 5:
        s_database = av[1];
        s_library  = av[2];
        s_coverage = av[3];
        s_feat     = av[4];
        break;
    case 6:
        s_database = av[1];
        s_library  = av[2];
        s_coverage = av[3];
        s_feat     = av[4];
        s_prop     = av[5];
        break;
    default:
      cerr << "Usage: " << av[0] << "<database> [library [coverage [feature]]]" << endl;
      return 2;
    }

    try {
    switch (ac) {
    case 1:
        show_usage();
        return 1;
    case 2:
      cout << "*** Database: " << s_database << " ***" << endl;
      dump_database(VpfDataBase(s_database), 0);
      dump_commands();
      return 0;
    case 3:
      cout << "*** Library: " << s_library << " ***" << endl;
      dump_library(VpfDataBase(s_database).getLibrary(s_library), 0);
      dump_commands();
      return 0;
    case 4:
      cout << "*** Coverage: " << s_coverage << " ***" << endl;
      dump_coverage(VpfDataBase(s_database)
		    .getLibrary(s_library).getCoverage(s_coverage), 0);
      dump_commands();
      return 0;
    case 5:
      cout << "*** Feature: " << s_feat << " ***" << endl;;
      dump_feature(VpfDataBase(s_database).getLibrary(s_library).getCoverage(s_coverage)
		   .getFeature(s_feat), 0);
      dump_commands();
      return 0;
    case 6:
      cout << "*** Feature Property: " << s_prop << " ***" << endl;
      dump_property_decl(VpfDataBase(s_database).getLibrary(s_library)
			 .getCoverage(s_coverage).getFeature(s_feat)
			 .getPropertyDecl(s_prop), 0);
      dump_commands();
      return 0;
    default:
      cerr << "Usage: " << av[0] << "<database> [library [coverage [feature]]]" << endl;
      return 2;
    }
  } catch (VpfException &e) {
    cerr << e.getMessage() << endl;
    return 1;
  }

}

// end of vpf-summary.cxx
