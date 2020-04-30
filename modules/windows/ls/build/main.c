#include <windows.h>

#include "settings.h"

int main()
{
    DWORD dwError = 0;
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    FILETIME filetime;
    SYSTEMTIME createtime, accesstime;
    
    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) 
    {
        printf("still ERROR\n");
        return dwError;
    } 
   
    // List all the files in the directory with some info about them.
    printf("Type\tCreated\t\t  Last Access\t     Length\t Name\n");
    printf("----\t-------\t\t  -----------\t     ------\t ----\n");

    do
    {

        FileTimeToSystemTime(&ffd.ftCreationTime, &createtime);
        FileTimeToSystemTime(&ffd.ftLastAccessTime, &accesstime);

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf(TEXT("DIR\t%.2d/%.2d/%.4d %.2d:%.2d  %.2d/%.2d/%.4d %.2d:%.2d      \t %s\n"), createtime.wDay, createtime.wMonth, createtime.wYear,
                                                                                                  createtime.wHour, createtime.wMinute, accesstime.wDay,
                                                                                                  accesstime.wMonth, accesstime.wYear, accesstime.wHour,
                                                                                                  accesstime.wMinute, ffd.cFileName);
        }
        else
        {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
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
    
    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) 
    {
        printf("ERROR: %d\n", dwError);
    }

    FindClose(hFind);
    return dwError;
}