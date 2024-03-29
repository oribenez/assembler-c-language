/**
 * @file stage_1.c
 * @brief this file includes all the functions which are related to the first stage of the assembler
 */

#include "stage_1.h"
#include <stdio.h>

/**
 * @brief Function which is managing in high level the first stage.
 * the main purpose of this function is to compile the .am file and to collect all the
 * information which is needed to create a table that from that in the second stage it will be transformed to
 * 32 base code.
 *
 * @param curr_file the current file to compile. it's the .am file.
 * @param filename the file name w/o its extension.
 */
void stage_1(FILE *curr_file, char *filename) {
    char temp_line[MAX_LINE_LENGTH]; /* temporary string for storing line, read from file */
    int line_count = 1;
    ic = dc = 0;
    error_occured_flag = FALSE;

    printf("\n __________________________\n");
    printf("|         STAGE 1#         |\n");
    printf(" ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");

    printf("* Compiling...\n");
    /* Read lines until end of file */
    while (fgets(temp_line, MAX_LINE_LENGTH, curr_file) != NULL) {
        set_error("NO_ERROR");
        read_line_stage_1(temp_line, line_count);

        line_count++; /* increment line counter */
    }

    /* When the first pass ends and the symbols table is complete and IC is evaluated,
       we can calculate real final addresses */
    proceed_addr(symbols_tbl, IC_INIT_ADDR, FALSE);     /* Instruction symbols will have addresses that start from 100 (MEMORY_START) */
    proceed_addr(symbols_tbl, ic + IC_INIT_ADDR, TRUE); /* Data symbols will have addresses that start fron NENORY_START + IC */

    printf("* Finished stage 1.\n");
}

/**
 * @brief Function which gets a line of code and a number of the line,
 * this function checks if there is a label in the line,
 * and if the line is a directive instruction or command instruction.
 * it treats this line according to its identity.
 *
 * @param line string which reperesents a line of code
 * @param line_num  the number of the line in code
 * @return status returns the status of success if there were errors while compiling the code.
 */
status read_line_stage_1(char *line, int line_num) {
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
    if (is_label(curr_word, TRUE)) {
        label_exists = TRUE;
        if (is_end_of_line(line)) {
            throw_err("ONLY_LABEL_IN_LINE", line_num);
            return ERROR;
        }

        strtok(curr_word, ":"); /* trim colon at end of he row */
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
                label_node->address = dc; /* Address of data label is dc */
        }
        line = next_word(line);
        directive_handler(instruction_index, line);
    } else if ((instruction_index = find_command(curr_word)) != NOT_FOUND) {
        if (label_exists) {
            label_node->activeRow = TRUE;
            label_node->address = ic;
        }
        line = next_word(line);
        command_handler(instruction_index, line);
    } else {
        throw_err("INSTRUCTION_NOT_FOUND", line_num);
        return ERROR;
    }

    /* if error occured in commannd handler or in directive handler */
    if (print_error(line_num))
        return ERROR;

    return NO_ERROR;
}

/**
 * @brief function which handles line of code which incl. a directive instruction
 *
 * @param instruction_index the type of the directive instruction
 * @param line string which represents the line
 * @return NO_ERROR if there were no errors. or ERORR if there were error while this function was running.
 */
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

/**
 * @brief function which handles line of code which incl. a command instruction
 *
 * @param instruction_index the type of the directive instruction
 * @param line string which represents the line
 * @return NO_ERROR if there were no errors. or ERORR if there were error while this function was running.
 */
