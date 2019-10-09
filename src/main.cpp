#include <sstream>

#include "crypto.h"
#include "auth.h"
#include "utils.h"
#include "main.h"

int main()
{

    BYTE *input = (BYTE *)"PASSPHRASE!";
    LPWSTR descr = L"DESCRIPTION";

    DATA_BLOB EncryptOutput;
    EncryptPassphrase(input, descr, &EncryptOutput);

    char *String = BinaryToBase64(EncryptOutput.pbData, EncryptOutput.cbData);

    if (!WriteToFile("test.txt", String))
    {
        printf("Error while writing to file.");
    }

    HRESULT hr = S_OK;
    do
    {
        hr = VerifyFingerprint(WINBIO_SUBTYPE_ANY);
    } while (hr != S_OK);

    char *Base64String = ReadFromFile("test.txt");

    DWORD BufferSize = 0;
    BYTE *Buffer = Base64ToBinary(Base64String, &BufferSize);

    DATA_BLOB DecryptInput;
    DWORD cbDataInput = BufferSize;

    DecryptInput.pbData = Buffer;
    DecryptInput.cbData = cbDataInput;

    DATA_BLOB DecryptOutput;
    LPWSTR DecryptDescrOutput;
    DecryptPassphrase(&DecryptInput, &DecryptOutput, &DecryptDescrOutput);

    printf((char *)DecryptOutput.pbData);

    free(String);
    free(Base64String);
    free(Buffer);
    LocalFree(DecryptDescrOutput);
    LocalFree(EncryptOutput.pbData);
    LocalFree(DecryptOutput.pbData);

    HRESULT hr2 = S_OK;
}