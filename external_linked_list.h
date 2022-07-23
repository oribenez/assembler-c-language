#ifndef EXTERNAL_LIST_H
#define EXTERNAL_LIST_H

#include "global.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ext_free_list(ext_ptr *hptr);
ext_ptr ext_insert_item(ext_ptr *hptr, char *name, unsigned int reference);
void print_ext(ext_ptr h);

#endif



