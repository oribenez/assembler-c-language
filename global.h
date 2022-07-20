#ifndef _GLOBAL_H
#define _GLOBAL_H

/* Declarations */

#define IC_INIT_ADDR 100
#define CODE_ARR_IMG_LENGTH 2560
#define MAX_LINE_LENGTH 82 /* Maximum length of a single source line  */
#define REGISTER_LENGTH 2 /* a register's name contains 2 characters */
#define MIN_REGISTER 0    /* r0 is the first register */
#define MAX_REGISTER 7    /* r7 is the last register */
#define NOT_FOUND -1

#define NO_ERROR "NO_ERROR"
#define UNDEFINED "UNDEFINED"
#define LABEL_MAX_LEN 30

#define NUM_DIRECTIVES 5
#define NUM_COMMANDS 16
#define BASE_NUMBER 32

/** Boolean */
typedef enum {
    FALSE,
    TRUE
} bool;

/* Directives types */
enum directives {DATA, STRING, STRUCT, ENTRY, EXTERN, UNKNOWN_TYPE}; 

/* Enum of commands ordered by their opcode */
enum commands {MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP, UNKNOWN_COMMAND};

/** Operand addressing type */
typedef enum addressing_types {
    /** Immediate addressing (0) */
    IMMEDIATE_ADDR = 0,
    /** Direct addressing (1) */
    DIRECT_ADDR = 1,
    /** Relative addressing (2) */
    RELATIVE_ADDR = 2,
    /** Register addressing */
    REGISTER_ADDR = 3,
    /** Failed/Not detected addressing */
    NONE_ADDR = -1
} addressing_type;

/** Represents a single code word */
typedef struct code_word {
    unsigned int ARE : 2;
    unsigned int dest_operand : 2;
    unsigned int src_operand : 2;
    unsigned int opcode : 4;
} code_word;

/** Represents a single data word. */
typedef struct data_word {
    unsigned int ARE : 2;
    /* The data content itself (a method for putting data into these field is defined) */
    unsigned int data : 8;
} data_word;

/** Represents a general machine code word contents */
typedef struct machine_word {
    /* if it represents code (not additional data), this field contains the total length required by the code. if it's data, this field is 0. */
    short length;
    /* The content can be code or data */
    union word {
        data_word *data;
        code_word *code;
    } word;
} machine_word;

/* linked list of labels */
typedef struct strLabels * label_ptr;
typedef struct strLabels {
	char name[LABEL_MAX_LEN]; /* label name */
	unsigned int address; /* label address */
	bool external; /* a boolean type variable to store if the label is extern or not */
	bool inActionStatement; /* a boolean type varialbe to store if the label is in an action statement or not */
	bool entry; /* a boolean type varialbe to store if the label is entry or not */
	label_ptr next; /* a pointer to the next label in the list */
} Labels;

typedef struct {
    char* key;
    char* message;
} err;

extern const char base32[];
extern const char *commands[];
extern const char *directives[];
extern const err errors[];
extern label_ptr symbols_tbl;
char* curr_error_key;


/* Prototypes */
bool is_error_exists();
bool print_error(int line_num);
void set_error(char* err_key);
void throw_err(char* err_key, int line_num);

#endif
