#include "stage_2.h"
#include <stdio.h>

void stage_2(FILE *curr_file, char *filename) {
    char temp_line[MAX_LINE_LENGTH]; /* temporary string for storing line, read from file */
    int line_count = 1;
    ic = 0;

    printf("\n __________________________\n");
    printf("|         STAGE 2#         |\n");
    printf(" ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n");

    printf("* Wrapping it up...\n");
    /* Read lines until end of file */
    while (fgets(temp_line, MAX_LINE_LENGTH, curr_file) != NULL) {
        set_error("NO_ERROR");
        read_line_stage_2(temp_line, line_count);

        line_count++;
    }

    /*create output files only if there were no errors at the process*/
    if (!error_occured_flag) {
        generate_output_files(filename);
    }

    printf("* Finished stage 2.");

    free_labels(&symbols_tbl);
    ext_free_list(&ext_list);
}

status read_line_stage_2(char *line, int line_num) {
    int instruction_index;
    char curr_word[MAX_LINE_LENGTH]; /* will hold current token as needed */

    /* Ignore line if it's blank or a comment */
    if (is_ignore_line(line))
        return NO_ERROR; /* skip to next line */

    line = skip_spaces(line); /* Proceeding to first non-blank character */
    if (is_end_of_line(line))
        return NO_ERROR; /* a blank line is not an error */

    copy_word(curr_word, line);
    if (is_label(curr_word, TRUE)) { /* If it's a label, skip it */
        line = next_word(line);
        copy_word(curr_word, line);
    }

    if ((instruction_index = find_directive(curr_word)) != NOT_FOUND) /* We need to handle only .entry directive */
    {
        line = next_word(line);
        if (instruction_index == ENTRY) {
            copy_word(curr_word, line);
            set_label_to_entry(symbols_tbl, curr_word); /* Creating an entry for the symbol */
        }
    }

    else if ((instruction_index = find_command(curr_word)) != NOT_FOUND) /* Encoding command's additional words */
    {
        line = next_word(line);
        command_handler_stage_2(instruction_index, line);
    }

    /* if error occured in commannd handler or in directive handler */
    if (print_error(line_num))
        return ERROR;

    return NO_ERROR;
}

/* This function writes all 3 output files (if they should be created)*/
int generate_output_files(char *original) {
    FILE *file;

    file = create_file(original, FILE_OBJECT);
    write_output_ob(file);

    if (entry_exists) {
        file = create_file(original, FILE_ENTRY);
        write_output_entry(file);
    }

    if (extern_exists) {
        file = create_file(original, FILE_EXTERN);
        write_output_extern(file);
    }

    return NO_ERROR;
}

/* This function writes the .ob file output.
 * The first line is the size of each memory (instructions and data).
 * Rest of the lines are: address in the first column, word in memory in the second.
 */
void write_output_ob(FILE *fp) {
    unsigned int address = IC_INIT_ADDR;
    int i;
    char *slice1;
    char *slice2;

    slice1 = convert_to_base_32(ic);
    slice2 = convert_to_base_32(dc);

    fprintf(fp, "%s\t%s\n\n", slice1, slice2); /* First line */
    free(slice1);
    free(slice2);

    for (i = 0; i < ic; address++, i++) /* Instructions memory */
    {
        slice1 = convert_to_base_32(address);
        slice2 = convert_to_base_32(instr_memory[i]);

        fprintf(fp, "%s\t%s\n", slice1, slice2);

        free(slice1);
        free(slice2);
    }

    for (i = 0; i < dc; address++, i++) /* Data memory */
    {
        slice1 = convert_to_base_32(address);
        slice2 = convert_to_base_32(data_memory[i]);

        fprintf(fp, "%s\t%s\n", slice1, slice2);

        free(slice1);
        free(slice2);
    }

    fclose(fp);
}

/* This function writes the output of the .ent file.
 * First column: name of label.
 * Second column: address of definition.
 */
void write_output_entry(FILE *fp) {
    char *base32_address;

    label_ptr label = symbols_tbl;
    /* Go through symbols table and print only symbols that have an entry */
    while (label) {
        if (label->entry) {
            base32_address = convert_to_base_32(label->address);
            fprintf(fp, "%s\t%s\n", label->name, base32_address);
            free(base32_address);
        }
        label = label->next;
    }
    fclose(fp);
}

/* This function writes the output of the .ext file.
 * First column: label name.
 * Second column: address where the external label should be replaced.
 */
void write_output_extern(FILE *fp) { /*FIXME: BUG in this function */
    char *base32_address;
    ext_ptr node;
    node = ext_list;
    /* Going through external circular linked list and pulling out values */
    do {
        base32_address = convert_to_base_32(node->address);
        fprintf(fp, "%s\t%s\n", node->name, base32_address); /* Printing to file */
        free(base32_address);
        node = node->next;
    } while (node != ext_list);
    fclose(fp);
}

