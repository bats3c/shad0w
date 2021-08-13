#include <stdio.h>
#include <windows.h>

char* getdir(){

    char* text = (char*)malloc(MAX_PATH + 1);
    GetCurrentDirectoryA(MAX_PATH, text);
    return text;
}
