#include <stdio.h>
#include <windows.h>

char* getdir()
{
    TCHAR NPath[MAX_PATH];
    char* text = (char*)malloc(MAX_PATH + 1);
    GetCurrentDirectory(MAX_PATH, NPath);

    sprintf(text, "%s", NPath);

    return text;
}