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
int set_label_to_entry(label_ptr h, char *name);
void proceed_addr(label_ptr label, int num, bool is_data);
unsigned int get_label_addr(label_ptr h, char *name);
bool is_label_external(label_ptr hptr, char *name);
void print_labels(label_ptr h);


#endif
