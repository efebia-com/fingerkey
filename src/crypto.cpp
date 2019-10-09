#include "crypto.h"

BOOL EncryptPassphrase(BYTE *DataInput, LPWSTR DescrInput, DATA_BLOB *DataOut)
{
    DATA_BLOB DataIn;
    DWORD cbDataInput = strlen((char *)DataInput) + 1;

    DataIn.pbData = DataInput;
    DataIn.cbData = cbDataInput;

    return CryptProtectData(
        &DataIn,
        DescrInput, // A description string
                    // to be included with the
                    // encrypted data.
        NULL,       // Optional entropy not used.
        NULL,       // Reserved.
        NULL,       // Pass NULL for the
                    // prompt structure.
        0,
        DataOut);
}

BOOL DecryptPassphrase(DATA_BLOB *DataInput, DATA_BLOB *DataOutput, LPWSTR *DescrOutput)
{
    return CryptUnprotectData(
        DataInput,
        DescrOutput,
        NULL, // Optional entropy
        NULL, // Reserved
        NULL, // Here, the optional
              // prompt structure is not
              // used.
        0,
        DataOutput);
}