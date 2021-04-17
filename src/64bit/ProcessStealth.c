#include "ProcessStealth.h"

DWORD GetProcessPID(wchar_t* ProcessName)
{
    PSYSTEM_PROCESS_INFORMATION spi;
    DWORD PID = NULL;
    ULONG ReturnLength;

    while (TRUE)
    {
        if (NtQuerySystemInformation(SystemProcessInformation, NULL, NULL, &ReturnLength) != STATUS_INFO_LENGTH_MISMATCH)
        {
            continue;
        }

        spi = VirtualAlloc(NULL, ReturnLength, MEM_COMMIT | MEM_RELEASE, PAGE_READWRITE);

        if (spi == NULL)
        {
            continue;
        }

        if (NT_SUCCESS(NtQuerySystemInformation(SystemProcessInformation, spi, ReturnLength, &ReturnLength)))
        {
            break;
        }

        VirtualFree(spi, 0, MEM_RELEASE);
    }

    PSYSTEM_PROCESS_INFORMATION temp = spi;
    spi = (ULONGLONG)spi + spi->NextEntryOffset;

    while (TRUE)
    {
        if (wcsicmp(spi->ImageName.Buffer, ProcessName) == 0)
        {
            PID = spi->UniqueProcessId;
            break;
        }

        if (spi->NextEntryOffset == 0)
            break;

        spi = (ULONGLONG)spi + spi->NextEntryOffset;
    }
    
    VirtualFree(temp, ReturnLength, MEM_DECOMMIT);
    VirtualFree(temp, 0, MEM_RELEASE);

    return PID;
}