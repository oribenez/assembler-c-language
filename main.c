/**
 * @file main.c
 * @author Ori Ben-Ezra (https://oribenezra.com/)
 * @brief  main file which controls all app switches 
 * such as, pre-processor, stage 1, stage 2.
 * @version 0.1
 * @date 2022-07-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "pre_processor.h"
#include "stage_1.h"
#include "stage_2.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

static status process_file(char *filename, int file_count);

/**
 * @brief calling assembler to interpret the given files in args.
 */
int main(int argc, char const *argv[]) {

    int i;
    status succeeded = NO_ERROR;
    printf("\nLets do it!\n");

    /* Check if the user entered mandatory filenames */
    if (argc <= 1) {
        printf("\nYou must specify file name in command line!\n");
        exit(0);
    }

    for (i = 1; i < argc; i++) {
        succeeded = process_file((char *)argv[i], i);
        if (!succeeded)
            printf("The assembler failed on file: %s", argv[i]);
    }

    return 0;
}

/**
 * Processes a single assembly source file, and returns the result status.
 * @param filename The filename, without it's extension
 * @param file_count the number of the file in order.
 * @return Whether succeeded or not.
 */
static status process_file(char *filename, int file_count) {
    char *input_filename;
    FILE *fd; /* Current assembly file descriptor to process */
    entry_exists = FALSE;
    extern_exists = FALSE;
    error_occured_flag = FALSE;
    ext_list = NULL;
    symbols_tbl = NULL;

    /* add filename extension, ".as" */
    input_filename = strallocat(filename, ".as");

    /* title */
    printf("\n\n ___\n");
    printf("|#%2d| File: %s                \n", file_count, input_filename);
    printf(" ‾‾‾  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾");

    fd = fopen(input_filename, "r");
    if (fd == NULL) {
        /* file couldn't be opened. */
        printf("Error: There is a problem with the file \"%s.as\". skipping to the next one... \n", filename);
        free(input_filename);
        return FAILED;
    }

    /* Pre-processor: expanding macros */
    pre_processor(fd, filename);

    /* cleanup before next stage */
    fclose(fd);

    /* open .am file with macros */
    input_filename = strallocat(filename, ".am");
    fd = fopen(input_filename, "r");
    if (fd == NULL) {
        /* file couldn't be opened. */
        printf("Error: There is a problem with the file \"%s.as\". skipping to the next one... \n", filename);
        free(input_filename);
        return FAILED;
    }

    /* Stage 1: Compiler  */
    stage_1(fd, filename);

    /* Stage 2: Wrapper */
    if (!error_occured_flag) {
        rewind(fd);
        stage_2(fd, filename);
    }

    printf("\n\nClosing file '%s'\n",filename);
    printf("‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");

    fclose(fd);
    free(input_filename);
    
    return SUCCESS;
}
