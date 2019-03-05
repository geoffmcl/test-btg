#ifndef BTG_FILE_H
#define BTG_FILE_H

#include "struct.h"

typedef struct xml_node_s {
	char key[512];
	char value[512];
	struct xml_node_s *tree;
	struct xml_node_s *next;
} xml_node;

typedef struct xml_tree_s {
	struct xml_node_s *node;
	struct xml_tree_s *next;
} xml_tree;

int get_terrain_path (char fullpath[], const char basepath[], const int index);
void get_airport_path (char fullpath[], const char basepath[], char airport[]);

runway_info *get_airport_info (const char fullpath[], const char als[]);
xml_node *get_xml_tree (FILE *xml);
xml_node *get_key_value (FILE *xml);
void set_als (threshold_info *threshold, const char *als);

int btg_decompress (const char filename[]);

#endif