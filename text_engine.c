#include "text_engine.h"
#include "global.h"

/* skip spaces of a string to the next non-blank character */
char *skip_spaces(char *ch) {
    if (ch == NULL)
        return NULL;
    /* move pointer to the end of whitespaces */
    while (isspace(*ch)) {
        ch++;
    }
    return ch;
}

/* returns true if this line is comment or has only whitespaces */
bool is_ignore_line(char *line) {
    line = skip_spaces(line);
    return *line == ';' || *line == '\0' || *line == '\n';
}

/**
 * @param word the word to check
 * @return TRUE, if the give word is reserved. Otherwise, false.
 */
bool is_reserved_word(char *word) {
    char **ptr;
    int i;
    char ch;
    
    /* check if word is equal to a word from commands list */
    ptr = (char **)commands;
    while (strcmp(*ptr, "NULL")) {
        if (!strcmp(*ptr, word)) { /* equal = found a match */
            return TRUE;
        }
        ptr++;
    }

    /* check if word is equal to a letter from directives list */
    ptr = (char **)directives;
    while (strcmp(*ptr, "NULL")) {
        if (!strcmp(*ptr, word)) { /* equal = found a match */
            return TRUE;
        }
        ptr++;
    }

    /* check if word is equal to a letter from base32 list */
    if (strlen(word) == 1) {
        ch = *word;
        for (i = 0; i < BASE_NUMBER; i++) {
            if (ch == base32[i])
            return TRUE;
        }
    }

    /* check if word is equal to register name */
    if(is_register(word)) return TRUE;

    return FALSE;
}

/**
 * @brief gets a string that represent a register, and validates the format
 *
 * @param word the address of the string
 * @return boolean whether or not this is valida register
 */
bool is_register(char *word) {
    return strlen(word) == REGISTER_LENGTH && word[0] == 'r' && word[1] >= '0' && word[1] <= '7';
}

/* Function will copy the next word (until a space or end of line) to a string */
void copy_word(char *word, char *line) {
    int i = 0;
    if (word == NULL || line == NULL)
        return;

    while (i < MAX_LINE_LENGTH && !isspace(line[i]) && line[i] != '\0') /* Copying token until its end to *dest */
    {
        word[i] = line[i];
        i++;
    }
    word[i] = '\0';
}

/**
 * @brief skip to the next non spaces to
 *
 * @param line the line we are working on
 * @return char* the pointer to the char that starts next word
 */
char *next_word(char *str) {
    if (str == NULL)
        return NULL;
    while (!isspace(*str) && !end_of_line(str))
        str++;              /* Skip rest of characters in the current token (until a space) */
    str = skip_spaces(str); /* Skip spaces */
    if (end_of_line(str))
        return NULL;
    return str;
}

/**
 * @brief prints an error message to the user according to a given table of erros (erros.json)
 *
 * @param error_code which error to show message on
 * @param current_line which line had an error
 */
int end_of_line(char *line) {
    return line == NULL || *line == '\0' || *line == '\n';
}

/* returns true if the word is label */
bool is_label(char *word) {
    int i;
    int word_len = strlen(word);
    /* validations */
    /* check lable max length */
    if(word_len-1 <= LABEL_MAX_LEN) set_error("LABEL_MAX_LEN"); /* minus 1 not inc. colon ':' */

    /* check word is including only characters of type letter */
    for (i = 0; i < word_len; i++) {
        if(!isalpha(word[i])) {
            set_error("LABEL_ONLY_ALPHA");
        }
    }
    
    if (word[word_len - 1] == ':')
        return TRUE;
    else
        return FALSE;
}
