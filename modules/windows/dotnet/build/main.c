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
            printf("ERROR: The path '%s' was unable to be found.\n", szDir);
            return;

        case 5:
            printf("ERROR: Access to the path '%s' is denied.\n", szDir);
            return;

        case 123:
            printf("ERROR: The filename, directory name, or volume label syntax of '%s' is incorrect.\n", szDir);
            return;

        case 267:
            printf("ERROR: The directory '%s' is invalid.\n", szDir);
            return;

        default:
            break;
        }

        printf("ERROR: listing '%s' code: %d.\n", szDir, GetLastError());
        return;
    }

    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("%s\n", ffd.cFileName);
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
}

int main(int argc, char const *argv[])
{
    // TODO: Read from the registry

    CHAR* NetPath   = "C:\\Windows\\Microsoft.NET\\Framework\\";
    CHAR* NetPath64 = "C:\\Windows\\Microsoft.NET\\Framework64\\";

    printf("%s\n", NetPath);
    listdirs(NetPath);

    printf("%s\n", NetPath64);
    listdirs(NetPath64);
}
