#include "StringUtil.h"

#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

List StringSplit(char* str, char* token){
    List temp;
    List_Init(&temp);
    char* currWord;
    currWord = strtok(str, token);
    while(currWord != NULL){
        List_Append(&temp, NewString(currWord));
        currWord = strtok(NULL, token);
    }

    return temp;
}

bool StringToInt(char* str,int* value, int base){
    errno = 0;
    char* endPtr;

    *value = (int)strtol(str,&endPtr,base);
    return (errno == 0 && *endPtr == '\0');
}

char* NewString(char* str){
    char* newStr = malloc((strlen(str) + 1)*sizeof(char));
    strcpy(newStr,str);
    return newStr;
}

bool StringCmp(void* value1,void* value2){
    return strcmp((char*)value1,(char*)value2) == 0;
}