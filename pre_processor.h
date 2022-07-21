#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

#include "global.h"
#include <stdio.h>
#define MACRO_NAME_LEN 35 /* *Assumption*: Max length of macro name */
#define MACRO_ROWS 15     /* *Assumption*: Max macro content rows */

/* Declarations */
typedef struct Macro *macro_ptr;
typedef struct Macro {
    char name[MACRO_NAME_LEN];                  /* macro unique name */
    char content[MAX_LINE_LENGTH * MACRO_ROWS]; /* content of the macro to expand */
    macro_ptr next;                             /* pointer to the next macro */
} macro_list;

/* Prototypes */
void pre_processor(FILE *, char *);
void read_line_pp(char *, int);
void add_line(char *, char *);
void macro_handler(char *, char *);
void add_macro(macro_ptr *, char *);
status macro_validation(char *mac_name);
macro_ptr check_macro(macro_ptr, char *);
bool is_macro_exist(macro_ptr, char *);
void freelist(macro_ptr *);

#endif
