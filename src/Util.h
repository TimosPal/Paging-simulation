#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define BUFFER_SIZE 256
#define EXIT_ERROR_CODE 1
#define DEFAULT_HASH_SIZE 128

#define PAGE_SIZE 4096

#define PID1 0
#define PID2 1

#define TRACE_FILE1_PATH "./bzip.trace"
#define TRACE_FILE2_PATH "./gcc.trace"

#define IF_ERROR_MSG(x,msg) { if(x) {fprintf(stderr,"ERROR : %s.\n",msg); exit(EXIT_ERROR_CODE); } }

typedef enum Algorithm {
    LRU, Second_Chance
} Algorithm;

typedef struct Frame{
    unsigned int currentPageNumber;
    unsigned int currentPID;
} Frame;

typedef struct PageEntry {
    unsigned int frameNumber;
    bool dirty;
    bool recentlyRead;
} PageEntry;


#endif