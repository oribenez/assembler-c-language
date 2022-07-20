#include "stage_1.h"
#include "text_engine.h"

void stage_1(FILE *curr_file, char *filename, machine_word **code_img, long *data_img, long *ic, long *dc) {
    char temp_line[MAX_LINE_LENGTH]; /* temporary string for storing line, read from file */
    int line_count = 1;

    printf("#Starting stage 1.\n");
    /* Read lines until end of file */
    while (fgets(temp_line, MAX_LINE_LENGTH, curr_file) != NULL) {
        read_line_stg1(temp_line, line_count, code_img, data_img, ic, dc);
        line_count++; /* increment line counter */
    }
}

bool read_line_stg1(char *line, int line_num, machine_word **code_img, long *data_img, long *ic, long *dc) {
    char curr_word[MAX_LINE_LENGTH];
    label_ptr label_node = NULL;
    bool label_exists = FALSE;
    int instruction_index = 0;

    /* Ignore line if it's blank or a comment */
    if (is_ignore_line(line))
        return TRUE; /* skip to next line */

    line = skip_spaces(line);
    copy_word(curr_word, line);

    /* check if first word is a label */
    if (is_label(curr_word)) {
        label_exists = TRUE;
        if (end_of_line(line)) {
            throw_err("ONLY_LABEL_IN_LINE", line_num); 
            return FALSE;
        }
        
        /* Add label to symbols table */
        label_node = insert_label(&symbols_tbl, curr_word, 0, FALSE, FALSE);
        if(label_node == NULL) /* There was an error creating label */
            throw_err("UNDEFINED", line_num); 
            return FALSE;
    }

    if (print_error(line_num))
        return FALSE; /* check for errors in internal function like is_label */

    /* continue to next word */
    line = next_word(line);
    copy_word(curr_word, line);

    if((instruction_index = find_directive(curr_word)) != NOT_FOUND){
        if(label_exists) {
            if(dir_type == EXTERN || dir_type == ENTRY) { /* we need to ignore creation of label before .entry/.extern */
                delete_label(&symbols_table, label_node->name);
                label_exists = FALSE;
            }
            else
                label_node -> address = dc; /* Address of data label is dc */
        }
        line = next_word(line);
        directive_handler(instruction_index, line);
    } else if((instruction_index = find_command(curr_word)) != NOT_FOUND){
        if(label_exists) {
            label_node -> inActionStatement = TRUE; 
            label_node -> address = ic;
        }
        line = next_word(line);
        command_handler(instruction_index, line);
    } else{
        throw_err("INSTRUCTION_NOT_FOUND");
        return FALSE;
    }
    

    printf("%s", line);

    return TRUE;
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
        if (!strcmp(directives[i], word)) return i;
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
        if (!strcmp(commands[i], word)) return i;
    }

    return NOT_FOUND;
}


void directive_handler(int instruction_index, char* line){

}


void command_handler(int instruction_index, char* line){
    
}



