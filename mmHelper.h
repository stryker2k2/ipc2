#include <windows.h>
#include <stdio.h>
#include "mmLoader.h"

char* readDLLFile(char* dllName);
FARPROC libLoadMemory(char *dllName, char *procName);