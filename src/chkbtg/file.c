#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#ifndef NO_LIBARCH
#include <archive.h>
#include <archive_entry.h>
#endif /* alternates... */

#include "file.h"
#include "coord.h"

#ifndef PATH_MAX
#define PATH_MAX 260
#endif

#ifdef _MSC_VER
/* missing windows define */
#ifndef _SSIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    ssize_t;
#else
typedef _W64 unsigned int   ssize_t;
#endif
#define _SSIZE_T_DEFINED
#endif
#endif /* #ifdef _MSC_V£R */

int get_terrain_path (char fullpath[], const char basepath[], const int index) {

	char latc, lonc;
	coord_geo geo = index2geo(index);
	geo.lon = geo.lon * 180.0 / M_PI + .001;
	geo.lat = geo.lat * 180.0 / M_PI + .001;
	printf("lon: %Lf / lat: %Lf\n", geo.lon, geo.lat);
	if (geo.lat < 0.0) latc = 's'; else latc = 'n';
	if (geo.lon < 0.0) lonc = 'w'; else lonc = 'e';
		sprintf(fullpath, "%s/Terrain/%c%03d%c%02d/%c%03d%c%02d/", basepath,
		    lonc, abs(((int) floor(geo.lon / 10.0)) * 10),
		    latc, abs(((int) floor(geo.lat / 10.0)) * 10),
		    lonc, abs((int) floor(geo.lon)),
		    latc, abs((int) floor(geo.lat))
		    );
	return 0;
}

void get_airport_path (char fullpath[], const char basepath[], char airport[]) {

	int i, len;
	len = (int)strlen(airport);
	char ap[10] = { 0 };

	for (i = 0 ; i < len ; i++) {
		airport[i] = toupper(airport[i]);
		if (i < (len -1)) {
			ap[i * 2] = airport[i];
			ap[i * 2 + 1] = '/';
		}
	}
	sprintf(fullpath, "%s/Airports/%s%s.threshold.xml", basepath, ap, airport);
}

runway_info *get_airport_info (const char fullpath[], const char als[]) {

	int thr = -1, level = 0, i;
	char order;
	FILE *xml = NULL;
	xml_node *root = NULL, *node = NULL;
	runway_info *runway = NULL, *new = NULL, *last = NULL;
	xml_tree *tree = NULL, *temp = NULL;

	if ((xml = fopen(fullpath, "r")) == NULL) {
		fprintf(stderr, "can't open file '%s'! break.\n", fullpath);
		return runway;
	}
	root = get_xml_tree (xml);
	fclose (xml);

	node = root;
	while (node) {
//		for (i = 0 ; i < level ; i++) printf("    ");
//		printf("key: '%s'", node->key);
		if (strlen(node->value)) {
//			printf(" / value: '%s'", node->value);
			if (new && strncmp(node->key, "lon"    , 3) == 0) new->threshold[thr].lon = atof(node->value);
			if (new && strncmp(node->key, "lat"    , 3) == 0) new->threshold[thr].lat = atof(node->value);
			if (new && strncmp(node->key, "hdg-deg", 7) == 0) new->threshold[thr].heading = atof(node->value);
			if (new && strncmp(node->key, "displ-m", 7) == 0) new->threshold[thr].displacement = atof(node->value);
			if (new && strncmp(node->key, "stopw-m", 7) == 0) new->threshold[thr].stopw = atof(node->value);
			if (new && strncmp(node->key, "rwy"    , 3) == 0) {
				i = atoi(node->value);
				order = node->value[strlen(node->value) - 1];
				new->threshold[thr].rwy_num = i;
				if      (order == 'L' || order == 'l') new->threshold[thr].rwy_ord = 1;
				else if (order == 'C' || order == 'c') new->threshold[thr].rwy_ord = 2;
				else if (order == 'R' || order == 'r') new->threshold[thr].rwy_ord = 3;
				else new->threshold[thr].rwy_ord = 0;
			}
		}

		if (node->tree) {
			if ((temp = malloc(sizeof(*temp))) == NULL) {
				fprintf(stderr, "no memory left for XML tree! break.\n");
				return runway;
			}
			temp->node = node;
			temp->next = tree;
			tree = temp;
			level += 1;
			if (strncmp(node->key, "threshold", 9) == 0) thr++;
			if (strncmp(node->key, "runway", 6) == 0) {
				if ((new = malloc(sizeof(*new))) == NULL) {
					fprintf(stderr, "no memory left for runway! break.\n");
					return runway;
				}
				new->threshold[0].lon = 0.0;
				new->threshold[0].lat = 0.0;
				new->threshold[0].heading = 0.0;
				new->threshold[0].displacement = 0.0;
				new->threshold[0].stopw = 0.0;
				new->threshold[0].rwy_num = 0;
				new->threshold[0].rwy_ord = 0;
				new->threshold[0].als_layout = ALS_NOOP;
				new->threshold[0].als_len = 0;
				new->threshold[0].origin = NULL;
				new->threshold[1].lon = 0.0;
				new->threshold[1].lat = 0.0;
				new->threshold[1].heading = 0.0;
				new->threshold[1].displacement = 0.0;
				new->threshold[1].stopw = 0.0;
				new->threshold[1].rwy_num = 0;
				new->threshold[1].rwy_ord = 0;
				new->threshold[1].als_layout = ALS_NOOP;
				new->threshold[1].als_len = 0;
				new->threshold[1].origin = NULL;
				new->next = NULL;

				if (last) {
					last->next = new;
					last = last->next;
				}
				else {
					runway = last = new;
				}
				thr = -1;

			}
			node = node->tree;

		}
		else node = node->next;

		if (!node) {
			while (!node && tree) {
				node = tree->node->next;
				temp = tree->next;
				free(tree);
				tree = temp;
				level -= 1;
			}
		}

	}

	last = runway;
	while (last) {
		set_als(&last->threshold[0], als);
		set_als(&last->threshold[1], als);
		last = last->next;
	}

	return runway;
}


