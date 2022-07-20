#ifndef TEXT_ENGINE_H
#define TEXT_ENGINE_H

#include "global.h"
#include <ctype.h>
#include <string.h>

/* Prototypes */
char *skip_spaces(char *);
bool is_ignore_line(char *);
bool is_reserved_word(char *);
bool is_register(char *);
void copy_word(char *word, char *line);
char *next_word(char *);
int end_of_line(char *);
bool is_label(char *word);


#endif
