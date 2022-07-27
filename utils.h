#ifndef _UTILS_H
#define _UTILS_H

#include "global.h"
#include "text_engine.h"
#include <stdio.h>

/* Declarations */
#define ERR_OUTPUT_FILE stderr
#define BASE32_SEQUENCE_LENGTH 3

enum filetypes { FILE_INPUT,
                 FILE_MACRO,
                 FILE_OBJECT,
                 FILE_ENTRY,
                 FILE_EXTERN };

/* Prototypes */
char *str_alloc_concat(char *s0, char *s1);
void *malloc_w_check(long size);
void write_num_to_data_memory(int number);
void write_string_to_data_memory(char *str);
void write_to_instructions_memory(unsigned int word);
unsigned int inject_ARE(unsigned int info, int are);
char *convert_to_base_32(unsigned int num);
unsigned int extract_bits(unsigned int word, int start, int end);
FILE *create_file(char *filename, int type);
char *generate_file_name(char *original, int type);
int find_directive(char *word);
int find_command(char *word);

#endif
