/*\
 * utils.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _UTILS_HXX_
#define _UTILS_HXX_
#include <string>
#include <vector>
#ifdef _MSC_VER
#define M_IS_DIR _S_IFDIR
#else // !_MSC_VER
#define M_IS_DIR S_IFDIR
#endif

enum DiskType {
    MDT_NONE,
    MDT_FILE,
    MDT_DIR
};

extern DiskType is_file_or_directory32 ( const char * path );
extern size_t get_last_file_size32();
#ifdef _MSC_VER
extern DiskType is_file_or_directory64 ( const char * path );
extern __int64 get_last_file_size64();
#define is_file_or_directory is_file_or_directory64
#define get_last_file_size get_last_file_size64
#else
#define is_file_or_directory is_file_or_directory32
#define get_last_file_size get_last_file_size32
#endif

extern char *double_to_stg( double d );
extern void trim_float_buf( char *pb );
extern char *get_I64u_Stg( unsigned long long val );
extern char *get_I64_Stg( long long val );
extern char *get_k_num( long long i64, int type = 0, int dotrim = 1 );
extern char *get_nice_number64( long long num, size_t width = 0 );
extern char *get_nice_number64u( unsigned long long num, size_t width = 0 );

extern void nice_num( char * dst, char * src ); // get nice number, with commas

extern double get_seconds();
char *get_elapsed_stg( double start );

typedef struct tagMBBOX {
    double min_lat,min_lon,max_lat,max_lon,min_alt,max_alt;
}MBBOX, *PMBBOX;

#define INVALID_LATLONG 400.0
#define INVALID_ALT 99999.0
#define init_mbbox(a) { \
    a.min_lat = INVALID_LATLONG;  \
    a.min_lon = INVALID_LATLONG;  \
    a.max_lat = -INVALID_LATLONG; \
    a.max_lon = -INVALID_LATLONG; \
    a.min_alt = INVALID_ALT; \
    a.max_alt = -INVALID_ALT; \
}

#define valid_mbbox(a) \
    ((a.min_lat != INVALID_LATLONG) &&  \
     (a.min_lon != INVALID_LATLONG) &&  \
     (a.max_lat != -INVALID_LATLONG) && \
     (a.max_lon != -INVALID_LATLONG) && \
     (a.min_alt != INVALID_ALT) && \
     (a.max_alt != -INVALID_ALT))

#define set_mbbox(a,lat,lon,alt) {        \
    if (lat < a.min_lat) a.min_lat = lat; \
    if (lat > a.max_lat) a.max_lat = lat; \
    if (lon < a.min_lon) a.min_lon = lon; \
    if (lon > a.max_lon) a.max_lon = lon; \
    if (alt > a.max_alt) a.max_alt = alt; \
    if (alt < a.min_alt) a.min_alt = alt; \
}

#define merge_mbbox(a,b) {        \
    if (valid_mbbox(b)) {         \
    if (b.min_lat < a.min_lat) a.min_lat = b.min_lat; \
    if (b.max_lat > a.max_lat) a.max_lat = b.max_lat; \
    if (b.min_lon < a.min_lon) a.min_lon = b.min_lon; \
    if (b.max_lon > a.max_lon) a.max_lon = b.max_lon; \
    if (b.max_alt > a.max_alt) a.max_alt = b.max_alt; \
    if (b.min_alt < a.min_alt) a.min_alt = b.min_alt; \
    }}


extern char *get_mbbox_stg( PMBBOX pbb );


#define VERB1(a) (a >= 1)
#define VERB2(a) (a >= 2)
#define VERB5(a) (a >= 5)
#define VERB9(a) (a >= 9)

typedef std::vector<std::string> vSTGS;
extern bool string_in_vec( vSTGS &vs, const char *file );


#endif // #ifndef _UTILS_HXX_
// eof - utils.hxx

