#include "Trace.h"

#include <stdlib.h>

#include "StringUtil.h"

void Trace_Init(Trace* trace, char* string){
    List tokens = StringSplit(string, " ");
    trace->address = NewString(tokens.head->value);
    trace->type = (strcmp(tokens.head->next->value, "R\n") == 0) ? Read : Write;

    List_FreeValues(tokens, free);
    List_Destroy(&tokens);
}

void Trace_Destroy(Trace* trace){
    free(trace->address);
}
