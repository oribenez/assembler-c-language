#include "text_engine.h"
#include <stdio.h>
#include <stdlib.h>

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
    int i;
    char ch;

    /* check if word is equal to a word from commands list */
    for (i = 0; i < NUM_COMMANDS; i++) {
        if (!strcmp(commands[i], word)) { /* equal = found a match */
            return TRUE;
        }
    }

    /* check if word is equal to a letter from directives list */
    for (i = 0; i < NUM_DIRECTIVES; i++) {
        if (!strcmp(directives[i], word)) { /* equal = found a match */
            return TRUE;
        }
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
    if (is_register(word))
        return TRUE;

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
    while (!isspace(*str) && !is_end_of_line(str))
        str++;              /* Skip rest of characters in the current token (until a space) */
    str = skip_spaces(str); /* Skip spaces */
    if (is_end_of_line(str))
        return NULL;
    return str;
}

bool is_end_of_line(char *line) {
    return line == NULL || *line == '\0' || *line == '\n';
}

/* returns true if the word is label */
bool is_label(char *word, bool is_w_colon) {
    int i;
    int word_len = strlen(word);
    int colon_bounce = is_w_colon ? 1 : 0;
    char* word_wo_colon;
    int word_length_wo_colon;

    /* validations */
    /* check lable max length */
    if (word_len - colon_bounce > LABEL_MAX_LEN)
        if(is_w_colon) set_error("LABEL_MAX_LENGTH"); /* minus 1 not inc. colon ':' */

    /* check word is including only characters of type letter */
    if (!isalpha(word[0])) {
        if(is_w_colon) set_error("LABEL_FIRST_CHAR_IS_LETTER");
    }
    for (i = 1; i < word_len - colon_bounce; i++) {
        if (!isalnum(word[i])) {
            if(is_w_colon) set_error("LABEL_ONLY_ALPHANUMERIC");
            break;
        }
    }
    
    /* check if label word name is reserved (register or command name) */
    word_length_wo_colon = is_w_colon ? strlen(word) : strlen(word)-1;
    word_wo_colon = (char*)malloc_with_check(word_length_wo_colon);
    strcpy(word_wo_colon, word);
    if(is_reserved_word(word_wo_colon)) return FALSE;


    free(word_wo_colon);

    if (is_w_colon && word[word_len - 1] == ':')
        return TRUE;
    else if (!is_w_colon && word[word_len - 1] != ':')
        return TRUE;
    else
        return FALSE;
}

char *next_list_token(char *dest, char *line) {
    char *temp = dest;
    /* If the given line is empty, copy an empty token */
    if (is_end_of_line(line)) {
        dest[0] = '\0';
        return NULL;
    }

    if (isspace(*line))
        line = skip_spaces(line);

    /* A comma deserves a separate, single-character token */
    if (*line == ',') {
        strcpy(dest, ",");
        return ++line;
    }

    /* Manually copying token until a ',', whitespace or end of line */
    while (!is_end_of_line(line) && *line != ',' && !isspace(*line)) {
        *temp = *line;
        temp++;
        line++;
    }
    *temp = '\0';

    return line;
}

/* This function checks if a given sequence is a valid string (wrapped with "") */
bool is_string(char *str) {
    if (str == NULL)
        return FALSE;

    if (*str == '"') /* starts with " */
        str++;
    else
        return FALSE;

    while (*str && *str != '"') { /* Goes until end of string */
        str++;
    }

    if (*str != '"') /* a string must end with " */
        return FALSE;

    str++;
    if (*str != '\0') /* string token must end after the ending " */
        return FALSE;

    return TRUE;
}

/* This function copies supposedly next string into dest array and returning a pointer to the
 * first character after it
 */
char *next_token_string(char *dest, char *line) {
    char temp[MAX_LINE_LENGTH];
    line = next_list_token(dest, line);

    if (*dest != '"')
        return line;
    while (!is_end_of_line(line) && dest[strlen(dest) - 1] != '"') {
        line = next_list_token(temp, line);
        if (line)
            strcat(dest, temp);
    }
    return line;
}

/* This function checks if a string is a number (all digits) */
bool is_number(char *str) {
    if (is_end_of_line(str))
        return FALSE;

    /* a number can contain a plus or minus sign */
    if (*str == '+' || *str == '-') {
        str++;
        if (!isdigit(*str++))
            return FALSE; /* but not only a sign */
    }

    /* Check that the rest of the token is made of digits */
    while (!is_end_of_line(str)) {
        if (!isdigit(*str++))
            return FALSE;
    }
    return TRUE;
}
