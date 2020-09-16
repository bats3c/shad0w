#include <windows.h>
#include <stdio.h>

char* decrypt_string(char* string, int key)
{
    size_t i;

    size_t out_len   = strlen(string) + 1;
    size_t b64_len   = b64_decoded_size(string);
    char*  b64_out;

    b64_out = base64_decode((const char*)string, out_len - 1, &out_len);

    for (i = 0; i < b64_len; i++)
    {
        b64_out[i] = b64_out[i] ^ key;
    }
    b64_out[i] = '\0';

    return b64_out;
}