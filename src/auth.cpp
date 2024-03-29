#include <stdio.h>
#include <conio.h>

#include "auth.h"

HRESULT CaptureSample()
{
    HRESULT hr = S_OK;
    WINBIO_SESSION_HANDLE sessionHandle = NULL;
    WINBIO_UNIT_ID unitId = 0;
    WINBIO_REJECT_DETAIL rejectDetail = 0;
    PWINBIO_BIR sample = NULL;
    SIZE_T sampleSize = 0;

    // Connect to the system pool.
    hr = WinBioOpenSession(
        WINBIO_TYPE_FINGERPRINT, // Service provider
        WINBIO_POOL_SYSTEM,      // Pool type
        WINBIO_FLAG_RAW,         // Access: Capture raw data
        NULL,                    // Array of biometric unit IDs
        0,                       // Count of biometric unit IDs
        WINBIO_DB_DEFAULT,       // Default database
        &sessionHandle           // [out] Session handle
    );
    if (FAILED(hr))
    {
        wprintf_s(L"\n WinBioOpenSession failed. hr = 0x%x\n", hr);
        goto e_Exit;
    }

    // Capture a biometric sample.
    wprintf_s(L"\n Calling WinBioCaptureSample - Swipe sensor...\n");
    hr = WinBioCaptureSample(
        sessionHandle,
        WINBIO_NO_PURPOSE_AVAILABLE,
        WINBIO_DATA_FLAG_RAW,
        &unitId,
        &sample,
        &sampleSize,
        &rejectDetail);
    if (FAILED(hr))
    {
        if (hr == WINBIO_E_BAD_CAPTURE)
        {
            wprintf_s(L"\n Bad capture; reason: %d\n", rejectDetail);
        }
        else
        {
            wprintf_s(L"\n WinBioCaptureSample failed. hr = 0x%x\n", hr);
        }
        goto e_Exit;
    }

    wprintf_s(L"\n Swipe processed - Unit ID: %d\n", unitId);
    wprintf_s(L"\n Captured %d bytes.\n", sampleSize);

    if (sample != NULL)
    {
        PWINBIO_BIR_HEADER BirHeader = (PWINBIO_BIR_HEADER)(((PBYTE)sample) + sample->HeaderBlock.Offset);
        PWINBIO_BDB_ANSI_381_HEADER AnsiBdbHeader = (PWINBIO_BDB_ANSI_381_HEADER)(((PBYTE)sample) + sample->StandardDataBlock.Offset);
        PWINBIO_BDB_ANSI_381_RECORD AnsiBdbRecord = (PWINBIO_BDB_ANSI_381_RECORD)(((PBYTE)AnsiBdbHeader) + sizeof(WINBIO_BDB_ANSI_381_HEADER));

        DWORD width = AnsiBdbRecord->HorizontalLineLength; // Width of image in pixels
        DWORD height = AnsiBdbRecord->VerticalLineLength;  // Height of image in pixels

        PBYTE firstPixel = (PBYTE)((PBYTE)AnsiBdbRecord) + sizeof(WINBIO_BDB_ANSI_381_RECORD);

        WinBioFree(sample);
        sample = NULL;
    }
e_Exit:
    if (sample != NULL)
    {
        WinBioFree(sample);
        sample = NULL;
    }

    if (sessionHandle != NULL)
    {
        WinBioCloseSession(sessionHandle);
        sessionHandle = NULL;
    }

    // wprintf_s(L"\n Press any key to exit...");
    // _getch();

    return hr;
}

