#include <windows.h>

#include "settings.h"

void main()
{
    DWORD ret;

    ret = CreateDirectoryA(szDirName, NULL);

    if (ret == 0)
    {
        switch (GetLastError())
        {
        case ERROR_ALREADY_EXISTS:
            printf("ERROR: Failed to create '%s' because it already exists.\n", szDirName);
            return;
        
        case ERROR_PATH_NOT_FOUND:
            printf("ERROR: Failed to create '%s' because one or more intermediate directories do not exist.", szDirName);
            return;
        
        default:
            printf("ERROR: Failed to create '%s' with error code: %d.\n", szDirName);
        }
    }

    printf("Created directory '%s'\n", szDirName);
    return;
}