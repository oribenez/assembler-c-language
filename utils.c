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
char *str_alloc_concat(char *s0, char *s1) {
    char *str = (char *)malloc_w_check(strlen(s0) + strlen(s1) + 1);
    strcpy(str, s0);
    strcat(str, s1);
    return str;
}

/**
 * Allocates memory in the required size. Exits the program if failed.
 * @param size The size to allocate in bytes
 * @return A generic pointer to the allocated memory if succeeded
 */
void *malloc_w_check(long size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        printf("Error: Fatal: Memory allocation failed.");
        exit(1);
    }
    return ptr;
}

/**
 * @brief function which insert a number to the data_memory
 * 
 * @param number the number to insert
 */
void write_num_to_data_memory(int number) {
    data_memory[dc++] = (unsigned int)number;
}

/* This function encodes a given string to data */
/**
 * @brief function which write a string to the data_memory
 * note: each character is a 'word' 10 bits as in the booklet.
 * 
 * @param str the string to insert to data_memory
 */
void write_string_to_data_memory(char *str) {
    while (!is_end_of_line(str)) {
        data_memory[dc++] = (unsigned int)*str;
        str++;
    }
    data_memory[dc++] = '\0';/* close string */
}

/**
 * @brief function which inserts a given word to the instructions_memory
 * 
 * @param word the word to insert to memory
 */
void write_to_instructions_memory(unsigned int word) {
    instr_memory[ic++] = word;
}

/**
 * @brief function which injects a given ARE to a given number called info.
 * 
 * @param info the number to insert to it a given ARE
 * @param are the ARE to insert
 * @return unsigned int the number with the new ARE
 */
unsigned int inject_ARE(unsigned int info, int ARE) {
    return ARE | (info << BITS_IN_ARE);
}

/**
 * @brief function which gets a number which represents a word and convert it to 2 digits in base 32.
 * 
 * @param num the number to convert
 * @return pointer to a string which represents the new converted base 32 word.
 */
char *convert_to_base_32(unsigned int num) {
    char *base32_token = (char *)malloc_w_check(BASE32_SEQUENCE_LENGTH);

    /* convert five bits at a time to letter in 32 base */
    base32_token[0] = base32[extract_bits(num, 5, 9)];
    base32_token[1] = base32[extract_bits(num, 0, 4)];
    base32_token[2] = '\0';

    return base32_token;
}

/**
 * @brief function which cuts a word which represents a number, and return the cutted number
 * 
 * @param word word to cut 
 * @param start starting index
 * @param end  end index
 * @return unsigned int which represents a number
 */
unsigned int extract_bits(unsigned int word, int start, int end) {
    unsigned int result;
    int length = end - start + 1;                /* Length of bit-sequence */
    unsigned int mask = (int)pow(2, length) - 1; /* Creating a '111...1' mask with above line's length */

    mask <<= start;       /* Moving mask to place of extraction */
    result = word & mask; /* The bits are now in their original position, and the rest is 0's */
    result >>= start;     /* Moving the sequence to LSB */
    return result;
}

/**
 * @brief Create a file object with a given filename and type
 * 
 * @param filename the file name of the new file to create
 * @param type the type of the file, the end extension filename, such as FILE_MACRO.
 * @return FILE* returns a pointer to the new file
 */
FILE *create_file(char *filename, int type) {
    FILE *fd;
    char *filename_w_ext;
    /* Creating filename with extension */
    filename_w_ext = generate_file_name(filename, type);

    fd = fopen(filename_w_ext, "w");
    free(filename_w_ext);

    if (fd == NULL) {
        printf("Failed creating file");
        return NULL;
    }
    return fd;
}

/**
 * @brief function which concatenating filename with a given type of file 
 * 
 * @param original filename to concat
 * @param type the type of the file to add it's extension to the filename
 * @return char* pointer to the string which will represent the filename
 */
char *generate_file_name(char *original, int type) {
    char *new_name;
    new_name = (char *)malloc_w_check(strlen(original) + EXT_MAX_LEN);
    strcpy(new_name, original); 

    /* concat file extension */
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
