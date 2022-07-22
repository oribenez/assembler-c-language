#ifndef _GLOBAL_H
#define _GLOBAL_H

/* Declarations */
#define IC_INIT_ADDR 100
#define IMAGE_MEM_SIZE 2560
#define CODE_ARR_IMG_LENGTH 2560
#define MAX_LINE_LENGTH 82 /* Maximum length of a single source line  */
#define REGISTER_LENGTH 2  /* a register's name contains 2 characters */
#define MIN_REGISTER 0     /* r0 is the first register */
#define MAX_REGISTER 7     /* r7 is the last register */
#define NOT_FOUND -1

#define LABEL_MAX_LEN 30
#define OPERAND_MAX_LEN 20

#define NUM_DIRECTIVES 5
#define NUM_COMMANDS 16
#define BASE_NUMBER 32

#define BITS_IN_WORD 10
#define BITS_IN_OPCODE 4
#define BITS_IN_METHOD 2
#define BITS_IN_ARE 2
#define BITS_IN_REGISTER 4
#define BITS_IN_ADDRESS 8

/** Boolean */
typedef enum {
    FALSE,
    TRUE
} bool;

/** ERROR */
typedef enum {
    ERROR = 0,
    NO_ERROR = 1,

    FAILED = 0,
    SUCCESS = 1,

    INVALID = 0,
    VALID = 1
} status;

/* Directives types */
enum directives { DATA,
                  STRING,
                  STRUCT,
                  ENTRY,
                  EXTERN,
                  UNKNOWN_TYPE };

/* Enum of commands ordered by their opcode */
enum commands { MOV,
                CMP,
                ADD,
                SUB,
                NOT,
                CLR,
                LEA,
                INC,
                DEC,
                JMP,
                BNE,
                RED,
                PRN,
                JSR,
                RTS,
                HLT,
                UNKNOWN_COMMAND };

/** Operand addressing type */
typedef enum addressing_types {
    ADDR_IMMEDIATE,
    ADDR_DIRECT,
    ADDR_STRUCT,
    ADDR_REGISTER
} addressing_type;

enum ARE {ABSOLUTE, EXTERNAL, RELOCATABLE};


/* linked list of labels */
typedef struct strLabels *label_ptr;
typedef struct strLabels {
    char name[LABEL_MAX_LEN]; /* label name */
    unsigned int address;     /* label address */
    bool external;            /* a boolean type variable to store if the label is extern or not */
    bool inActionStatement;   /* a boolean type varialbe to store if the label is in an action statement or not */
    bool entry;               /* a boolean type varialbe to store if the label is entry or not */
    label_ptr next;           /* a pointer to the next label in the list */
} Labels;

typedef struct {
    char *key;
    char *message;
} err;

extern const char base32[];
extern const char *commands[];
extern const char *directives[];
extern const err errors[];
extern label_ptr symbols_tbl;
extern bool entry_exists, extern_exists;
extern char *curr_error_key;
extern unsigned int data_memory[];
extern unsigned int instr_memory[];
extern int ic;
extern int dc;

/* Prototypes */
bool is_error_exists();
bool print_error(int line_num);
void set_error(char *err_key);
void throw_err(char *err_key, int line_num);

#endif
