#include <windows.h>
#include <stdio.h>

#include "settings.h"

void main()
{
    char *fcontent = NULL;
    int fsize = 0;
    FILE *fp;

    fp = fopen(szFile, "rb");
    if(fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        fcontent = (char*) malloc(sizeof(char) * fsize);
        fread(fcontent, 1, fsize, fp);

        for (size_t i = 0; i < fsize; i++)
        {
            printf("%.1c", fcontent[i]);
        }
        printf("\n");

        fclose(fp);
    }
}