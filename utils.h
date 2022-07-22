#ifndef _UTILS_H
#define _UTILS_H

#include "global.h"
#include "text_engine.h"

/* Declarations */
#define ERR_OUTPUT_FILE stderr

/* Prototypes */
char *strallocat(char *s0, char *s1);
void *malloc_with_check(long size);
void write_num_to_data_memory(int number);
void write_string_to_data_memory(char *str);
void write_to_instructions_memory(unsigned int word);
unsigned int inject_ARE(unsigned int info, int are);

#endif