/* This function determines if source and destination operands exist by opcode */
void check_operands_exist(int type, bool *is_src, bool *is_dest) {
    switch (type) {
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        *is_src = TRUE;
        *is_dest = TRUE;
        break;

    case NOT:
    case CLR:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case RED:
    case PRN:
    case JSR:
        *is_src = FALSE;
        *is_dest = TRUE;
        break;

    case RTS:
    case HLT:
        *is_src = FALSE;
        *is_dest = FALSE;
    }
}

/* This function handles commands for the second pass - encoding additional words */
int command_handler_stage_2(int type, char *line) {
    char first_op[MAX_LINE_LENGTH];
    char second_op[MAX_LINE_LENGTH];                           /* will hold first and second operands */
    char *src = first_op, *dest = second_op;                   /* after the check below, src will point to source and
                                                                *                                              dest to destination operands */
    bool is_src = FALSE, is_dest = FALSE;                      /* Source/destination operands existence */
    int src_method = ADDR_UNKNOWN, dest_method = ADDR_UNKNOWN; /* Their addressing methods */

    check_operands_exist(type, &is_src, &is_dest);

    /* Extracting source and destination addressing methods */
    if (is_src)
        src_method = extract_bits(instr_memory[ic], 4, 5);
    if (is_dest)
        dest_method = extract_bits(instr_memory[ic], 2, 3);

    /* Matching src and dest pointers to the correct operands (first or second or both) */
    if (is_src || is_dest) {
        line = next_list_token(first_op, line);
        /* There are 2 operands */
        if (is_src && is_dest) {
            line = next_list_token(second_op, line);
            next_list_token(second_op, line);
        } else {
            dest = first_op; /* If there's only one operand, it's a destination operand */
            src = NULL;
        }
    }

    ic++; /* The first word of the command was already encoded in this IC in the first pass */
    return encode_additional_words(src, dest, is_src, is_dest, src_method, dest_method);
}

/* This function encodes the additional words of the operands to instructions memory */
int encode_additional_words(char *src, char *dest, bool is_src, bool is_dest, int src_method, int dest_method) {
    /* There's a special case where 2 register operands share the same additional word */
    if (is_src && is_dest && src_method == ADDR_REGISTER && dest_method == ADDR_REGISTER) {
        write_to_instructions_memory(build_register_word(FALSE, src) | build_register_word(TRUE, dest));
    } else {
        if (is_src)
            encode_additional_word(FALSE, src_method, src);
        if (is_dest)
            encode_additional_word(TRUE, dest_method, dest);
    }
    return is_error_exists();
}

/* This function builds the additional word for a register operand */
unsigned int build_register_word(bool is_dest, char *reg) {
    unsigned int word = (unsigned int)atoi(reg + 1); /* Getting the register's number */
    /* Inserting it to the required bits (by source or destination operand) */
    if (!is_dest)
        word <<= BITS_IN_REGISTER;
    word = inject_ARE(word, ABSOLUTE);
    return word;
}

/* This function encodes a given label (by name) to memory */
void encode_label(char *label) {
    unsigned int word; /* The word to be encoded */

    if (is_existing_label(symbols_tbl, label)) {   /* If label exists */
        word = get_label_addr(symbols_tbl, label); /* Getting label's address */

        if (is_label_external(symbols_tbl, label)) { /* If the label is an external one */
            /* Adding external label to external list (value should be replaced in this address) */
            ext_insert_item(&ext_list, label, ic + IC_INIT_ADDR);
            word = inject_ARE(word, EXTERNAL);
        } else
            word = inject_ARE(word, RELOCATABLE); /* If it's not an external label, then it's relocatable */

        write_to_instructions_memory(word); /* Encode word to memory */
    } else {
        ic++;
        set_error("COMMAND_LABEL_DOES_NOT_EXIST");
    }
}

/* This function encodes an additional word to instructions memory, given the addressing method */
void encode_additional_word(bool is_dest, int method, char *operand) {
    unsigned int word = 0; /* An empty word */
    char *temp;

    switch (method) {
    case ADDR_IMMEDIATE: /* Extracting immediate number */
        word = (unsigned int)atoi(operand + 1);
        word = inject_ARE(word, ABSOLUTE);
        write_to_instructions_memory(word);
        break;

    case ADDR_DIRECT:
        encode_label(operand);
        break;

    case ADDR_STRUCT: /* Before the dot there should be a label, and after it a number */
        temp = strchr(operand, '.');
        *temp = '\0';

        encode_label(operand); /* Label before dot is the first additional word */
        *temp++ = '.';
        word = (unsigned int)atoi(temp);
        word = inject_ARE(word, ABSOLUTE);
        write_to_instructions_memory(word); /* The number after the dot is the second */
        break;

    case ADDR_REGISTER:
        word = build_register_word(is_dest, operand);
        write_to_instructions_memory(word);
    }
}
