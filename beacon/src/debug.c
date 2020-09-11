#include <stdarg.h>

void DEBUG(const char* text, ...)
{
    va_list ap;
    char* string;

    va_start(ap, text);
    if(0 > vasprintf(&string, text, ap))
    {
        string = "FAILED TO FORMAT";
    }
    va_end(ap);

    #ifdef DEBUG_MODE
        printf("[DEBUG] %s\n", string);
    #endif

    free(string);
}