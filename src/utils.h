#ifndef FINGERKEY_UTILS_H
#define FINGERKEY_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


#define FINGERKEY_FILENAME "test.fink"

void SetStdinEcho(bool enable);
BOOL DirectoryExists(LPCTSTR);
BOOL WriteToFile(char *FilePath, char *DataToWrite);
char *ReadFromFile(char *FilePath);
LPSTR BinaryToBase64(BYTE *InputBuffer, DWORD BufferSize);
BYTE *Base64ToBinary(char *InputString, DWORD *BufferSize);
bool LaunchDebugger(void);

#endif