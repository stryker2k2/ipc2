#include <windows.h>
#include <stdio.h>
#include "payload.h"
#include "mmHelper.h"
#pragma comment(lib, "user32.lib")

#define BUF_SIZE 256
char *szName = "common_dependencies";

int testMsgBox(void)
{
     MessageBoxA(NULL, "Test Message from payload.dll", "Payload", MB_OK);

     return 0;
}

void changePtr(int **p, int *newAddr)
{
     *p = newAddr;
}

int mapFile(void)
{
     HANDLE hMapFile;
     char *pBuf;
     FARPROC _printSuccess;
     long int procSz;

     /* Obtain Handle of a Pre-Existing File Mapping */
     hMapFile = OpenFileMappingA(
                    FILE_MAP_ALL_ACCESS,   // read/write access
                    FALSE,                 // do not inherit the name
                    szName);               // name of mapping object

     if (hMapFile == NULL)
     {
          printf("Could not open file mapping object (%d).\n",
               GetLastError());
          MessageBoxA(NULL, "Could not open file mapping object", "Payload", MB_OK | MB_ICONWARNING);
          return 1;
     }

     /* Read Contents from File Mapping */
     pBuf = (char *) MapViewOfFile(hMapFile, // handle to map object
               FILE_MAP_ALL_ACCESS,  // read/write permission
               0,                       // dwFileOffsetHigh
               0,                       // dwFileOffsetLow
               BUF_SIZE);                      // dwNumberOfBytesToMap

     if (pBuf == NULL)
     {
          printf("Could not map view of file (%d).\n", GetLastError());
          MessageBoxA(NULL, "Could not open file mapping object", "Payload", MB_OK | MB_ICONWARNING);
          CloseHandle(hMapFile);

          return 1;
     }

     char output[8];
     sprintf(output, "%s", (PVOID)pBuf);
     int outputLoc = (int) strtol(output, NULL, 16);
     printf("[+] outputLoc is %x\n", outputLoc);

     /* Testing out that we have the right Magic Number (ex: MZ) */
     int bufSize = 48;
     char *contentPtr = malloc(&bufSize);
     CopyMemory((PVOID)contentPtr, (PVOID)outputLoc, 48);
     for (int i = 0; i < bufSize; i++)
     {
          printf("%c", contentPtr[i]);
          if (i == (bufSize - 1))
            printf("EOL\n");
     }
     free(contentPtr);


     /* Creating a temporary pointer to host correct pointer location */
     char *myPointer;
     printf("[+] myPointer is: %x\n", myPointer);
     changePtr(&myPointer, outputLoc);
     printf("[+] myPointer is: %x\n", myPointer);


     /* Launching 'PrintSuccess' from 'Common Deps' in Memory */
     _printSuccess = libLoadMemory(myPointer, "printSuccess");
     if (!_printSuccess)
     {
          MessageBoxA(NULL, "printSuccess is NULL", "Payload", MB_OK | MB_ICONWARNING);
          UnmapViewOfFile(pBuf);
          CloseHandle(hMapFile);
          return 1;
     }
     _printSuccess();
     printf("[+] Success!\n");


     /* Clean Up */
     UnmapViewOfFile(pBuf);
     CloseHandle(hMapFile);

     return 0;
}