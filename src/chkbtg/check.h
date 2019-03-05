#ifndef BTG_CHECK_H
#define BTG_CHECK_H

#include "btgio.h"

void remove_unused  (btg_header *head);
void check_ojects   (btg_header *head);
void check_base     (btg_header *head);
void check          (btg_header *head);

#endif