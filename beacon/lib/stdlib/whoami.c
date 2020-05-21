// This works fine in every binary appart from the staged secure binary. I have no ideas why.
// So therefore this is no longer in use.

#include <windows.h>

#define SECURITY_WIN32
#include <security.h>

VOID* GetTokenInfo(TOKEN_INFORMATION_CLASS TokenType)
{
    HANDLE hToken = 0;
    DWORD dwLength = 0;
    VOID* pTokenInfo = 0;

    // get the tokens
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &hToken))
    {
        // get length of the token info
        GetTokenInformation(hToken, TokenType, NULL, dwLength, &dwLength);

        // alloc the memory for the token info
        pTokenInfo = malloc(dwLength);

        // get the actual token info now
        if (!GetTokenInformation(hToken, TokenType, (LPVOID)pTokenInfo, dwLength, &dwLength))
        {
            // rip guess somethings gone wrong, clean up and go
            free(pTokenInfo);
            return;
        }

        // clean up
        CloseHandle(hToken);
        
    }

    // return the tokens
    return pTokenInfo;
}

char* wchar_to_char(const wchar_t* pwchar)
{
    // get the number of characters in the string.
    int currentCharIndex = 0;
    char currentChar = pwchar[currentCharIndex];

    while (currentChar != '\0')
    {
        currentCharIndex++;
        currentChar = pwchar[currentCharIndex];
    }

    const int charCount = currentCharIndex + 1;

    // allocate a new block of memory size char (1 byte) instead of wide char (2 bytes)
    char* filePathC = (char*)malloc(sizeof(char) * charCount);

    for (int i = 0; i < charCount; i++)
    {
        // convert to char (1 byte)
        char character = pwchar[i];

        *filePathC = character;

        filePathC += sizeof(char);

    }
    filePathC += '\0';

    filePathC -= (sizeof(char) * charCount);

    return filePathC;
}

