#ifndef THRUNTER_H
#define THRUNTER_H

#include <stdint.h>

#include "qemu/osdep.h"

#include "qemu/host-utils.h"
#include "cpu.h"

typedef uint8_t BYTE;
typedef void* PVOID;
typedef uint32_t ULONG;

typedef struct _UNICODE_STRING {
    uint16_t Length;
    uint16_t MaximumLength;
    uint16_t* Buffer;
} UNICODE_STRING;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    BYTE Reserved1[16];
    PVOID Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[1];
    PVOID Reserved3[2];
    PVOID Ldr;
    PVOID ProcessParameters;
    PVOID Reserved4[3];
    PVOID AtlThunkSListPtr;
    PVOID Reserved5;
    ULONG Reserved6;
    PVOID Reserved7;
    ULONG Reserved8;
    ULONG AtlThunkSListPtr32;
    PVOID Reserved9[45];
    BYTE  Reserved10[96];
    PVOID PostProcessInitRoutine;
    BYTE  Reserved11[128];
    PVOID Reserved12[1];
    ULONG SessionId;
} PEB, *PPEB;

typedef struct _CLIENT_ID {
    PVOID UniqueProcess;
    PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _TEB {
    PVOID Reserved1[7];
    PVOID EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID Reserved2[2];
    PPEB  ProcessEnvironmentBlock;
    PVOID Reserved3[399];
    BYTE  Reserved4[1952];
    PVOID TlsSlots[64];
    BYTE  Reserved5[8];
    PVOID Reserved6[26];
    PVOID ReservedForOle;
    PVOID Reserved7[4];
    PVOID TlsExpansionSlots;
} TEB, *PTEB;

typedef struct {
    uintptr_t pid;
} Process;

int prepare_teb(CPUState *cpu);
int trace_syscall(CPUState *cpu);
int hook_new_proc(CPUState *cpu);
Process *get_curr_proc(CPUState *cpu);

#endif
