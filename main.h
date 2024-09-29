#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LABEL 32
#define MAX_LINE 80
#define LINE 1024

#define MALLOC_CORECT(X)/*Setting up a macro to check memory allocation*/\
    if(!X){\
        fprintf(stderr,"No space in memory\n");\
        exit(1);\
    }
#define REALLOC_CORECT(X, y)/*Setting up a macro to check memory allocation*/\
    if(!X){\
        fprintf(stderr,"No space in memory\n");\
        free(y);\
        exit(1);\
    }

#endif