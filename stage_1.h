#ifndef STAGE_1_H
#define STAGE_1_H

#include "global.h"
#include "text_engine.h"
#include "labels_linked_list.h"
#include <stdio.h>

/* Declarations */


/* Prototypes */
void stage_1(FILE *curr_file, char *filename);
status read_line_stage_1(char *line, int line_num);
status directive_handler(int instruction_index, char* line);
status command_handler(int instruction_index, char* line);
status data_directive_handler(char *line);
status string_directive_handler(char *line);
status struct_directive_handler(char *line);
status extern_directive_handler(char *line);
int cmd_calc_num_additive_words(int is_first, int is_second, int first_method, int second_method);
bool command_accept_num_operands(int type, bool first, bool second);
bool command_accept_methods(int type, int first_method, int second_method);
int get_addr_method(char *operand);
int num_words_per_addr_method(int method);
unsigned int build_first_word(int type, int is_first, int is_second, int first_method, int second_method);

#endif
