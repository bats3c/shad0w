#include <windows.h>

#include "settings.h"

void main()
{
    BOOL Ret;
    DWORD dwWrite;
    HANDLE wHandle;

    wHandle = CreateFile(szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (wHandle == INVALID_HANDLE_VALUE)
    {

        switch (GetLastError())
        {

        case 5:
            printf("ERROR: Failed to write to '%s' because access is denied.\n", szFileName);
            return;

        case 19:
            printf("ERROR: Failed to write to '%s' because the media is write protected.\n", szFileName);
            return;
        
        default:
            break;
        }

        printf("ERROR: Failed to write to '%s' with error code: %d\n", szFileName, GetLastError());
        return;
    }

    Ret = WriteFile(wHandle, wData, strlen(wData), &dwWrite, NULL);
    
    if (Ret != TRUE)
    {

        switch (GetLastError())
        {
        case 5:
            printf("ERROR: Failed to write to '%s' because access is denied.\n", szFileName);
            return;
        
        case 19:
            printf("ERROR: Failed to write to '%s' because the media is write protected.\n", szFileName);
            return;
        
        default:
            break;
        }

        printf("ERROR: Failed to write to '%s' with error code: %d\n", szFileName, GetLastError());
        return;
    }
    
    printf("Wrote %d bytes to %s\n", dwWrite, szFileName);

    return;
}