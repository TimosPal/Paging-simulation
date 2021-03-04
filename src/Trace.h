#ifndef TRACE_H
#define TRACE_H

typedef struct Trace{
    char* address;

    enum {
        Read , Write
    } type;
} Trace;

void Trace_Init(Trace* trace, char* string);
void Trace_Destroy(Trace* trace);

#endif
