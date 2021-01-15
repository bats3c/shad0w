#include <stdio.h>
#include <windows.h>

char* listdirs(char* Dir)
{
    int buffSize = 2000;
    DWORD dwError = 0;
    FILETIME filetime;
    CHAR szDir[MAX_PATH];
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    SYSTEMTIME createtime, accesstime;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    char* tmpBuffer = (char*)malloc(2000);
    char* oBuffer = (char*)malloc(buffSize);

    sprintf(szDir, "%s\\*", Dir);

    // start getting the files
    hFind = FindFirstFile(szDir, &ffd);    

    // if we have a bad handle try to work out what went wrong
    if (INVALID_HANDLE_VALUE == hFind) 
    {
        switch (GetLastError())
        {
        case 3:
            sprintf(oBuffer, "ERROR: The path '%s' was unable to be found.\n", szDir);
            return oBuffer;
        
        case 5:
            sprintf(oBuffer, "ERROR: Access to the path '%s' is denied.\n", szDir);
            return oBuffer;
        
        case 123:
            sprintf(oBuffer, "ERROR: The filename, directory name, or volume label syntax of '%s' is incorrect.\n", szDir);
            return oBuffer;
        
        case 267:
            sprintf(oBuffer, "ERROR: The directory '%s' is invalid.\n", szDir);
            return oBuffer;
        
        default:
            break;
        }
        
        sprintf(oBuffer, "ERROR: listing '%s' code: %d.\n", szDir, GetLastError());
        return oBuffer;
    }

    // make a nicely formatted display of it all
    sprintf(oBuffer, "Directory: %s\n\n", szDir);   
    sprintf(oBuffer, "%sType\tCreated\t\t  Last Access\t     Length\t Name\n", oBuffer);
    sprintf(oBuffer, "%s----\t-------\t\t  -----------\t     ------\t ----\n", oBuffer);

    do
    {
        // get the creation & last access time in something we can use
        FileTimeToSystemTime(&ffd.ftCreationTime, &createtime);
        FileTimeToSystemTime(&ffd.ftLastAccessTime, &accesstime);

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // its a directory so display info as a directory
            sprintf(tmpBuffer, "DIR\t%.2d/%.2d/%.4d %.2d:%.2d  %.2d/%.2d/%.4d %.2d:%.2d      \t %s\n", createtime.wDay, createtime.wMonth, createtime.wYear,
                                                                                                    createtime.wHour, createtime.wMinute, accesstime.wDay,
                                                                                                    accesstime.wMonth, accesstime.wYear, accesstime.wHour,
                                                                                                    accesstime.wMinute, ffd.cFileName);
        }
        else
        {
            // use these so that filesize.QuadPart has the correct size
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;

            // this is just purely about formatting, making sure it looks nice
            if (filesize.QuadPart > 99)
            {
                sprintf(tmpBuffer, "FILE\t%.2d/%.2d/%.4d %.2d:%.2d  %.2d/%.2d/%.4d %.2d:%.2d   %d\t %s\n", createtime.wDay, createtime.wMonth, createtime.wYear,
                                                                                                     createtime.wHour, createtime.wMinute, accesstime.wDay,
                                                                                                     accesstime.wMonth, accesstime.wYear, accesstime.wHour,
                                                                                                     accesstime.wMinute, filesize.QuadPart, ffd.cFileName);
            } else {
                sprintf(tmpBuffer, "FILE\t%.2d/%.2d/%.4d %.2d:%.2d  %.2d/%.2d/%.4d %.2d:%.2d   %d    \t %s\n", createtime.wDay, createtime.wMonth, createtime.wYear,
                                                                                                         createtime.wHour, createtime.wMinute, accesstime.wDay,
                                                                                                         accesstime.wMonth, accesstime.wYear, accesstime.wHour,
                                                                                                         accesstime.wMinute, filesize.QuadPart, ffd.cFileName);
            }
        }

        if ((strlen(tmpBuffer) + strlen(oBuffer)) > buffSize)
        {

            buffSize = buffSize + strlen(tmpBuffer) + 1000;

            oBuffer = (char*)realloc(oBuffer, buffSize);

            if (oBuffer == NULL)
            {
                return oBuffer;
            }
            
        }

        sprintf(oBuffer, "%s%s", oBuffer, tmpBuffer);
        memset(tmpBuffer, '\0', strlen(tmpBuffer));
    }
    while (FindNextFile(hFind, &ffd) != 0);
    
    // we hit an error so tell them
    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) 
    {
        sprintf(oBuffer, "%sERROR: listing '%s' code: %d.\n", oBuffer, szDir, dwError);
    }

    FindClose(hFind);
    return oBuffer;
}