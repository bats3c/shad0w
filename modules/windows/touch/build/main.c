#include <stdio.h>
#include <windows.h>

#include "settings.h"

int main()
{


    HANDLE cHandle = CreateFile(szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (cHandle == INVALID_HANDLE_VALUE)
    {

        switch (GetLastError())
        {

        case 5:
            printf("ERROR: Failed to create '%s' because access is denied.\n", szFileName);
            return;

        case 19:
            printf("ERROR: Failed to create '%s' because the media is write protected.\n", szFileName);
            return;
        
        default:
            break;
        }

        printf("ERROR: Failed to create '%s' with error code: %d\n", szFileName, GetLastError());
        return;
    }

    printf("Created file '%s'\n", szFileName);
    
}