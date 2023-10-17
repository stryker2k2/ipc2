#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include "mmHelper.h"

#define BUF_SIZE 256
char *szName = "common_dependencies";

char * injestDLL(char *name)
{
    char *buf;
    printf("[+] Reading DLL into dllBuffer\n");
    buf = readDLLFile(name);

    if (buf == NULL)
    {
        printf("Could not find %s\n", name);

        CloseHandle(buf);

        return NULL;
    }

    printf("[+] %s Magic Numer: %c %c\n", name, buf[0], buf[1]);
    printf("[+] %s Location: %x\n", name, (PVOID)buf);

    return buf;
}

int main()
{
    HANDLE hMapFile;
    char *pBuf;
    char *dllBuffer;
    char *payloadBuffer;

    /* Load Common Dependencies into Local Memory Space */
    char *dllName = "./output/common_deps.dll";
    dllBuffer = injestDLL(dllName);

    /* Load Payload (which will require Common Dependencies) into Local Memory Space */
    char *payloadName = "./output/payload.dll";
    payloadBuffer = injestDLL(payloadName);

    /* Create Shared Memory Space to store Common Dependencies' Address */
    printf("[+] Creating File Map Handle\n");
    hMapFile = CreateFileMappingA(
                    INVALID_HANDLE_VALUE,    // use paging file
                    NULL,                    // default security
                    PAGE_READWRITE,          // read/write access
                    0,                       // maximum object size (high-order DWORD)
                    BUF_SIZE,                // maximum object size (low-order DWORD)
                    szName);                 // name of mapping object

    if (hMapFile == NULL)
    {
        printf("[!] Could not create file mapping object (%d).\n",
                GetLastError());
        return 1;
    }

    printf("[+] Acquring Map View of File Handle\n");

    /* Access Shared Memory Space to store Common Dependencies' Address */
    pBuf = (char *) MapViewOfFile(hMapFile,   // handle to map object
                            FILE_MAP_ALL_ACCESS, // read/write permission
                            0,
                            0,
                            BUF_SIZE);

    if (pBuf == NULL)
    {
        printf("Could not map view of file (%d).\n",
                GetLastError());

        CloseHandle(hMapFile);

        return 1;
    }

    printf("[+] Copying dllBuffer into Map View of File Memory\n");
    printf("[+] dllBuffer location is %x and the pointer is %d bytes in size.\n", (PVOID)dllBuffer, sizeof((PVOID)dllBuffer));


    /* Copy Common Dependencies' Address into Shared Memory Space */
    char buffPtr[8];
    sprintf(buffPtr, "%x", (PVOID)dllBuffer);
    printf("[+] buffPtr is %s and the pointer is %d bytes in size.\n", buffPtr, sizeof(buffPtr));
    CopyMemory((PVOID)pBuf, buffPtr, sizeof(buffPtr));


    /* Load 'mapFile' Module/Function from the 'Payload' Library */
    FARPROC _mapFile = libLoadMemory(payloadBuffer, "mapFile");
    if(_mapFile)
    {
        _mapFile();
    }

    /* Cleaup */
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}