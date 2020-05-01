#include <windows.h>

#include "settings.h"

int main()
{
    DWORD dwError = 0;
    FILETIME filetime;
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    SYSTEMTIME createtime, accesstime;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // start getting the files
    hFind = FindFirstFile(szDir, &ffd);

    // if we have a bad handle try to work out what went wrong
    if (INVALID_HANDLE_VALUE == hFind) 
    {
        switch (GetLastError())
        {
        case 5:
            printf("ERROR: Access to the path '%s' is denied.\n", szDir);
            return dwError;
        
        case 267:
            printf("ERROR: The directory '%s' is invalid.\n", szDir);
            return dwError;
        
        default:
            break;
        }
        
        printf("ERROR: listing '%s' code: %d.\n", szDir, GetLastError());
        return dwError;
    }

    // make a nicely formated display of it all
    printf("Directory: %s\n\n", szDir);   
    printf("Type\tCreated\t\t  Last Access\t     Length\t Name\n");
    printf("----\t-------\t\t  -----------\t     ------\t ----\n");

    do
    {
        
        // get the creation & last access time in something we can use
        FileTimeToSystemTime(&ffd.ftCreationTime, &createtime);
        FileTimeToSystemTime(&ffd.ftLastAccessTime, &accesstime);

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // its a directory so display info as a directory
            printf(TEXT("DIR\t%.2d/%.2d/%.4d %.2d:%.2d  %.2d/%.2d/%.4d %.2d:%.2d      \t %s\n"), createtime.wDay, createtime.wMonth, createtime.wYear,
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
            if (filesize.QuadPart > 9)
            {
                printf(TEXT("FILE\t%.2d/%.2d/%.4d %.2d:%.2d  %.2d/%.2d/%.4d %.2d:%.2d   %d\t %s\n"), createtime.wDay, createtime.wMonth, createtime.wYear,
                                                                                                     createtime.wHour, createtime.wMinute, accesstime.wDay,
                                                                                                     accesstime.wMonth, accesstime.wYear, accesstime.wHour,
                                                                                                     accesstime.wMinute, filesize.QuadPart, ffd.cFileName);
            } else {
                printf(TEXT("FILE\t%.2d/%.2d/%.4d %.2d:%.2d  %.2d/%.2d/%.4d %.2d:%.2d   %d    \t %s\n"), createtime.wDay, createtime.wMonth, createtime.wYear,
                                                                                                         createtime.wHour, createtime.wMinute, accesstime.wDay,
                                                                                                         accesstime.wMonth, accesstime.wYear, accesstime.wHour,
                                                                                                         accesstime.wMinute, filesize.QuadPart, ffd.cFileName);
            }
        }
    }
    while (FindNextFile(hFind, &ffd) != 0);
    
    // we hit an error so tell them
    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) 
    {
        printf("ERROR: listing '%s' code: %d.\n", szDir, dwError);
    }

    FindClose(hFind);
    return dwError;
}