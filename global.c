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
    {NO_ERROR, ""},
    {"LABEL_ONLY_ALPHA", "Label characters can be only lower or Upper case letters, without numbers or any signs."},
    {"LABEL_MAX_LEN", "The label exceeds max label length (MAX: 30 letters not inc. ':')"},
    {"ONLY_LABEL_IN_LINE", "The line of code can't have only label, please complete it."},
    {"INSTRUCTION_NOT_FOUND","Instruction not found, line must incl. command or directive instruction."},
    {UNDEFINED, "Undefined"}};

char* curr_error_key = NO_ERROR;



/**
 * @return true if error exists in the global err variable, otherwise false.
 */
bool is_error_exists() {
    return strcmp(curr_error_key, NO_ERROR);
}

/**
 * @brief prints error if exists in the global err variable
 * 
 * @param line_num the line number in which the error occured
 * @return true if there was an error, otherwise false.
 */
bool print_error(int line_num) {
    err* err_ptr = (err*)errors;
    
    if (is_error_exists()) {
        do{
            err_ptr++;
        }
        while(strcmp(err_ptr->key, curr_error_key) && strcmp(err_ptr->key, UNDEFINED));

        printf("ERROR: line %d , Message: %s", line_num, err_ptr->message);
        return TRUE;
    }

    return FALSE;
}

void set_error(char* err_key){
    curr_error_key = strallocat(curr_error_key, err_key);
}

void throw_err(char* err_key, int line_num){
    set_error(err_key);
    print_error(line_num);
}