xml_node *get_xml_tree (FILE *xml) {

	xml_node *root = NULL, *node = NULL, *last = NULL;

	while ((node = get_key_value (xml))) {

		if (node->key[0] != '\0' || node->value[0] != '\0') {

			if (node->value[0] == '\0') {
			if (node->key[0] == '/') {
//				printf("end key: '%s'\n", node->key);
				node->next = root;
				root = node;
				return root;
			}
			else {
//				printf("start key: '%s'\n", node->key);
				if (last) {
					last->next = node;
					last = last->next;
				}
				else root = last = node;
				node = get_xml_tree(xml);
				if (strlen(last->key) == strlen(&node->key[1]) && !strncmp(last->key, &node->key[1], strlen(last->key))) {
					last->tree = node->next;
					free (node);
				}
			}
		}
		else {
//			printf("key: '%s' / value: '%s'\n", node->key, node->value);
			if (last) {
				last->next = node;
				last = last->next;
			}
			else root = last = node;
		}
		}
		else {
			free (node);
		}
	}

	return root;
}

xml_node *get_key_value (FILE *xml) {

	char line[1024];
	int len, i;
	int key_flag = 0, key_pos = 0;
	int value_flag = 1, value_pos = 0;
	xml_node *node = NULL;

	if (fgets(line, 1024, xml) == NULL) return NULL;
	if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

	if ((node = malloc(sizeof(*node))) == NULL) {
		fprintf(stderr, "no memory left for XML node! break.\n");
		return NULL;
	}
	node->key[0] = '\0';
	node->value[0] = '\0';
	node->tree = NULL;
	node->next = NULL;

	len = (int)strlen(line);
	for (i = 0 ; i < len ; i++) {
		if (!key_flag && !value_flag && isspace(line[i])) continue;

		if (value_flag && line[i] == '<') {
			node->value[value_pos] = '\0';
			value_flag = 0;
		}
		if (value_flag) {
			if (value_pos || !isspace(line[i])) node->value[value_pos++] = line[i];
		}
		if (key_flag && line[i] == '>') {
			key_flag = 0;
			if (node->key[0] == '?') {
				key_pos = 0;
				node->key[0] = '\0';
			}
			else {
				node->key[key_pos] = '\0';
				value_flag = 1;
			}
		}
		if (key_flag) {
			node->key[key_pos++] = line[i];
		}
		if (!key_flag && line[i] == '<') key_flag = 1;
	}
	node->key[key_pos] = '\0';
	node->value[value_pos] = '\0';
	return node;
}

