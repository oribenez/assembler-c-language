#include "pre_processor.h"
#include "stage_1.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


static bool process_file(char *filename);

int main(int argc, char const *argv[]) {

    int i;
    bool succeeded = TRUE;
    printf("\nLets do it!\n");

    /* Check if the user entered mandatory filenames */
    if (argc <= 1) {
        printf("\nYou must specify file name in command line!\n");
        exit(0);
    }

    for (i = 1; i < argc; i++) {
        succeeded = process_file((char *)argv[i]);
        if(!succeeded) printf("The assembler failed on file: %s", argv[i]);
    } 

    return 0;
}

/**
 * Processes a single assembly source file, and returns the result status.
 * @param filename The filename, without it's extension
 * @return Whether succeeded
 */
static bool process_file(char *filename) {
    long ic = 0, dc = 0;
    char *input_filename;
    FILE *fd;                           /* Current assembly file descriptor to process */
    long data_img[CODE_ARR_IMG_LENGTH]; /* Contains an image of the machine code */
    machine_word *code_img[CODE_ARR_IMG_LENGTH];

    /* add filename extension, ".as" */
    input_filename = strallocat(filename, ".as");

    fd = fopen(input_filename, "r");
    if (fd == NULL) {
        /* file couldn't be opened. */
        printf("Error: There is a problem with the file \"%s.as\". skipping to the next one... \n", filename);
        free(input_filename);
        return FALSE;
    }

    /* Pre-processor: expanding macros */
    pre_processor(fd, filename);

    /* cleanup before next stage */
    fclose(fd);

    /* open .am file with macros */
    input_filename = strallocat(filename, ".am");
    fopen(input_filename, "r");
    if (fd == NULL) {
        /* file couldn't be opened. */
        printf("Error: There is a problem with the file \"%s.as\". skipping to the next one... \n", filename);
        free(input_filename);
        return FALSE;
    }
    
    /* Stage 1: Compiler  */
    stage_1(fd, filename, code_img, data_img, &ic, &dc);
    

    fclose(fd);
    free(input_filename);
    return TRUE;
}
