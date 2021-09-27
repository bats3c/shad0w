#pragma once

typedef struct _PROFILE
{
    PWCHAR lpwHost;
    PWCHAR lpwUserAgent;
    PWCHAR lpwRegister;
    PWCHAR lpwPoll;
    DWORD  dwCallbackTimeout;
    DWORD  dwSleepTime;
    BOOL   bUnHook;
    BOOL   bSyscalls;
    BOOL   bEncryptSleep;
    BOOL   bPatchAmsi;
    BOOL   bPatchEtw;
    BOOL   bCleanSleep;
} PROFILE, *PPROFILE;

VOID Start();