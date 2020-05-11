#include <stdio.h>
#include <windows.h>

char* makedirectory(char* szDirName)
{
    DWORD ret;
    char* text = (char*)malloc(2000);

    ret = CreateDirectoryA(szDirName, NULL);

    if (ret == 0)
    {
        switch (GetLastError())
        {
        case ERROR_ALREADY_EXISTS:
            sprintf(text, "ERROR: Failed to create '%s' because it already exists.\n", szDirName);
            return text;
        
        case ERROR_PATH_NOT_FOUND:
            sprintf(text, "ERROR: Failed to create '%s' because one or more intermediate directories do not exist.\n", szDirName);
            return text;
        
        default:
            sprintf(text, "ERROR: Failed to create '%s' with error code: %d.\n", szDirName);
            return text;
        }
    }

    sprintf(text, "Created directory '%s'\n", szDirName);
    return text;
}