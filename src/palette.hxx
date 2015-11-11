/*\
 * palette.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _PALETTE_HXX_
#define _PALETTE_HXX_
#include <string>


extern std::string get_mat_color( const char *mat );
extern bool is_mat_in_list( const char *mat );
extern void outout_mat_color_counts();
extern std::string get_palette_html();
extern void close_palette();

#endif // #ifndef _PALETTE_HXX_
// eof - palette.hxx
