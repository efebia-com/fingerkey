#ifndef FINGERKEY_CRYPTO_H
#define FINGERKEY_CRYPTO_H

#include <windows.h>
#include <wincrypt.h>

BOOL EncryptPassphrase(BYTE *DataInput, LPWSTR DescrInput, DATA_BLOB *DataOut);
BOOL DecryptPassphrase(DATA_BLOB *DataInput, DATA_BLOB *DataOutput, LPWSTR *DescrOutput);

#endif