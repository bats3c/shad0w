#include <windows.h>

#include "settings.h"

void main()
{
    DWORD ret;

    ret = DeleteFileA(szFileName);

    if (ret == 0)
    {
        switch (GetLastError())
        {
        case 5:
            printf("ERROR: Failed to delete '%s' because access is denied.\n", szFileName);
            return;
        
        case 3:
            printf("ERROR: Failed to delete '%s' because it could not be found.\n", szFileName);
            return;
        
        default:
            printf("ERROR: Failed to delete '%s' with error code: %d.\n", szFileName);
        }
    }

    printf("Deleted '%s'\n", szFileName);
    return;
}