status command_handler(int instruction_index, char *line) {
    bool is_first = FALSE, is_second = FALSE;
    int first_operand_addr_method, second_operand_addr_method;
    char first_operand[OPERAND_MAX_LEN], second_operand[OPERAND_MAX_LEN];

    line = copy_next_li_word(first_operand, line);
    if (!is_end_of_line(first_operand)) /* If first operand is not empty */
    {
        is_first = TRUE; /* First operand exists! */
        line = copy_next_li_word(second_operand, line);
        if (!is_end_of_line(second_operand)) /* If second operand (should hold temporarily a comma) is not empty */
        {
            /* A comma must separate two operands of a command */
            if (second_operand[0] != ',') {
                set_error("COMMAND_UNEXPECTED_CHAR");
                return ERROR;
            }

            else {
                line = copy_next_li_word(second_operand, line);
                if (is_end_of_line(second_operand)) /* If second operand is not empty */
                {
                    set_error("COMMAND_UNEXPECTED_CHAR");
                    return ERROR;
                }
                is_second = TRUE; /* Second operand exists! */
            }
        }
    }

    line = skip_spaces(line);
    if (!is_end_of_line(line)) /* If the line continues after two operands */
    {
        set_error("COMMAND_TOO_MANY_OPERANDS");
        return ERROR;
    }

    if (is_first)
        first_operand_addr_method = get_addr_method(first_operand); /* Detect addressing method of first operand */
    if (is_second)
        second_operand_addr_method = get_addr_method(second_operand); /* Detect addressing method of second operand */

    /* If there was no error while trying to parse addressing methods */
    if (!is_error_exists()) {
        /* If number of operands is valid for this specific command */
        if (command_accept_num_operands(instruction_index, is_first, is_second)) {
            /* If addressing methods are valid for this specific command */
            if (command_accept_methods(instruction_index, first_operand_addr_method, second_operand_addr_method)) {

                /* encode first word of the command to memory and increase ic by the number of additional words */
                write_to_instructions_memory(build_first_word(instruction_index, is_first, is_second, first_operand_addr_method, second_operand_addr_method));
                ic += cmd_calc_num_additive_words(is_first, is_second, first_operand_addr_method, second_operand_addr_method);
            }

            else {
                set_error("COMMAND_INVALID_OPERANDS_METHODS");
                return ERROR;
            }
        } else {
            set_error("COMMAND_INVALID_NUMBER_OF_OPERANDS");
            return ERROR;
        }
    }

    return NO_ERROR;
}

/**
 * @brief function which handles the directive instruction ".data" and add values to data memory
 *
 * @param line string which represents the line
 * @return NO_ERROR if there were no errors. or ERORR if there were error while this function was running.
 */
status data_directive_handler(char *line) {
    bool number_exist = FALSE;
    bool comma_sep_exist = FALSE;
    char token[20];

    while (!is_end_of_line(line)) {
        line = copy_next_li_word(token, line);

        /* Not an empty token */
        if (strlen(token) > 0) {
            if (!number_exist) {         /* if there wasn't a number before */
                if (!is_number(token)) { /* then the token must be a number */
                    set_error("DATA_EXPECTED_NUM");
                    return ERROR;
                } else {
                    number_exist = TRUE;                   /* A valid number was inputted */
                    comma_sep_exist = FALSE;               /* Resetting comma (now it is needed) */
                    write_num_to_data_memory(atoi(token)); /* encoding number to data */
                }
            } else if (*token != ',') { /* If there was a number, now a comma is needed */
                set_error("DATA_EXPECTED_COMMA_AFTER_NUM");
                return ERROR;
            } else { /* If there was a comma, it should be only once (comma should be false) */
                if (comma_sep_exist) {
                    set_error("DATA_COMMAS_IN_A_ROW");
                    return ERROR;
                } else {
                    comma_sep_exist = TRUE;
                    number_exist = FALSE;
                }
            }
        }
    }

    return NO_ERROR;
}

/**
 * @brief function which handles the directive instruction ".string" and add values to data memory.
 * each letter is filling a 'word' in the memory.
 *
 * @param line string which represents the line
 * @return NO_ERROR if there were no errors. or ERORR if there were error while this function was running.
 */
status string_directive_handler(char *line) {
    char token[MAX_LINE_LENGTH];

    line = next_token_string(token, line);
    if (!is_end_of_line(token) && is_string(token)) { /* If token exists and it's a valid string */
        line = skip_spaces(line);
        /* If there's no additional token */
        if (is_end_of_line(line)) {
            /* "Cutting" quotation marks and encoding it to data */
            token[strlen(token) - 1] = '\0';
            write_string_to_data_memory(token + 1);
        } else {
            /* There's another token */
            set_error("STRING_TOO_MANY_OPERANDS");
            return ERROR;
        }
    } else {
        set_error("STRING_OPERAND_NOT_VALID");
        return ERROR;
    }

    return NO_ERROR;
}

