#ifndef _UTILS_H
#define _UTILS_H

#include "global.h"

/* Declarations */
#define ERR_OUTPUT_FILE stderr

/* Prototypes */
char *strallocat(char *s0, char *s1);
void *malloc_with_check(long size);

#endif
