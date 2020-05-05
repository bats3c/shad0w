#include <windows.h>

void main()
{
    LPCSTR Pwd;

    GetCurrentDirectory(MAX_PATH, Pwd);

    printf("%s\n", Pwd);
}