/**
 * @brief function which handles the directive instruction ".struct" and add values to data memory.
 * the values are number and a string.
 *
 * @param line string which represents the line
 * @return NO_ERROR if there were no errors. or ERORR if there were error while this function was running.
 */
status struct_directive_handler(char *line) {
    char token[MAX_LINE_LENGTH];
    /* Getting the first token into token array in the line above */
    line = copy_next_li_word(token, line);

    /* First token must be a number */
    if (!is_end_of_line(token) && is_number(token)) {
        write_num_to_data_memory(atoi(token));
        line = copy_next_li_word(token, line); /* Get next token */

        /* There must be a comma between .struct operands */
        if (!is_end_of_line(token) && *token == ',') {
            line = next_token_string(token, line); /* Get next token (second operand) */
            if (!is_end_of_line(token)) {          /* There's a second operand */
                if (is_string(token)) {
                    token[strlen(token) - 1] = '\0';
                    write_string_to_data_memory(token + 1);
                } else {
                    set_error("STRUCT_INVALID_STRING");
                    return ERROR;
                }
            } else {
                set_error("STRUCT_EXPECTED_STRING");
                return ERROR;
            }
        } else {
            set_error("EXPECTED_COMMA_BETWEEN_OPERANDS");
            return ERROR;
        }
    } else {
        set_error("STRUCT_INVALID_NUM");
        return ERROR;
    }
    if (!is_end_of_line(copy_next_li_word(token, line))) {
        set_error("STRUCT_TOO_MANY_OPERANDS");
        return ERROR;
    }

    return NO_ERROR;
}

/**
 * @brief function which handles the directive instruction ".extern".
 *
 * @param line string which represents the line
 * @return NO_ERROR if there were no errors. or ERORR if there were error while this function was running.
 */
status extern_directive_handler(char *line) {
    char token[LABEL_MAX_LEN]; /* This will hold the required label */
    copy_word(token, line);    /* Getting the next token */

    if (is_end_of_line(token)) {
        set_error("EXTERN_NO_LABEL");
        return ERROR;
    }

    /* The token should be a label (without a colon) */
    if (!is_label(token, FALSE)) {
        set_error("EXTERN_INVALID_LABEL");
        return ERROR;
    }

    line = next_word(line);
    if (!is_end_of_line(line)) {
        set_error("EXTERN_TOO_MANY_OPERANDS");
        return ERROR;
    }

    /* Trying to add the label to the symbols table */
    if (insert_label(&symbols_tbl, token, FALSE, TRUE) == NULL)
        return ERROR;

    return NO_ERROR;
}

/**
 * @brief Get the addr method of a given operand,
 *
 * @param operand
 * @return the addressing methos of the operand. if not found return NOT_FOUND.
 */
int get_addr_method(char *operand) {
    char *struct_field; /* When determining if it's a .struct directive, this will hold the part after the dot */

    if (is_end_of_line(operand))
        return NOT_FOUND;

    /* Immediate addressing method check */
    if (*operand == '#') { /* First character is '#' */
        operand++;
        if (is_number(operand))
            return ADDR_IMMEDIATE;
    }

    /* Register addressing method check */
    else if (is_register(operand))
        return ADDR_REGISTER;

    /* Direct addressing method check */
    else if (is_label(operand, FALSE) && strchr(operand, '.') == NULL) { /* Checking if it's a label when there shouldn't be a colon (:) at the end */
        return ADDR_DIRECT;
    }

    /*----- Struct addressing method check -----*/
    else if (is_label(strtok(operand, "."), FALSE)) { /* Splitting by dot character */
        struct_field = strtok(NULL, "");              /* Getting the rest of the string */

        /* After the dot there should be '1' or '2' */
        if (strlen(struct_field) == 1 && (*struct_field == '1' || *struct_field == '2'))
            return ADDR_STRUCT;
    }

    set_error("COMMAND_INVALID_METHOD");
    return NOT_FOUND;
}

