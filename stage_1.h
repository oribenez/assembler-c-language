#ifndef STAGE_1_H
#define STAGE_1_H

#include "global.h"
#include "text_engine.h"
#include "labelsLinkedList.h"
#include <stdio.h>

/* Declarations */


/* Prototypes */
void stage_1(FILE *curr_file, char *filename, machine_word **code_img, long *data_img, unsigned int *ic, unsigned int *dc);
status read_line_stage_1(char *line, int line_num, machine_word **code_img, long *data_img, unsigned int *ic, unsigned int *dc);
int find_directive(char *word);
int find_command(char *word);
status directive_handler(int instruction_index, char* line);
void command_handler(int instruction_index, char* line);
status data_directive_handler(char *line);
status string_directive_handler(char *line);
status struct_directive_handler(char *line);
status extern_directive_handler(char *line);

#endif
