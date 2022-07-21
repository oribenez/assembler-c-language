#include "pre_processor.h"
#include "global.h"
#include "text_engine.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool reading_macro;
macro_ptr curr_macro;
macro_ptr macro_pointer;
FILE *macro_file;

/* expands macros */
void pre_processor(FILE *curr_file, char *filename) {
    char *input_filename;
    char temp_line[MAX_LINE_LENGTH]; /* temporary string for storing line, read from file */
    int line_count = 1;

    printf("#Start Pre assembler.\n");

    reading_macro = FALSE;
    input_filename = strallocat(filename, ".am");
    macro_file = fopen(input_filename, "w");
    curr_macro = NULL;

    /* Read lines until end of file */
    printf("#Expanding macros(if exists).\n");
    while (fgets(temp_line, MAX_LINE_LENGTH, curr_file) != NULL) {
        read_line_pp(temp_line, line_count);
        line_count++; /* increment line counter */
    }

    freelist(&curr_macro);
    free(input_filename);
    fclose(macro_file);

    printf("#Pre assembler finsihed.\n");
}

/* parse row */
void read_line_pp(char *line, int line_num) {

    char word[MAX_LINE_LENGTH];
    char *all_line = line;

    line = skip_spaces(line);
    copy_word(word, line);

    /* if first word is label, check the next one */
    if (is_label(word)) {
        /* check next word for macro */
        line = next_word(line);
        copy_word(word, line);
    }
    if (reading_macro) { /* if it's macro we will just add the lines to the macro table until endmacro */
        /* finish macro reading */
        if (!strcmp(word, "endmacro")) {
            reading_macro = FALSE;
            return;
        }
        /* add line to macro content */
        strcat(curr_macro->content, all_line);
    } else { /* Check for start of macro */
        macro_handler(word, line);
        if (!reading_macro) {
            add_line(all_line, word);
        }
    }
    
}

void macro_handler(char *word, char *line) {
    if (!strcmp(word, "macro")) { /* enter if macro */
        reading_macro = TRUE;

        /* get the name of the macro and insert to Macro list */
        line = next_word(line);
        copy_word(word, line);

        add_macro(&curr_macro, word);
    }
}

/**
 * @brief check if a given macro name is valid (check if reserved word)
 *
 * @param mac_name macro name
 * @return TRUE, if macro is *VALID*
 * @return FALSE, if macro is *INVALID*
 */
status macro_validation(char *mac_name) {
    if(is_macro_exist(curr_macro, mac_name)) return INVALID;

    /* check if macro is command name */
    if(is_reserved_word(mac_name)) return INVALID; 
    
    return VALID;
}

/* Function used to add a new macro name to the macro table we have */
void add_macro(macro_ptr *macroTable, char *macroName) {
    macro_ptr ptr1, ptr2;
    if (macro_validation(macroName)) {
        ptr1 = (macro_ptr)malloc_with_check(sizeof(macro_list));

        /* Save the new macro as a node in our table */
        strcpy(ptr1->name, macroName);
        if (*macroTable == NULL) /* Init Macro list: if table empty */
            *macroTable = ptr1;
        else { /* Add macro to existing list: add the new macro as the last node in table */
            ptr2 = *macroTable;
            *macroTable = ptr1;
            ptr1->next = ptr2;
        }
    }
}

/**
 * @brief insert a given line to the macro file, or expand macros if it's a macro callback.
 *
 * @param line the current line
 * @param word the current word in the given line
 */
void add_line(char *line, char *word) {
    /* add line depend if it's macro name or regular code */
    if ((macro_pointer = check_macro(curr_macro, word)) != NULL) {
        /* Expand macro content */
        fputs(macro_pointer->content, macro_file);
    } else {
        /* place the code as is! */
        fputs(line, macro_file);
    }
}

/**
 * @brief checks if there is existing macro in the list that match to the macro name .
 *
 * @param macroTable macro list to search
 * @param word macro name
 * @return macro_ptr return a pointer to the existing macro. NULL if doesn't exist.
 */
macro_ptr check_macro(macro_ptr macroTable, char *word) {
    macro_ptr ptr1;
    ptr1 = macroTable;

    while (ptr1 != NULL) {
        if (!strcmp(ptr1->name, word))
            return ptr1;
        ptr1 = ptr1->next;
    }
    return NULL;
}

/**
 * @brief checks if there is existing macro in the list that match to the macro name .
 *
 * @param macroTable macro list to search
 * @param word macro name
 * @return macro_ptr return a pointer to the existing macro. NULL if doesn't exist.
 */
bool is_macro_exist(macro_ptr macroTable, char *mac_name) {
    return check_macro(macroTable, mac_name) == NULL ? FALSE : TRUE;
}

/* Free the memory we allocated for the macro list */
void freelist(macro_ptr *macroTable) {

    macro_ptr p;
    while (*macroTable) {
        p = *macroTable;
        *macroTable = (*macroTable)->next;
        free(p);
    }
}