/**
 * @brief a function which returns the true if the command method is valid according to given methods
 *
 * @param type the command type
 * @param first_method the first addressing method
 * @param second_method the second addressing method
 * @return true if valid, false if invalid.
 */
bool command_accept_methods(int type, int first_method, int second_method) {
    switch (type) {
    /* SRC: 0,1,2,3.  DEST: 1,2,3. */
    case MOV:
    case ADD:
    case SUB:
        return (first_method == ADDR_IMMEDIATE ||
                first_method == ADDR_DIRECT ||
                first_method == ADDR_STRUCT ||
                first_method == ADDR_REGISTER) &&
               (second_method == ADDR_DIRECT ||
                second_method == ADDR_STRUCT ||
                second_method == ADDR_REGISTER);

    /* SRC: 1,2.  DEST: 1,2,3. */
    case LEA:
        return (first_method == ADDR_DIRECT ||
                first_method == ADDR_STRUCT) &&
               (second_method == ADDR_DIRECT ||
                second_method == ADDR_STRUCT ||
                second_method == ADDR_REGISTER);

    /* SRC: NONE.  DEST: 1,2,3. */
    case NOT:
    case CLR:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case GET:
    case JSR:
        return first_method == ADDR_DIRECT ||
               first_method == ADDR_STRUCT ||
               first_method == ADDR_REGISTER;

    /* SRC: NO_LIMIT.  DEST: NO_LIMIT. */
    case PRN:
    case CMP:
    case RTS:
    case HLT:
        return TRUE;
    }

    return FALSE;
}

/**
 * @brief a function which returns the true if the command method is valid according to the number of given operands
 *
 * @param type type of the operation
 * @param first true if first operand exists, otherwise false.
 * @param second true if second operand exists, otherwise false.
 * @return true if valid, false if invalid.
 */
bool command_accept_num_operands(int type, bool first, bool second) {
    switch (type) {
    /* These opcodes must receive 2 operands */
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        return first && second;

    /* These opcodes must only receive 1 operand */
    case NOT:
    case CLR:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case GET:
    case PRN:
    case JSR:
        return first && !second;

    /* These opcodes can't have any operand */
    case RTS:
    case HLT:
        return !first && !second;
    }
    return FALSE;
}

/**
 * @brief function which returns the number of words according to addresing method requires
 *
 * @param method the addressing method
 * @return int number of words
 */
int num_words_per_addr_method(int method) {
    /* Struct addressing method requires two additional words */
    if (method == ADDR_STRUCT)
        return 2;
    return 1;
}

/**
 * @brief Calculates number of additive words accurdong to a certain command
 * @return unsigned int which represents the first word in the instructions memory.
 */
int cmd_calc_num_additive_words(int is_first, int is_second, int first_method, int second_method) {
    int count = 0;
    if (is_first)
        count += num_words_per_addr_method(first_method);
    if (is_second)
        count += num_words_per_addr_method(second_method);

    /* If both methods are register, they will share the same additional word */
    if (is_first && is_second && first_method == ADDR_REGISTER && second_method == ADDR_REGISTER)
        count--;

    return count;
}

/* This function encodes the first word of the command */
/**
 * @brief function which generates a the first word of command with given params
 *
 * @param type the type of the command
 * @return unsigned int which represents the first word in the instructions memory.
 */
unsigned int build_first_word(int type, int is_first, int is_second, int first_method, int second_method) {
    unsigned int word = 0;

    /* Inserting the opcode */
    word = type;

    word <<= BITS_IN_METHOD; /* Leave space for first addressing method */

    /* If there are two operands, insert the first */
    if (is_first && is_second)
        word |= first_method;

    word <<= BITS_IN_METHOD; /* Leave space for second addressing method */

    /* If there are two operands, insert the second. */
    if (is_first && is_second)
        word |= second_method;
    /* If not, insert the first one (a single operand is a destination operand). */
    else if (is_first)
        word |= first_method;

    word = inject_ARE(word, ABSOLUTE); /* Insert A/R/E mode to the word */

    return word;
}
