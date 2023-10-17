#include <windows.h>
#include <stdio.h>
#include "mmLoader.h"

char* readDLLFile(char* dllName)
{
    FILE *fp = fopen(dllName, "rb");
    long int *procSz;

    if (!fp)
        { printf("[!] Failed to load library\n"); exit(-1); }

    fseek(fp, 0, SEEK_END);
    long int fpSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // printf("[+] %s is %ld bytes\n", dllName, fpSize);

    char *dllBuffer = (char*)malloc(fpSize);

    fread((char *)dllBuffer, fpSize, 1, fp);

    // Prints "MZ" if all goes well
    // printf("Magic Number: \n");
    // for(int i = 0x00; i <= 0x01; i++)
    // {
    //     printf("%c", dllBuffer[i]);
    // }
    // printf("\n");

    fclose(fp);

    *procSz = fpSize;
    printf("[+] (%s) procSz: %x\n", __func__, *procSz);
    return dllBuffer;
}

// Loads a local DLL into Memory then calls LoadLibary on the DLL in memory
FARPROC libLoadMemory(char *dllName, char *procName)
{
    char *dllBuffer = dllName;

    // printf("[+] (%s) procSz: %x\n", __func__, *procSz);

    DWORD pdwError;

    HMEMMODULE hSharedLib = LoadMemModule(dllBuffer, FALSE, &pdwError);
    if (!hSharedLib)
    {
        printf("[!] pdwError: %x\n", pdwError);
        exit(-1);
    }

    printf("[+] Payload DLL Successfully Loaded into Memory\n");

    // printf("[+] Loading %s from hSharedLib: 0%08x\n", procName, hSharedLib);
    FARPROC fpSharedProc = GetMemModuleProc(hSharedLib, procName);

    free(dllBuffer);

    if (fpSharedProc)
    {
        // printf("[+] fpSharedProc is 0x%08x\n", fpSharedProc);
        printf("[+] Handle to Payload DLL Function Successfully Obtained\n");
        return fpSharedProc;
    }

    else
    {
        // printf("[!] fpSharedProc is NULL\n");
        return NULL;
    }
}