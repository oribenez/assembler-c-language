#include "utils.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Concatenates both string to a new allocated memory
 * @param s0 The first string
 * @param s1 The second string
 * @return A pointer to the new, allocated string
 */
char *strallocat(char *s0, char *s1) {
    char *str = (char *)malloc_with_check(strlen(s0) + strlen(s1) + 1);
    strcpy(str, s0);
    strcat(str, s1);
    return str;
}

/**
 * Allocates memory in the required size. Exits the program if failed.
 * @param size The size to allocate in bytes
 * @return A generic pointer to the allocated memory if succeeded
 */
void *malloc_with_check(long size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        printf("Error: Fatal: Memory allocation failed.");
        exit(1);
    }
    return ptr;
}

/* This function encodes a given number to data */
void write_num_to_data_memory(int number) {
    data_memory[dc++] = (unsigned int)number;
}

/* This function encodes a given string to data */
void write_string_to_data_memory(char *str) {
    while (!is_end_of_line(str)) {
        data_memory[dc++] = (unsigned int)*str; /* Inserting a character to data array */
        str++;
    }
    data_memory[dc++] = '\0'; /* Insert a null character to data */
}

/* This function inserts a given word to instructions memory */
void write_to_instructions_memory(unsigned int word) {
    instr_memory[ic++] = word;
}

/* This function inserts given A/R/E 2 bits into given info bit-sequence (the info is being shifted left) */
unsigned int inject_ARE(unsigned int info, int are) {
    return (info << BITS_IN_ARE) | are; /* OR operand allows insertion of the 2 bits because 1 + 0 = 1 */
}
