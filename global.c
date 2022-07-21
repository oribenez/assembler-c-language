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
    {"LABEL_FIRST_CHAR_IS_LETTER","First character in label has to be letter (upper or lower case)."},
    {"LABEL_ONLY_ALPHANUMERIC", "Label characters can be only lower or Upper case letters, numbers."},
    {"LABEL_MAX_LENGTH", "The label exceeds max label length (MAX: 30 letters not inc. ':')"},
    {"ONLY_LABEL_IN_LINE", "The line of code can't have only label, please complete it."},
    {"INSTRUCTION_NOT_FOUND", "Instruction not found, line must incl. command or directive instruction."},
    {"DIRECTIVE_NO_OPERANDS", "Directive instruction has to have at least one operand."},
    {"DIRECTIVE_INVALID_NUM_PARAMS", "invalid num operands"},
    {"LABEL_INSERT_FAILED", "Label insertion has been failed"},
    {"LABEL_ALREADY_EXISTS", "Label already exists"},
    {"UNDEFINED", "Undefined error."}};

char *curr_error_key = "NO_ERROR";
bool entry_exists, extern_exists;
label_ptr symbols_tbl;

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
    curr_error_key = (char *)malloc_with_check(strlen(err_key) + 1);
    strcpy(curr_error_key, err_key);
}

void throw_err(char *err_key, int line_num) {
    set_error(err_key);
    print_error(line_num);
}
