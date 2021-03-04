#ifndef CUSTOM_STRING_FUNCTIONS_H
#define CUSTOM_STRING_FUNCTIONS_H

#include <stdbool.h>
#include <string.h>

#include "LinkedList.h"
#include "Hash.h"

char* NewString(char* str);

bool StringToInt(char* str,int* value,int base);
bool StringCmp(void* value1,void* value2);

List StringSplit(char* str, char* token);

#endif
