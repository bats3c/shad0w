#include <stdio.h>
#include <windows.h>

char* changedir(char* Dir)
{
    DWORD ret;
    ret = SetCurrentDirectory(Dir);

    char* oBuffer = (char*)malloc(2000);

    if (ret == 0)
    {
        switch (GetLastError())
        {
        case 3:
            sprintf(oBuffer, "ERROR: The path '%s' was unable to be found.\n", Dir);
            return oBuffer;
        
        case 5:
            sprintf(oBuffer, "ERROR: Access to '%s' is denied.\n", Dir);
            return oBuffer;
        
        case 123:
            sprintf(oBuffer, "ERROR: The filename, directory name, or volume label syntax of '%s' is incorrect.\n", Dir);
            return oBuffer;
        
        case 267:
            sprintf(oBuffer, "ERROR: The directory '%s' is invalid.\n", Dir);
            return oBuffer;
        
        default:
            break;
        }
        
        sprintf(oBuffer, "ERROR: changing directory to '%s' code: %d.\n", Dir, GetLastError());
        return oBuffer;
    }

    sprintf(oBuffer, "Directory Changed to: '%s'\n", Dir);

    return oBuffer;
}