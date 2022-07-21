#include "stage_1.h"
#include <stdio.h>

void stage_1(FILE *curr_file, char *filename, machine_word **code_img, long *data_img, unsigned int *ic, unsigned int *dc) {
    char temp_line[MAX_LINE_LENGTH]; /* temporary string for storing line, read from file */
    int line_count = 1;

    printf("#Starting stage 1.\n");
    /* Read lines until end of file */
    while (fgets(temp_line, MAX_LINE_LENGTH, curr_file) != NULL) {
        set_error("NO_ERROR");
        read_line_stage_1(temp_line, line_count, code_img, data_img, ic, dc);
        line_count++; /* increment line counter */
    }

    printf("#Finished stage 1.\n");
}

status read_line_stage_1(char *line, int line_num, machine_word **code_img, long *data_img, unsigned int *ic, unsigned int *dc) {
    char curr_word[MAX_LINE_LENGTH];
    label_ptr label_node = NULL;
    bool label_exists = FALSE;
    int instruction_index = 0;

    /* Ignore line if it's blank or a comment */
    if (is_ignore_line(line))
        return NO_ERROR; /* skip to next line */

    line = skip_spaces(line);
    copy_word(curr_word, line);

    /* check if first word is a label */
    if (is_label(curr_word)) {
        label_exists = TRUE;
        if (is_end_of_line(line)) {
            throw_err("ONLY_LABEL_IN_LINE", line_num);
            return ERROR;
        }

        /* Add label to symbols table */
        label_node = insert_label(&symbols_tbl, curr_word, 0, FALSE, FALSE);
        if (print_error(line_num))
            return ERROR; /* check for errors in internal function like is_label */

        if (label_node == NULL) { /* There was an error creating label */
            throw_err("LABEL_INSERT_FAILED", line_num);
            return ERROR;
        }

        /* continue to next word */
        line = next_word(line);
        copy_word(curr_word, line);
    }

    if (print_error(line_num))
        return ERROR; /* check for errors in internal function like is_label */

    /* check if instruction is of type directive */
    if ((instruction_index = find_directive(curr_word)) != NOT_FOUND) {
        if (label_exists) {
            if (instruction_index == EXTERN || instruction_index == ENTRY) { /* we need to ignore creation of label before .entry/.extern */
                delete_label(&symbols_tbl, label_node->name);
                label_exists = FALSE;
            } else
                label_node->address = *dc; /* Address of data label is dc */
        }
        line = next_word(line);
        directive_handler(instruction_index, line);
    } else if ((instruction_index = find_command(curr_word)) != NOT_FOUND) {
        if (label_exists) {
            label_node->inActionStatement = TRUE;
            label_node->address = *ic;
        }
        line = next_word(line);
        command_handler(instruction_index, line);
    } else {
        throw_err("INSTRUCTION_NOT_FOUND", line_num);
        return ERROR;
    }

    return NO_ERROR;
}

/**
 * @brief search index for a given directive name in directive array
 *
 * @param word the directive to check
 * @return int directive index in array. NOT_FOUND if not found (-1)
 */
int find_directive(char *word) {
    int i;
    for (i = 0; i < NUM_DIRECTIVES; i++) {
        if (!strcmp(directives[i], word))
            return i;
    }

    return NOT_FOUND;
}

/**
 * @brief search index for a given command name in commands array
 *
 * @param word the command to check
 * @return int command index in array. NOT_FOUND if not found (-1)
 */
int find_command(char *word) {
    int i;
    for (i = 0; i < NUM_COMMANDS; i++) {
        if (!strcmp(commands[i], word))
            return i;
    }

    return NOT_FOUND;
}

status directive_handler(int instruction_index, char *line) {
    /* check if this directive instruction have at least one operand  */
    if (line == NULL || is_end_of_line(line)) {
        set_error("DIRECTIVE_NO_OPERANDS");
        return ERROR;
    }

    switch (instruction_index) {
    case DATA:
        return data_directive_handler(line);

    case STRING:
        return string_directive_handler(line);

    case STRUCT:
        return struct_directive_handler(line);

    case ENTRY:
        /*check if there is one operand only*/
        if (!is_end_of_line(next_word(line))) {
            set_error("DIRECTIVE_INVALID_NUM_PARAMS");
            return ERROR;
        }
        break;

    case EXTERN:
        return extern_directive_handler(line);
    }

    return NO_ERROR;
}

void command_handler(int instruction_index, char *line) {
}

/* Handle .data directive and insert values separated by comma to the memory */
status data_directive_handler(char *line) {
    return NO_ERROR;
}

/* Handle .string directive and insert all characters (including a '\0') to memory */
status string_directive_handler(char *line) {
    return NO_ERROR;
}

/* Handle .struct directive and insert both number and string to memory */
status struct_directive_handler(char *line) {
    return NO_ERROR;
}

/* Handle .extern directive */
status extern_directive_handler(char *line) {
    return NO_ERROR;
}
