#include <string>

#include "utils.h"

void SetStdinEcho(bool enable)
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if (!enable)
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode);
}

BOOL DirectoryExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL WriteToFile(char *FilePath, char *DataToWrite)
{
    HANDLE h = CreateFileA(FilePath,      // name of the file
                           GENERIC_WRITE, // open for writing
                           0,             // sharing mode, none in this case
                           0,             // use default security descriptor
                           CREATE_ALWAYS, // overwrite if exists
                           FILE_ATTRIBUTE_NORMAL,
                           0);

    DWORD dwBytesToWrite = (DWORD)strlen(DataToWrite);
    DWORD dwBytesWritten = 0;
    BOOL bErrorFlag = FALSE;

    bErrorFlag = WriteFile(
        h,               // open file handle
        DataToWrite,     // start of data to write
        dwBytesToWrite,  // number of bytes to write
        &dwBytesWritten, // number of bytes that were written
        NULL);           // no overlapped structure

    CloseHandle(h);

    return bErrorFlag;
}

char *ReadFromFile(char *FilePath)
{
    FILE *f = fopen(FilePath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    char *OutputBuffer = (char *)malloc(fsize + 1);
    fread(OutputBuffer, 1, fsize, f);
    fclose(f);
    return OutputBuffer;
}

LPSTR BinaryToBase64(BYTE *InputBuffer, DWORD BufferSize)
{
    DWORD NumberOfChars = 0;
    CryptBinaryToStringA(
        InputBuffer,
        BufferSize,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        NULL,
        &NumberOfChars);

    LPSTR String = (LPSTR)malloc(NumberOfChars * sizeof(CHAR));
    CryptBinaryToStringA(
        InputBuffer,
        BufferSize,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        String,
        &NumberOfChars);

    return String;
}

BYTE *Base64ToBinary(char *InputString, DWORD *BufferSize)
{
    DWORD NumberOfBytes = 0;
    CryptStringToBinaryA(
        InputString,
        NULL,
        CRYPT_STRING_BASE64,
        NULL,
        &NumberOfBytes,
        NULL,
        NULL);

    BYTE *Buffer = (BYTE *)malloc(NumberOfBytes * sizeof(BYTE));
    CryptStringToBinaryA(
        InputString,
        strlen(InputString),
        CRYPT_STRING_BASE64,
        Buffer,
        &NumberOfBytes,
        NULL,
        NULL);

    *BufferSize = NumberOfBytes;

    return Buffer;
}

// bool LaunchDebugger(void)
// {
//     // Get System directory, typically c:\windows\system32
//     std::wstring systemDir(MAX_PATH + 1, '\0');
//     UINT nChars = GetSystemDirectoryW(&systemDir[0], systemDir.length());
//     if (nChars == 0)
//         return false; // failed to get system directory
//     systemDir.resize(nChars);

//     // Get process ID and create the command line
//     DWORD pid = GetCurrentProcessId();
//     std::wostringstream s;
//     s << systemDir << L"\\vsjitdebugger.exe -p " << pid;
//     std::wstring cmdLine = s.str();

//     // Start debugger process
//     STARTUPINFOW si;
//     ZeroMemory(&si, sizeof(si));
//     si.cb = sizeof(si);

//     PROCESS_INFORMATION pi;
//     ZeroMemory(&pi, sizeof(pi));

//     if (!CreateProcessW(NULL, &cmdLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
//         return false;

//     // Close debugger process handles to eliminate resource leak
//     CloseHandle(pi.hThread);
//     CloseHandle(pi.hProcess);

//     // Wait for the debugger to attach
//     while (!IsDebuggerPresent())
//         Sleep(100);

//     // Stop execution so the debugger can take over
//     DebugBreak();
//     return true;
// }