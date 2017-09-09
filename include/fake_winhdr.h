#ifndef FAKE_WINHDR_H
# define FAKE_WINHDR_H

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

typedef struct _PEB {
  BYTE                          Reserved1[2];
  BYTE                          BeingDebugged;
  BYTE                          Reserved2[1];
  PVOID                         Reserved3[2];
  PVOID				Ldr; /*PPEB_LDR_DATA*/ 
  PVOID				ProcessParameters; /*PRTL_USER_PROCESS_PARAMETERS*/
  BYTE                          Reserved4[104];
  PVOID                         Reserved5[52];
  PVOID				PostProcessInitRoutine; /*PPS_POST_PROCESS_INIT_ROUTINE*/
  BYTE                          Reserved6[128];
  PVOID                         Reserved7[1];
  ULONG                         SessionId;
} PEB, *PPEB;

#endif
