#include "utils.h"
#include <ctype.h>
#include <math.h>
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

/* Converting a word to 2 digits in base 32 (as a string) */
char *convert_to_base_32(unsigned int num) {
    char *base32_seq = (char *)malloc_with_check(BASE32_SEQUENCE_LENGTH);

    /* To convert from binary to base 32 we can just take the 5 right binary digits and 5 left */
    base32_seq[0] = base32[extract_bits(num, 5, 9)];
    base32_seq[1] = base32[extract_bits(num, 0, 4)];
    base32_seq[2] = '\0';

    return base32_seq;
}

/* This function extracts bits, given start and end positions of the bit-sequence (0 is LSB) */
unsigned int extract_bits(unsigned int word, int start, int end) {
    unsigned int result;
    int length = end - start + 1;                /* Length of bit-sequence */
    unsigned int mask = (int)pow(2, length) - 1; /* Creating a '111...1' mask with above line's length */

    mask <<= start;       /* Moving mask to place of extraction */
    result = word & mask; /* The bits are now in their original position, and the rest is 0's */
    result >>= start;     /* Moving the sequence to LSB */
    return result;
}

/* This function opens a file with writing permissions, given the original input filename and the
 * wanted file extension (by type)
 */
FILE *create_file(char *filename, int type) {
    FILE *fd;
    char *filename_w_ext;

    filename_w_ext = generate_file_name(filename, type); /* Creating filename with extension */

    fd = fopen(filename_w_ext, "w"); /* Opening file with permissions */
    free(filename_w_ext);            /* Allocated modified filename is no longer needed */

    if (fd == NULL) {
        printf("Failed creating file");
        return NULL;
    }
    return fd;
}

/* This function creates a file name by appending suitable extension (by type) to the original string */
char *generate_file_name(char *original, int type) {
    char *new_name;
    new_name = (char *)malloc_with_check(strlen(original) + EXT_MAX_LEN);

    strcpy(new_name, original); /* Copying original filename to the bigger string */

    /* Concatenating the required file extension */

    switch (type) {
    case FILE_INPUT:
        strcat(new_name, ".as");
        break;

    case FILE_MACRO:
        strcat(new_name, ".am");
        break;

    case FILE_OBJECT:
        strcat(new_name, ".ob");
        break;

    case FILE_ENTRY:
        strcat(new_name, ".ent");
        break;

    case FILE_EXTERN:
        strcat(new_name, ".ext");
    }
    return new_name;
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