char* whoami(BOOL DisplayAll, char* oBuffer)
{
    DWORD pCount;
    PSID pSid = 0;
    LPWSTR pSidStr = 0;
    LPWSTR UserName, SPN;
    ULONG uSize = MAX_PATH;

    OutputDebugStringA("Start of whoami\n");

    // get all the token info into our structs
    PTOKEN_GROUPS pGroupInfo = (PTOKEN_GROUPS)GetTokenInfo(TokenGroups);
    PTOKEN_PRIVILEGES pPrivInfo = (PTOKEN_PRIVILEGES)GetTokenInfo(TokenPrivileges);

    OutputDebugStringA("this is the first\n");

    sprintf(oBuffer, "User Infomation\n");
    sprintf(oBuffer, "%s===============\n\n", oBuffer);

    OutputDebugStringA("this is the second\n");

    sprintf(oBuffer, "%sUser Name\t\t\tSPN\t\t\t\tSID\n", oBuffer);
    sprintf(oBuffer, "%s---------\t\t\t---\t\t\t\t---\n", oBuffer);

    OutputDebugStringA("this is the third\n");

    // alloc our memory
    SPN = (LPWSTR)malloc(MAX_PATH);
    UserName = (LPWSTR)malloc(MAX_PATH);

    // get username and spn
    GetUserNameExW(NameSamCompatible, UserName, &uSize);
    GetUserNameExW(NameServicePrincipal, SPN, &uSize);

    // make sure we have a value for our SPN
    memset(SPN, '\0', MAX_PATH);
    if (wcslen(SPN) == 0)
    {
        SPN = L" ";
    }

    // find the sid
    ConvertSidToStringSidW(pGroupInfo->Groups[0].Sid, &pSidStr);

    // format all the info
    sprintf(oBuffer, "%s%S\t", oBuffer, wchar_to_char(UserName));

    OutputDebugStringA("before wcslen\n");
    if (wcslen(SPN) <= 1)
    {
        sprintf(oBuffer, "%s%S\t\t\t\t", oBuffer, wchar_to_char(SPN));
    } else
    {
        sprintf(oBuffer, "%s%S\t", oBuffer, wchar_to_char(SPN));
    }
    OutputDebugStringA("after wcslen\n");
    sprintf(oBuffer, "%s%S\n\n", oBuffer, wchar_to_char(pSidStr));

    // display the groups
    if (DisplayAll)
    {
        OutputDebugStringA("inside if statement\n");
        SID_NAME_USE Use = 0;
        wchar_t tmpBuffer[666];
        WCHAR szGroupName[255] = {0};
        WCHAR szDomainName[255] = {0};
        DWORD cchGroupName  = _countof(szGroupName);
        DWORD cchDomainName = _countof(szGroupName);

        sprintf(oBuffer, "%sGroups\n==========\n\n", oBuffer);

        sprintf(oBuffer, "%sName\t\t\t\tSID\t\t\t\tDetails\n", oBuffer);
        sprintf(oBuffer, "%s----\t\t\t\t---\t\t\t\t-------\n", oBuffer);

        // start to loop thought the groups
        for (pCount = 0; pCount < pGroupInfo->GroupCount; pCount++)
        {
            // get the groups sid
            LookupAccountSidW(NULL, pGroupInfo->Groups[pCount].Sid, (LPWSTR)&szGroupName, &cchGroupName, (LPWSTR)&szDomainName, &cchDomainName, &Use);

            // only look for these
            if ((Use == SidTypeWellKnownGroup || Use == SidTypeAlias || Use == SidTypeLabel) && !(pGroupInfo->Groups[pCount].Attributes & SE_GROUP_LOGON_ID))
            {
                if (pGroupInfo->Groups[pCount].Attributes == 0x60)
                {
                    pGroupInfo->Groups[pCount].Attributes = 0x07;
                }

                // format the group name
                snwprintf((LPWSTR)&tmpBuffer, _countof(tmpBuffer), L"%s%s%s", szDomainName, cchDomainName ? L"\\" : L"", szGroupName);

                // format the sid
                ConvertSidToStringSidW(pGroupInfo->Groups[pCount].Sid, &pSidStr);

                // formatting so far
                if ((wcslen(tmpBuffer) >= 22) && (wcslen(tmpBuffer) <= 29))
                {
                    if (wcslen(pSidStr) > 7)
                    {
                        sprintf(oBuffer, "%s%S\t\t%S\t\t\t", oBuffer, wchar_to_char(tmpBuffer), wchar_to_char(pSidStr));
                    } else{
                        sprintf(oBuffer, "%s%S\t\t%S\t\t\t\t", oBuffer, wchar_to_char(tmpBuffer), wchar_to_char(pSidStr));
                    }
                } else if (wcslen(tmpBuffer) >= 30)
                {
                    if (wcslen(pSidStr) > 7)
                    {
                        sprintf(oBuffer, "%s%S  %S\t\t\t", oBuffer, wchar_to_char(tmpBuffer), wchar_to_char(pSidStr));
                    } else {
                        sprintf(oBuffer, "%s%S  %S\t\t\t\t", oBuffer, wchar_to_char(tmpBuffer), wchar_to_char(pSidStr));
                    }
                } else {
                    if (wcslen(pSidStr) > 7)
                    {
                        sprintf(oBuffer, "%s%S\t\t\t\t%S\t\t\t", oBuffer, wchar_to_char(tmpBuffer), wchar_to_char(pSidStr));
                    } else {
                        sprintf(oBuffer, "%s%S\t\t\t\t%S\t\t\t\t", oBuffer, wchar_to_char(tmpBuffer), wchar_to_char(pSidStr));
                    }
                }

                // get the attribute details
                if (pGroupInfo->Groups[pCount].Attributes & SE_GROUP_MANDATORY)
                {
                    sprintf(oBuffer, "%sMandatory group", oBuffer);
                }
                if (pGroupInfo->Groups[pCount].Attributes & SE_GROUP_ENABLED_BY_DEFAULT)
                {
                    sprintf(oBuffer, "%s, Enabled by default", oBuffer);
                }
                if (pGroupInfo->Groups[pCount].Attributes & SE_GROUP_ENABLED)
                {
                    sprintf(oBuffer, "%s, Enabled group", oBuffer);
                }
                if (pGroupInfo->Groups[pCount].Attributes & SE_GROUP_OWNER)
                {
                    sprintf(oBuffer, "%s, Owner of group", oBuffer);
                }

                sprintf(oBuffer, "%s\n", oBuffer);
            }
        }

        OutputDebugStringA("after all the groups stuff\n");

        // clean it up a bit
        sprintf(oBuffer, "%s\n\n", oBuffer);

        sprintf(oBuffer, "%sPrivileges\n==========\n\n", oBuffer);

        sprintf(oBuffer, "%sName\t\t\t\tStatus\n", oBuffer);
        sprintf(oBuffer, "%s----\t\t\t\t------\n", oBuffer);

        // start looping through the info
        for (pCount = 0; pCount < pPrivInfo->PrivilegeCount; pCount++)
        {
            DWORD success;
            char* status = (char*)malloc(15);
            DWORD pnSize = 0, dnSize = 0;
            PWSTR pName = NULL, dName = NULL;

            // get the size of the priv name
            LookupPrivilegeNameW(NULL, &pPrivInfo->Privileges[pCount].Luid, NULL, &pnSize);

            // alloc our memory for the name of the priv
            pName = (PWSTR)malloc(++pnSize * sizeof(WCHAR));

            // get the name of the priv
            LookupPrivilegeNameW(NULL, &pPrivInfo->Privileges[pCount].Luid, pName, &pnSize);

            if (pPrivInfo->Privileges[pCount].Attributes & SE_PRIVILEGE_ENABLED)
            {
                status = "ENABLED";
            } else {
                status = "DISABLED";
            }

            if (pnSize > 23)
            {
                sprintf(oBuffer, "%s%S\t%s\n", oBuffer, wchar_to_char(pName), status);
            } else {
                sprintf(oBuffer, "%s%S\t\t%s\n", oBuffer, wchar_to_char(pName), status);
            }            
        }
    }

    OutputDebugStringA("finished, about to return\n");
    return oBuffer;
}