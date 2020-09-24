#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include <winhttp.h>

#include "settings.h"

static char encoding_table[] = {
                                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'
                                };

static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
 
    *output_length = 4 * ((input_length + 2) / 3);
 
    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;
 
    for (int i = 0, j = 0; i < input_length;) {
 
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;
 
        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
 
        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }
 
    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';
 
    return encoded_data;
}

void main ()
{
    LPCWSTR   ReqBuffer;
    HINTERNET hSession, hConnect, hRequest = NULL;
    char* file_b64;
    char* fcontent = NULL;
    int   fsize = 0;
    FILE* fp;

    fp = fopen(FILENAME, "rb");
    if(!fp)
    {
        printf("ERROR: Failed to open file\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);

    fcontent = (char*) malloc(sizeof(char) * fsize);
    fread(fcontent, fsize, 1, fp);
    fclose(fp);

    file_b64 = (char*)malloc(fsize * 2);
    file_b64 = base64_encode((const char*)fcontent, fsize, &fsize);

    printf("%s", file_b64);
}