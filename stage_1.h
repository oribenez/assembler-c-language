#ifndef STAGE_1_H
#define STAGE_1_H

#include "global.h"
#include <stdio.h>

/* Declarations */


/* Prototypes */
void stage_1(FILE *curr_file, char *filename, machine_word **code_img, long *data_img, long *IC, long *DC);
bool read_line_stg1(char *line, int line_num, machine_word **code_img, long *data_img, long *IC, long *DC);
int find_directive(char *word);
int find_command(char *word);
void directive_handler(int instruction_index, char* line);
void command_handler(int instruction_index, char* line);

#endif