HRESULT VerifyFingerprint(WINBIO_BIOMETRIC_SUBTYPE subFactor)
{
    HRESULT hr = S_OK;
    WINBIO_SESSION_HANDLE sessionHandle = 0;
    WINBIO_UNIT_ID unitId = 0;
    WINBIO_REJECT_DETAIL rejectDetail = 0;
    WINBIO_IDENTITY identity = {0};
    BOOLEAN match = FALSE;

    // Find the identity of the user.
    hr = GetCurrentUserIdentity(&identity);
    if (FAILED(hr))
    {
        wprintf_s(L"\n User identity not found. hr = 0x%x\n", hr);
        goto e_Exit;
    }

    // Connect to the system pool.
    hr = WinBioOpenSession(
        WINBIO_TYPE_FINGERPRINT, // Service provider
        WINBIO_POOL_SYSTEM,      // Pool type
        WINBIO_FLAG_DEFAULT,     // Configuration and access
        NULL,                    // Array of biometric unit IDs
        0,                       // Count of biometric unit IDs
        NULL,                    // Database ID
        &sessionHandle           // [out] Session handle
    );
    if (FAILED(hr))
    {
        wprintf_s(L"\n WinBioOpenSession failed. hr = 0x%x\n", hr);
        goto e_Exit;
    }

    // Verify a biometric sample.
    wprintf_s(L"\n Calling WinBioVerify - Swipe finger on sensor...\n");
    hr = WinBioVerify(
        sessionHandle,
        &identity,
        subFactor,
        &unitId,
        &match,
        &rejectDetail);
    wprintf_s(L"\n Swipe processed - Unit ID: %d\n", unitId);
    if (FAILED(hr))
    {
        if (hr == WINBIO_E_NO_MATCH)
        {
            wprintf_s(L"\n- NO MATCH - identity verification failed.\n");
        }
        else if (hr == WINBIO_E_BAD_CAPTURE)
        {
            wprintf_s(L"\n- Bad capture; reason: %d\n", rejectDetail);
        }
        else
        {
            wprintf_s(L"\n WinBioVerify failed. hr = 0x%x\n", hr);
        }
        goto e_Exit;
    }
    wprintf_s(L"\n Fingerprint verified:\n", unitId);

e_Exit:
    if (sessionHandle != 0)
    {
        WinBioCloseSession(sessionHandle);
        sessionHandle = 0;
    }

    // wprintf_s(L"\n Press any key to exit...");
    // _getch();

    return hr;
}

//------------------------------------------------------------------------
// The following function retrieves the identity of the current user.
// This is a helper function and is not part of the Windows Biometric
// Framework API.
//
HRESULT GetCurrentUserIdentity(__inout PWINBIO_IDENTITY Identity)
{
    // Declare variables.
    HRESULT hr = S_OK;
    HANDLE tokenHandle = NULL;
    DWORD bytesReturned = 0;
    struct
    {
        TOKEN_USER tokenUser;
        BYTE buffer[SECURITY_MAX_SID_SIZE];
    } tokenInfoBuffer;

    // Zero the input identity and specify the type.
    ZeroMemory(Identity, sizeof(WINBIO_IDENTITY));
    Identity->Type = WINBIO_ID_TYPE_NULL;

    // Open the access token associated with the
    // current process
    if (!OpenProcessToken(
            GetCurrentProcess(), // Process handle
            TOKEN_READ,          // Read access only
            &tokenHandle))       // Access token handle
    {
        DWORD win32Status = GetLastError();
        wprintf_s(L"Cannot open token handle: %d\n", win32Status);
        hr = HRESULT_FROM_WIN32(win32Status);
        goto e_Exit;
    }

    // Zero the tokenInfoBuffer structure.
    ZeroMemory(&tokenInfoBuffer, sizeof(tokenInfoBuffer));

    // Retrieve information about the access token. In this case,
    // retrieve a SID.
    if (!GetTokenInformation(
            tokenHandle,                // Access token handle
            TokenUser,                  // User for the token
            &tokenInfoBuffer.tokenUser, // Buffer to fill
            sizeof(tokenInfoBuffer),    // Size of the buffer
            &bytesReturned))            // Size needed
    {
        DWORD win32Status = GetLastError();
        wprintf_s(L"Cannot query token information: %d\n", win32Status);
        hr = HRESULT_FROM_WIN32(win32Status);
        goto e_Exit;
    }

    // Copy the SID from the tokenInfoBuffer structure to the
    // WINBIO_IDENTITY structure.
    CopySid(
        SECURITY_MAX_SID_SIZE,
        Identity->Value.AccountSid.Data,
        tokenInfoBuffer.tokenUser.User.Sid);

    // Specify the size of the SID and assign WINBIO_ID_TYPE_SID
    // to the type member of the WINBIO_IDENTITY structure.
    Identity->Value.AccountSid.Size = GetLengthSid(tokenInfoBuffer.tokenUser.User.Sid);
    Identity->Type = WINBIO_ID_TYPE_SID;

e_Exit:

    if (tokenHandle != NULL)
    {
        CloseHandle(tokenHandle);
    }

    return hr;
}