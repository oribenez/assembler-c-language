#include "text_engine.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief strting from a given pointer skipping spaces until start of next word
 *
 * @param ch pointer to string
 * @return char* pointer to the next non blank char
 */
char *skip_spaces(char *ch) {
    if (ch == NULL)
        return NULL;

    while (isspace(*ch))
        ch++;
    return ch;
}

/**
 * @brief checks if a given line is blank or a comment and should be ignored by the interpreter
 *
 * @param line pointer to starting of line string
 * @return true if this line is blank or a comment and should be ignored by the interpreter
 */
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
 * @brief function which checks if a given string is a register for example 'r4'
 *
 * @param word pointer to the register string
 * @return true if the given string is a register between 0 to 7 and it is in a register format.
 */
bool is_register(char *word) {
    return strlen(word) == REGISTER_LENGTH && word[0] == 'r' && word[1] >= '0' && word[1] <= '7';
}

/**
 * @brief function which is copying from given line the first next word to a given word pointer.
 * word pointer should be allocated outside of this function
 *
 * @param word destination to copy
 * @param line the line to copy from
 */
void copy_word(char *word, char *line) {
    int i = 0;
    if (word == NULL || line == NULL)
        return;

    while (i < MAX_LINE_LENGTH && !isspace(line[i]) && line[i] != '\0') {
        word[i] = line[i];
        i++;
    }
    word[i] = '\0';
}

/**
 * @brief function which frorm a given string pointer skips to the next word in which between there is a whitespaces.
 * in short: skips to the first letter of the next word after reading at least one whitespace.
 *
 * @param line the line to read
 * @return pointer to the starting letter of the next word
 */
char *next_word(char *str) {
    if (str == NULL)
        return NULL;

    /* skip characters until whitespace */
    while (!isspace(*str) && !is_end_of_line(str))
        str++;

    str = skip_spaces(str); /* Skip spaces until next word/ end of line */
    
    /* if the word didn't reached to the end of the line after skipping spaces, means there is no next word and return null */
    if (is_end_of_line(str)) return NULL;
    return str;
}

/**
 * @brief check if a given pointer to a line is in the end of it, which menas that there are no more word to read in this line.
 * 
 * @param line pointer to a character in a string
 * @return true if end of line
 */
bool is_end_of_line(char *line) {
    return line == NULL || *line == '\n' || *line == '\0';
}

/**
 * @brief function which checks if a given word is a label
 * @exception there is a need check after calling this funciton if there were errors during the function operation.
 * 
 * @param word the word to check if it is a label
 * @param is_w_colon indicator which tells if the word includes a colon at the end of it, true if coolon exists, otherwise false.
 * @return true if the given word is a label, otherwise false.
 */
bool is_label(char *word, bool is_w_colon) {
    int i;
    int word_len = strlen(word);

    if (word == NULL) {
        if (is_w_colon) {
            if (word_len < 2)
                return FALSE;
        } else {
            if (word_len < 1)
                return FALSE;
        }
    }

    if (is_w_colon && word[word_len - 1] != ':')
        return FALSE;

    if (word_len > LABEL_MAX_LEN) {
        if (is_w_colon)
            set_error("LABEL_MAX_LENGTH");
        return FALSE;
    }

    /* first char must be letter */
    if (!isalpha(word[0])) {
        if (is_w_colon)
            set_error("LABEL_FIRST_CHAR_IS_LETTER");
        return FALSE;
    }

    /* trim colon at end of the word */
    if (is_w_colon) {
        word[word_len - 1] = '\0';
        word_len--;
    }

    for (i = 1; i < word_len; i++) {
        if (!isalnum(word[i])) {
            if (is_w_colon)
                set_error("LABEL_ONLY_ALPHANUMERIC");
            break;
        }
    }

    if (is_reserved_word(word))
        return FALSE;

    return TRUE;
}

/**
 * @brief funciton which copy the next word to a given destination
 * Important note: the words are seperated with one comma  ','
 * 
 * @param dest destination pointer to copy the word to, make sure that memory is allocated before callling this function.
 * @param line the line to read and find in it the next word
 * @return pointer to the next word in this line
 */
char *copy_next_li_word(char *dest, char *line) {
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

/**
 * @brief function which checks if a given sequence is a valid string (wrapped with "")
 * 
 * @param str string to check if it is a valid string 
 * @return true if string, otherwise false.
 */
bool is_string(char *str) {
    if (str == NULL)
        return FALSE;

    if (*str == '"') /* starts with quote " */
        str++;
    else
        return FALSE;

    /* skip to end of string*/
    while (*str && *str != '"')
        str++;

    if (*str != '"') /* a string must end with " */
        return FALSE;

    str++;
    if (*str != '\0') /* string token must end after the ending " */
        return FALSE;

    return TRUE;
}

/**
 * @brief function which copy the next word within a given line, but the next word is a String which is wrapped with quotes ""
 * 
 * @param dest destination pointer to copy, make sure memory allocation is made before calling this funciton
 * @param line the line to read next word
 * @return pointer to the next word
 */
char *next_token_string(char *dest, char *line) {
    char temp[MAX_LINE_LENGTH];
    line = copy_next_li_word(dest, line);

    if (*dest != '"')
        return line;
        
    while (!is_end_of_line(line) && dest[strlen(dest) - 1] != '"') {
        line = copy_next_li_word(temp, line);
        if (line)
            strcat(dest, temp);
    }
    return line;
}

/**
 * @brief function which checks if a string is a number which is built all from digits
 * 
 * @param str the string to check
 * @return true if string is a number, otherwise false.
 */
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
