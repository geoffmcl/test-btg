// vpf-dump.cxx - dump the contents of VPF tables for debugging.
// This file is released into the Public Domain, and comes with NO WARRANTY!

////////////////////////////////////////////////////////////////////////
// Dump the contents of one or more VPF tables.
//
// Usage:
//
// vpf-dump <table...>
//
// This utility is useful for studying the structure of VPF tables
// or looking for data not currently available through the library.
// Here's an example:
//
//   vpf-dump char.vdt | less
////////////////////////////////////////////////////////////////////////

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#include "vpf.hxx"


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
 * Dump the table's header.
 */
void
dump_table_header (const VpfTable &table)
{
  cout << "Path: " << table.getPath() << endl;
  cout << "Description: \"" << table.getDescription() << '"' << endl;
  cout << "Documentation file: " << table.getDocFileName() << endl;
  cout << "Total columns: " << table.getColumnCount() << endl;
  cout << "Total rows: " << table.getRowCount() << endl;
}


/**
 * Dump the declarations for each table column.
 */
void
dump_table_column_decls (const VpfTable &table)
{
  for (int i = 0; i < table.getColumnCount(); i++) {
    const VpfColumnDecl &column = table.getColumnDecl(i);
    cout << "Column " << i << " Declaration: " << endl;
    cout << "  Name: " << column.getName() << endl;
    cout << "  Type: " << get_value_type_name(column.getValueType()) << endl;
    cout << "  Element count: " << column.getElementCount() << endl;
    cout << "  Key type: " << column.getKeyType() << endl;
    cout << "  Description: " << column.getDescription() << endl;
    cout << "  Value description table: "
	 << column.getValueDescriptionTable() << endl;
    cout << "  Thematic index name: "
	 << column.getThematicIndexName() << endl;
    cout << "  Narrative table: "
	 << column.getNarrativeTable () << endl;
  }
}


/**
 * Dump the main body of the table itself.
 */
void
dump_table_body (const VpfTable &table)
{
  for (int i = 0; i < table.getRowCount(); i++) {
    cout << "Row " << i << ':' << endl;
    for (int j = 0; j < table.getColumnCount(); j++) {
      cout << "  "
	   << table.getColumnDecl(j).getName()
	   << '='
	   << table.getValue(i, j)
	   << endl;
    }
  }
}

void show_usage()
{
    const char *ind = "   ";
    cerr << endl;
    cerr << ind << "Usages: vpf-dump table1 [table2...]" << endl;
    cerr << ind << "Example: 'vpf-dump char.vdt'" << endl;
    cerr << endl;
    cerr << ind << "Will load table and dump header showing path, description, columns and row counts" << endl;
    cerr << ind << "Proceed to dump each table columns declarations, show name, type, count, key, desc,..." << endl;
    cerr << ind << "Finally dump table body per row, show id, table, attribute, value, description." << endl;
    cerr << ind << "eg id=4, table=rryardp.pft, attribute=f_code, value=AN060, description=Railroad Yard/Marshalling Yard." << endl;
    cerr << endl;
    cerr << ind << "Use vpf-summary to progressively drill down into the data" << endl;
    cerr << ind << "vpf-summary database_root [library [coverage [feature [property]]]]" << endl;

}

/**
 * Main entry point.
 */
int
main (int ac, char ** av)
{
    if (ac < 2) {
        show_usage();
        return 1;
    }
  for (int i = 1; i < ac; i++) {
    try {
        cout << "Load table '" << av[i] << "'" << endl;
      VpfTable table(av[i]);
        cout << "Dump table header..." << endl;
      dump_table_header(table);
        cout << "Dump table columns declarations..." << endl;
      dump_table_column_decls(table);
        cout << "Dump table body..." << endl;
      dump_table_body(table);
        cout << endl;
    } catch (VpfException &e) {
      cerr << e.getMessage() << endl;
      return 1;
    }
  }
  return 0;
}

// end of vpf-dump.cxx
