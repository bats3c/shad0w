typedef HINTERNET (WINAPI * WinHttpOpen_) (
    LPCWSTR pszAgentW,
    DWORD   dwAccessType,
    LPCWSTR pszProxyW,
    LPCWSTR pszProxyBypassW,
    DWORD   dwFlags
);

typedef HINTERNET (WINAPI * WinHttpConnect_) (
    IN HINTERNET     hSession,
    IN LPCWSTR       pswzServerName,
    IN INTERNET_PORT nServerPort,
    IN DWORD         dwReserved
);

typedef HINTERNET (WINAPI * WinHttpOpenRequest_) (
    IN HINTERNET hConnect,
    IN LPCWSTR   pwszVerb,
    IN LPCWSTR   pwszObjectName,
    IN LPCWSTR   pwszVersion,
    IN LPCWSTR   pwszReferrer,
    IN LPCWSTR   *ppwszAcceptTypes,
    IN DWORD     dwFlags
);

typedef BOOLAPI (WINAPI * WinHttpSetOption_) (
    HINTERNET hInternet,
    DWORD     dwOption,
    LPVOID    lpBuffer,
    DWORD     dwBufferLength
);

typedef BOOLAPI (WINAPI * WinHttpSendRequest_) (
    IN HINTERNET hRequest,
    LPCWSTR      lpszHeaders,
    IN DWORD     dwHeadersLength,
    LPVOID       lpOptional,
    IN DWORD     dwOptionalLength,
    IN DWORD     dwTotalLength,
    IN DWORD_PTR dwContext
);

typedef BOOLAPI (WINAPI * WinHttpReceiveResponse_) (
    IN HINTERNET hRequest,
    IN LPVOID    lpReserved
);

typedef BOOLAPI (WINAPI * WinHttpQueryDataAvailable_) (
    IN HINTERNET hRequest,
    LPDWORD      lpdwNumberOfBytesAvailable
);

typedef BOOLAPI (WINAPI * WinHttpReadData_) (
    IN HINTERNET hRequest,
    LPVOID       lpBuffer,
    IN DWORD     dwNumberOfBytesToRead,
    OUT LPDWORD  lpdwNumberOfBytesRead
);

typedef BOOLAPI (WINAPI * WinHttpCloseHandle_) (
    IN HINTERNET hInternet
);

typedef HANDLE (WINAPI * GetCurrentProcess_) (
);