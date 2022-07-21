#ifndef LABELS_LIST_H
#define LABELS_LIST_H

#include "global.h"
#include "utils.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


/* Declarations */


/* Prototypes */
void free_labels(label_ptr *hptr);
status delete_label(label_ptr *hptr, char *name);
label_ptr get_label(label_ptr hptr, char *name);
label_ptr insert_label(label_ptr *hptr, char *name, unsigned int address, bool external, ...);
bool is_existing_label(label_ptr hptr, char *name);

#endif
