/**
 * @file global.c
 * @author Ori Ben Ezra
 * @brief handles all global variables and functions related to the assembler
 * @version 0.1
 * @date 2022-07-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "global.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

const char *commands[NUM_COMMANDS] = {
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne",
    "get", "prn", "jsr", "rts", "hlt"};
const char *directives[NUM_DIRECTIVES] = {
    ".data", ".string", ".struct", ".entry", ".extern"};
const char base32[BASE_NUMBER] = {
    '!', '@', '#', '$', '%', '^', '&', '*', '<', '>', 'a', 'b', 'c',
    'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v'};
const err errors[] = {
    {"NO_ERROR", ""},
    {"LABEL_FIRST_CHAR_IS_LETTER", "First character in label has to be letter (upper or lower case)."},
    {"LABEL_ONLY_ALPHANUMERIC", "Label characters can be only lower or Upper case letters, numbers."},
    {"LABEL_MAX_LENGTH", "The label exceeds max label length (MAX: 30 letters not inc. ':')"},
    {"ONLY_LABEL_IN_LINE", "The line of code can't have only label, please complete it."},
    {"INSTRUCTION_NOT_FOUND", "Instruction not found, line must incl. command or directive instruction."},
    {"DIRECTIVE_NO_OPERANDS", "Directive instruction has to have at least one operand."},
    {"DIRECTIVE_INVALID_NUM_PARAMS", "invalid num operands."},
    {"LABEL_INSERT_FAILED", "Label insertion has been failed."},
    {"LABEL_ALREADY_EXISTS", "Label already exists."},
    {"DATA_EXPECTED_NUM", ".data directive is expecting a number in the operand."},
    {"DATA_COMMAS_IN_A_ROW", "Too many commas."},
    {"STRING_TOO_MANY_OPERANDS", "Too many operands."},
    {"STRING_OPERAND_NOT_VALID", "String operand is invalid."},
    {"STRUCT_INVALID_STRING", "String is invalid in struct"},
    {"EXTERN_NO_LABEL", " No label in extern"},
    {"EXTERN_INVALID_LABEL", " Invalid label"},
    {"EXTERN_TOO_MANY_OPERANDS", "Too many operands"},
    {"COMMAND_UNEXPECTED_CHAR", "invalid token"},
    {"COMMAND_LABEL_DOES_NOT_EXIST","Label doesn't exist."},
    {"FAILED_OPEN_FILE", "failed to create and open new file."},
    {"", ""},
    {"UNDEFINED", "Undefined error."}};
/*{"",""},*/

char *curr_error_key = "NO_ERROR";
bool entry_exists, extern_exists;
label_ptr symbols_tbl;
unsigned int data_memory[IMAGE_MEM_SIZE];
unsigned int instr_memory[IMAGE_MEM_SIZE];
int ic, dc;
bool error_occured_flag;
ext_ptr ext_list;

/**
 * @return true if error exists in the global err variable, otherwise false.
 */
bool is_error_exists() {
    return strcmp(curr_error_key, "NO_ERROR");
}

/**
 * @brief prints error if exists in the global err variable
 *
 * @param line_num the line number in which the error occured
 * @return true if there was an error, otherwise false.
 */
bool print_error(int line_num) {
    err *err_ptr = (err *)errors;

    if (is_error_exists()) {
        do {
            err_ptr++;
        } while (strcmp(err_ptr->key, curr_error_key) && strcmp(err_ptr->key, "UNDEFINED"));

        printf("\n#ERROR:(line %d) %s, Message: %s\n", line_num, err_ptr->key, err_ptr->message);
        return TRUE;
    }

    return FALSE;
}

void set_error(char *err_key) {
    if(strcmp(err_key,"NO_ERROR")) error_occured_flag = TRUE;
    curr_error_key = (char *)malloc_with_check(strlen(err_key) + 1);
    strcpy(curr_error_key, err_key);
}

void throw_err(char *err_key, int line_num) {
    set_error(err_key);
    print_error(line_num);
}
