#include <windows.h>

char* removefile(char* szFileName)
{
    DWORD ret;
    char* text = (char*)malloc(2000);

    ret = DeleteFileA(szFileName);

    if (ret == 0)
    {
        switch (GetLastError())
        {
        case 5:
            sprintf(text, "ERROR: Failed to delete '%s' because access is denied.\n", szFileName);
            return text;
        
        case 3:
            sprintf(text, "ERROR: Failed to delete '%s' because it could not be found.\n", szFileName);
            return text;
        
        default:
            sprintf(text, "ERROR: Failed to delete '%s' with error code: %d.\n", szFileName);
            return text;
        }
    }

    sprintf(text, "Deleted '%s'\n", szFileName);
    return text;
}