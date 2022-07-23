#ifndef STAGE_2_H
#define STAGE_2_H

#include "global.h"
#include "labels_linked_list.h"
#include "text_engine.h"
#include "utils.h"
#include "external_linked_list.h"
#include <stdio.h>

void stage_2(FILE *curr_file, char *filename);
status read_line_stage_2(char *line, int line_num);
int command_handler_stage_2(int type, char *line);
int generate_output_files(char *original);
void write_output_ob(FILE *fp);
void write_output_entry(FILE *fp);
void write_output_extern(FILE *fp);
void check_operands_exist(int type, bool *is_src, bool *is_dest);
int encode_additional_words(char *src, char *dest, bool is_src, bool is_dest, int src_method, int dest_method);
unsigned int build_register_word(bool is_dest, char *reg);
void encode_additional_word(bool is_dest, int method, char *operand);

#endif
