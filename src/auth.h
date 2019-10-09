#ifndef FINGERKEY_AUTH_H
#define FINGERKEY_AUTH_H

#include <windows.h>
#include <winbio.h>
#include <wincred.h>

HRESULT GetCurrentUserIdentity(__inout PWINBIO_IDENTITY);
HRESULT CaptureSample(void);
HRESULT VerifyFingerprint(WINBIO_BIOMETRIC_SUBTYPE);

#endif