void set_als (threshold_info *threshold, const char *als) {

	int pos, last = 0, rwy, order, als_num, als_len;
	char rwy_info[64], als_info[64], *token, ord;

	for (pos = 0 ; als[pos] ; pos++) {
		if (als[pos] == '/') {
			strncpy(rwy_info, &als[last], pos - last);
			rwy_info[pos - last] = '\0';

			token = strpbrk(rwy_info, "=");
			if (token) {
				*token = '\0';
				token++;
				if (token[0]) {
					strncpy(als_info, token, 64);
					token = strpbrk(als_info, "@");
					if (token) {
						*token = '\0';
						token++;
						if (token[0]) {
							als_len = atoi(token);
						}
						else {
							als_len = 0;
						}
					}
					else {
						als_len = 0;
					}
				}
				else {
					als_num = 0;
					als_len = 0;
				}
			}
			else {
				als_info[0] = '\0';
				als_num = 0;
				als_len = 0;
			}

			if (strlen(als_info)) {
				token = als_info;
				while (token[0]) {
					token[0] = tolower(token[0]);
					token++;
				}
				if (
				    strncmp(als_info, "alsf1", 5) == 0 ||
				    strncmp(als_info, "alsfi", 5) == 0 ||
				    strncmp(als_info, "alsf-1", 6) == 0 ||
				    strncmp(als_info, "alsf-i", 6) == 0
				    ) {
					als_num = ALS_ALSF1;
					if (!als_len) als_len = 720;
					if (als_len < 420) als_len = 420;
					if (als_len > 900) als_len = 900;
				}
				else if (
				    strncmp(als_info, "alsf2", 5) == 0 ||
				    strncmp(als_info, "alsfii", 6) == 0 ||
				    strncmp(als_info, "alsf-2", 5) == 0 ||
				    strncmp(als_info, "alsf-ii", 7) == 0
				    ) {
					als_num = ALS_ALSF2;
					if (!als_len) als_len = 720;
					if (als_len < 420) als_len = 420;
					if (als_len > 900) als_len = 900;
				}
				else if (strncmp(als_info, "ssalr", 5) == 0) {
					als_num = ALS_SSALR;
					if (!als_len) als_len = 720;
					if (als_len < 600) als_len = 600;
					if (als_len > 900) als_len = 900;
				}
				else if (strncmp(als_info, "ssalf", 5) == 0) {
					als_num = ALS_SSALF;
					if (!als_len) als_len = 420;
					if (als_len < 420) als_len = 420;
					if (als_len > 900) als_len = 900;
				}
				else if (strncmp(als_info, "ssals", 5) == 0 ||
				    strncmp(als_info, "sals", 4) == 0
				    ) {
					als_num = ALS_SSALS;
					if (!als_len) als_len = 420;
					if (als_len < 420) als_len = 420;
					if (als_len > 900) als_len = 900;
				}
				else if (strncmp(als_info, "malsr", 5) == 0) {
					als_num = ALS_MALSR;
					if (!als_len) als_len = 720;
					if (als_len < 600) als_len = 600;
					if (als_len > 900) als_len = 900;
				}
				else if (strncmp(als_info, "malsf", 5) == 0) {
					als_num = ALS_MALSF;
					if (!als_len) als_len = 420;
					if (als_len < 420) als_len = 420;
					if (als_len > 900) als_len = 900;
				}
				else if (strncmp(als_info, "mals", 4) == 0) {
					als_num = ALS_MALS;
					if (!als_len) als_len = 420;
					if (als_len < 420) als_len = 420;
					if (als_len > 900) als_len = 900;
				}
				else {
					fprintf(stderr, "can't read Approach Light System! ignore it.\n");
					als_num = ALS_NOOP;
					als_len = 0;
				}
			}

			rwy = atoi(rwy_info);
			ord = rwy_info[strlen(rwy_info) - 1];
			if      (ord == 'L' || ord == 'l') order = 1;
			else if (ord == 'C' || ord == 'c') order = 2;
			else if (ord == 'R' || ord == 'r') order = 3;
			else order = 0;

			if (threshold->rwy_num == rwy && threshold->rwy_ord == order) {
				threshold->als_layout = als_num;
				threshold->als_len = als_len;
			}

			last = pos + 1;
		}
	}
}

