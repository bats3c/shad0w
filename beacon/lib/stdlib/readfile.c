#include <stdio.h>
#include <windows.h>

char* readfile(char* szFile)
{
    FILE *fp;
    HANDLE hFile;
    int fsize = 0;
    char* oBuffer;
    char* fcontent = NULL;
    
    // open so we can get error codes
    hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL); 

    // parse the error codes
    if (hFile == INVALID_HANDLE_VALUE)
    {
        
        char* errbuf = (char*)malloc(MAX_PATH + 500);

        switch (GetLastError())
        {
        case 2:
            sprintf(errbuf, "ERROR: The file '%s' was unable to be found.\n", szFile);
            return errbuf;
        
        case 5:
            sprintf(errbuf, "ERROR: Access to the file '%s' is denied.\n", szFile);
            return errbuf;
        
        case 123:
            sprintf(errbuf, "ERROR: The filename, directory name, or volume label syntax of '%s' is incorrect.\n", szFile);
            return errbuf;
        
        case 267:
            sprintf(errbuf, "ERROR: The file '%s' is invalid.\n", szFile);
            return errbuf;
        
        default:
            break;
        }

        sprintf(errbuf, "ERROR: listing '%s' code: %d.\n", szFile, GetLastError());
        return errbuf;
    }

    // we don't need this anymore
    CloseHandle(hFile);
    
    // open it again so we can use it
    fp = fopen(szFile, "rb");
    
    // read it an return it
    if (fp) 
    {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        fcontent = (char*)malloc(fsize);
        fread(fcontent, 1, fsize, fp);

        oBuffer = (char*)malloc(fsize);

        memset(oBuffer, '\0', fsize);
        for (size_t i = 0; i < fsize; i++)
        {
            sprintf(oBuffer, "%s%c", oBuffer, fcontent[i]);
        }
        sprintf(oBuffer, "%s%c", oBuffer, '\n');
        fclose(fp);
    }

    return oBuffer;
}