#ifndef TEXT_ENGINE_H
#define TEXT_ENGINE_H

#include "global.h"
#include "utils.h"
#include <ctype.h>
#include <string.h>

/* Prototypes */
char *skip_spaces(char *);
bool is_ignore_line(char *);
bool is_reserved_word(char *);
bool is_register(char *);
void copy_word(char *word, char *line);
char *next_word(char *);
bool is_end_of_line(char *);
bool is_label(char *word, bool is_w_colon);
char *next_list_token(char *dest, char *line);
bool is_number(char *str);
bool is_string(char *str);
char *next_token_string(char *dest, char *line);

#endif