int btg_decompress (const char filename[]) {

#ifndef NO_LIBARCH
	char iname[PATH_MAX], oname[PATH_MAX];
	struct archive *a = NULL;
	struct archive_entry *entry = NULL;
	FILE *ifile = NULL, *ofile = NULL;
	int res;
	ssize_t size_r, size_w;
	char buff[1024];

	snprintf(iname, PATH_MAX, "%s.btg.gz", filename);
	snprintf(oname, PATH_MAX, "%s.btg", filename);
	printf("extracting archive '%s' ...\n", iname);

	if ((ifile = fopen(iname, "rb")) == NULL) {
		fprintf(stderr, "could't open file '%s' to read! exit.\n", iname);
		return 1;
	}

	if ((ofile = fopen(oname, "wb")) == NULL) {
		fprintf(stderr, "could't open file '%s' to write! exit.\n", oname);
		fclose(ifile);
		return 1;
	}

	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_raw(a);
	if ((res = archive_read_open_FILE(a, ifile)) != ARCHIVE_OK) {
		fprintf(stderr, "%s! exit.\n", archive_error_string(a));
		fclose(ifile);
		fclose(ofile);
		return 1;
	}

	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		for (;;) {
			size_r = archive_read_data(a, buff, 1024); // archive_read_data(struct archive *, void *buff, size_t len);
			if (size_r == 0) break;
			if (size_r < 0) {
				fprintf(stderr, "%s! exit.\n", archive_error_string(a));
				fclose(ifile);
				fclose(ofile);
				return 1;
			}
			size_w = fwrite(buff, 1, size_r, ofile);
			if (size_r != size_w) {
				fprintf(stderr, "read %zd byte / write %zd byte! exit.\n", size_r, size_w);
				fclose(ifile);
				fclose(ofile);
				return 1;
			}
		}
		archive_read_data_skip(a);
	}

	if ((res = archive_read_free(a)) != ARCHIVE_OK) {
		fprintf(stderr, "cant close archive! exit.\n");
		fclose(ifile);
		fclose(ofile);
		return 1;
	}

	fclose(ifile);
	fclose(ofile);

	printf("done.\n");
	return 0;
#else
    /* fprintf(stderr, "not implemented...\n"); */
    /* some alternative, or... */
    int iret = 0;
    FILE *ofile, *ifile;
    int res = 0;
    char cmd[PATH_MAX * 2];
    char iname[PATH_MAX], oname[PATH_MAX];
    snprintf(iname, PATH_MAX, "%s.btg.gz", filename);
    snprintf(oname, PATH_MAX, "%s.btg", filename);
    if ((ifile = fopen(iname, "rb")) == NULL) {
        fprintf(stderr, "could't open file '%s' for reading! exit.\n", iname);
        return 1;
    }
    fclose(ifile);

    /* check for gzip -d version ... */
    if ((ofile = fopen(oname, "rb")) == NULL) 
    {
        /* extract the archive... */
        printf("extracting archive '%s' ...\n", iname);
        sprintf(cmd, "gzip -d -k %s", iname);
        res = system(cmd);  /* try it ... */
        /* could/should check 'res'... but at least check for the file... */
        if ((ofile = fopen(oname, "rb")) == NULL) {
            fprintf(stderr, "could't open file '%s' to read! exit.\n", oname);
            return 1;
        }
        fclose(ofile);  /* SUCCESS */
        printf("created outfile '%s' ...\n", oname);
        /* maybe need ceanup of these 'temporary' extracts, until ... say gzFile used... */
    }
    else {
        /* found gzip -d version */
        fclose(ofile);
    }
    return iret;
#endif
    
}

/* eof */
