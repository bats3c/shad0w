#include <windows.h>

char* changedir(char* Dir);
char* getdir();
char* getpid();
char* getps();
char* listdirs(char* Dir);
char* makedirectory(char* szDirName);
char* readfile(char* szFile);
char* removefile(char* szFileName);
char* whoami(BOOL DisplayAll, char* oBuffer);