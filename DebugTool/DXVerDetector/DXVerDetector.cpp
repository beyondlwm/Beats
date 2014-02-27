#include "stdafx.h"
#include "DXVerDetector.h"
#include <dxdiag.h>

#pragma comment(lib, "Version.lib ")
#pragma comment(lib, "dxguid.lib ")

CDXVerDetector* CDXVerDetector::m_pInstance = NULL;

CDXVerDetector::CDXVerDetector(void)
{
}

CDXVerDetector::~CDXVerDetector(void)
{
}

bool CDXVerDetector::GetVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion )
{
    // Init values to unknown
    if( pdwDirectXVersion )
        *pdwDirectXVersion = 0;
    if( strDirectXVersion && cchDirectXVersion > 0 )
        strDirectXVersion[0] = 0;

    DWORD dwDirectXVersionMajor = 0;
    DWORD dwDirectXVersionMinor = 0;
    TCHAR verLetter =' ';

    // First, try to use dxdiag's COM interface to get the DirectX version.  
    // The only downside is this will only work on DX9 or later.
    // Getting the DirectX version info from DxDiag failed,  
    // so most likely we are on DX8.x or earlier

    bool bGotDirectXVersion = SUCCEEDED(GetDirectXVersionViaDxDiag(&dwDirectXVersionMajor, &dwDirectXVersionMinor, &verLetter))
        || SUCCEEDED( GetDirectXVerionViaFileVersions( &dwDirectXVersionMajor, &dwDirectXVersionMinor, &verLetter));

    if( bGotDirectXVersion )
    {
        // Set the output values to what we got and return  
        verLetter = (char)tolower(verLetter);

        if( pdwDirectXVersion )
        {
            *pdwDirectXVersion = GetVersionID(dwDirectXVersionMajor, dwDirectXVersionMinor, verLetter);
        }

        if(pdwDirectXVersion > 0 && strDirectXVersion && cchDirectXVersion > 0 )
        {
            // If strDirectXVersion is non-NULL, then set it to something  
            // like "8.1b " which would represent DX8.1b
            if( verLetter ==' ' )
                _sntprintf( strDirectXVersion, cchDirectXVersion, TEXT( "%d.%d "), dwDirectXVersionMajor, dwDirectXVersionMinor );
            else
                _sntprintf( strDirectXVersion, cchDirectXVersion, TEXT( "%d.%d%c "), dwDirectXVersionMajor, dwDirectXVersionMinor, verLetter );
            strDirectXVersion[cchDirectXVersion-1] = 0;
        }
    }
    return bGotDirectXVersion;
}


//-----------------------------------------------------------------------------
// Name: GetDirectXVersionViaDxDiag()
// Desc: Tries to get the DirectX version from DxDiag's COM interface
//-----------------------------------------------------------------------------
bool CDXVerDetector::GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor,  
                                                DWORD* pdwDirectXVersionMinor,  
                                                TCHAR* pCDXVerDetectorsionLetter )
{
    bool hr;
    bool bCleanupCOM = false;

    bool bSuccessGettingMajor = false;
    bool bSuccessGettingMinor = false;
    bool bSuccessGettingLetter = false;

    // Init COM. COM may fail if its already been inited with a different  
    // concurrency model. And if it fails you shouldn't release it.
    hr = CoInitialize(NULL) == S_OK;
    bCleanupCOM = hr;

    // Get an IDxDiagProvider
    bool bGotDirectXVersion = false;
    IDxDiagProvider* pDxDiagProvider = NULL;
    hr = CoCreateInstance( CLSID_DxDiagProvider,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IDxDiagProvider,
        (LPVOID*) &pDxDiagProvider ) == S_OK;
    if( hr )
    {
        // Fill out a DXDIAG_INIT_PARAMS struct
        DXDIAG_INIT_PARAMS dxDiagInitParam;
        ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );
        dxDiagInitParam.dwSize = sizeof(DXDIAG_INIT_PARAMS);
        dxDiagInitParam.dwDxDiagHeaderVersion = DXDIAG_DX9_SDK_VERSION;
        dxDiagInitParam.bAllowWHQLChecks = false;
        dxDiagInitParam.pReserved = NULL;

        // Init the m_pDxDiagProvider
        if( pDxDiagProvider->Initialize( &dxDiagInitParam ) == S_OK )
        {
            IDxDiagContainer* pDxDiagRoot = NULL;
            IDxDiagContainer* pDxDiagSystemInfo = NULL;

            // Get the DxDiag root container
            if( pDxDiagProvider->GetRootContainer( &pDxDiagRoot ) == S_OK )  
            {
                // Get the object called DxDiag_SystemInfo
                if( pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pDxDiagSystemInfo ) == S_OK )
                {
                    VARIANT var;
                    VariantInit( &var );

                    // Get the "dwDirectXVersionMajor " property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMajor", &var ) == S_OK;
                    if( hr && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMajor )
                            *pdwDirectXVersionMajor = var.ulVal;  
                        bSuccessGettingMajor = true;
                    }
                    VariantClear( &var );

                    // Get the "dwDirectXVersionMinor " property
                    hr = pDxDiagSystemInfo-> GetProp( L"dwDirectXVersionMinor", &var ) == S_OK;
                    if( hr && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMinor )
                            *pdwDirectXVersionMinor = var.ulVal;  
                        bSuccessGettingMinor = true;
                    }
                    VariantClear( &var );

                    // Get the "szDirectXVersionLetter " property
                    hr = pDxDiagSystemInfo-> GetProp( L"szDirectXVersionLetter", &var ) == S_OK;
                    if( hr && var.vt == VT_BSTR && var.bstrVal != NULL )
                    {
#ifdef UNICODE
                        *pCDXVerDetectorsionLetter = var.bstrVal[0];  
#else
                        char strDestination[10];
                        WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, strDestination, 10*sizeof(CHAR), NULL, NULL );
                        if( pCDXVerDetectorsionLetter )
                            *pCDXVerDetectorsionLetter = strDestination[0];  
#endif
                        bSuccessGettingLetter = true;
                    }
                    VariantClear( &var );

                    // If it all worked right, then mark it down
                    if( bSuccessGettingMajor && bSuccessGettingMinor && bSuccessGettingLetter )
                        bGotDirectXVersion = true;

                    pDxDiagSystemInfo-> Release();
                }
                pDxDiagRoot-> Release();
            }
        }
        pDxDiagProvider-> Release();
    }
    if( bCleanupCOM )
        CoUninitialize();

    return bGotDirectXVersion;
}

//-----------------------------------------------------------------------------
// Name: GetDirectXVerionViaFileVersions()
// Desc: Tries to get the DirectX version by looking at DirectX file versions
//-----------------------------------------------------------------------------
bool CDXVerDetector::GetDirectXVerionViaFileVersions( DWORD* pdwDirectXVersionMajor,  
                                                     DWORD* pdwDirectXVersionMinor,  
                                                     TCHAR* pCDXVerDetectorsionLetter )
{  
    ULARGE_INTEGER llFileVersion;  
    TCHAR szPath[512];
    TCHAR szFile[512];
    BOOL bFound = false;

    if( GetSystemDirectory( szPath, MAX_PATH ) != 0 )
    {
        szPath[MAX_PATH - 1]=0;

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT( "\\ddraw.dll") );
        if( SUCCEEDED(GetFileVersion( szFile, &llFileVersion )) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 2, 0, 95 ) ) >= 0 ) // Win9x version
            {  
                // flle is >= DX1.0 version, so we must be at least DX1.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 1;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 3, 0, 1096 ) ) >= 0 ) // Win9x version
            {  
                // flle is is >= DX2.0 version, so we must DX2.0 or DX2.0a (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 2;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 4, 0, 68 ) ) >= 0 ) // Win9x version
            {  
                // flle is is >= DX3.0 version, so we must be at least DX3.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 3;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('');
                bFound = true;
            }
        }

        // Switch off the d3drg8x.dll version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT( "\\d3drg8x.dll ") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 4, 0, 70 ) ) >= 0 ) // Win9x version
            {  
                // d3drg8x.dll is the DX3.0a version, so we must be DX3.0a or DX3.0b (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 3;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('a');
                bFound = true;
            }
        }  

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT( "\\ddraw.dll ") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 5, 0, 155 ) ) >= 0 ) // Win9x version
            {  
                // ddraw.dll is the DX5.0 version, so we must be DX5.0 or DX5.2 (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 5;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 0, 318 ) ) >= 0 ) // Win9x version
            {  
                // ddraw.dll is the DX6.0 version, so we must be at least DX6.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 0, 436 ) ) >= 0 ) // Win9x version
            {  
                // ddraw.dll is the DX6.1 version, so we must be at least DX6.1
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('');
                bFound = true;
            }
        }

        // Switch off the dplayx.dll version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT( "\\dplayx.dll ") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 3, 518 ) ) >= 0 ) // Win9x version
            {  
                // ddraw.dll is the DX6.1 version, so we must be at least DX6.1a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT( "\\ddraw.dll ") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 7, 0, 700 ) ) >= 0 ) // Win9x version
            {  
                // TODO: find win2k version

                // ddraw.dll is the DX7.0 version, so we must be at least DX7.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 7;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('');
                bFound = true;
            }
        }

        // Switch off the dinput version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT( "\\dinput.dll ") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 7, 0, 716 ) ) >= 0 ) // Win9x version
            {  
                // ddraw.dll is the DX7.0 version, so we must be at least DX7.0a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 7;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        _tcscpy( szFile, szPath );
        _tcscat( szFile, TEXT( "\\ddraw.dll ") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 0, 400 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2258, 400 ) ) >= 0) ) // Win2k/WinXP version
            {  
                // ddraw.dll is the DX8.0 version, so we must be at least DX8.0 or DX8.0a (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pCDXVerDetectorsionLetter ) *pCDXVerDetectorsionLetter = TEXT('');
                bFound = true;
            }
        }
    }
    return true;
}

int CDXVerDetector::CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 )
{
    if( ullParam1.HighPart > ullParam2.HighPart )
        return 1;
    if( ullParam1.HighPart < ullParam2.HighPart )
        return -1;

    if( ullParam1.LowPart > ullParam2.LowPart )
        return 1;
    if( ullParam1.LowPart < ullParam2.LowPart )
        return -1;

    return 0;
}

ULARGE_INTEGER CDXVerDetector::MakeInt64( WORD a, WORD b, WORD c, WORD d )
{
    ULARGE_INTEGER ull;
    ull.HighPart = MAKELONG(b,a);
    ull.LowPart = MAKELONG(d,c);
    return ull;
}

bool CDXVerDetector::GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion )
{
    if( szPath == NULL)
        return false;

    DWORD dwHandle;
    UINT  cb;
    cb = GetFileVersionInfoSize( szPath, &dwHandle );
    if (cb > 0)
    {
        BYTE* pFileVersionBuffer = new BYTE[cb];
        if( pFileVersionBuffer == NULL )
            return false;

        if (GetFileVersionInfo( szPath, 0, cb, pFileVersionBuffer))
        {
            VS_FIXEDFILEINFO* pVersion = NULL;
            if (VerQueryValue(pFileVersionBuffer, TEXT("\\"), (VOID**)&pVersion, &cb) &&
                pVersion != NULL)
            {
                pllFileVersion->HighPart = pVersion->dwFileVersionMS;
                pllFileVersion->LowPart  = pVersion->dwFileVersionLS;
                BEATS_SAFE_DELETE_ARRAY(pFileVersionBuffer);
                return true;
            }
        }
        BEATS_SAFE_DELETE_ARRAY(pFileVersionBuffer);
    }
    return false;
}

size_t CDXVerDetector::GetVersionID( DWORD dwDirectXVersionMajor, DWORD dwDirectXVersionMinor, TCHAR cDirectXVersionLetter )
{
    DWORD dwDirectXVersion = dwDirectXVersionMajor;
    dwDirectXVersion <<= 8;
    dwDirectXVersion += dwDirectXVersionMinor;
    dwDirectXVersion <<= 8;
    if( cDirectXVersionLetter >='a' && cDirectXVersionLetter <='z' )
        dwDirectXVersion += (cDirectXVersionLetter -'a') + 1;
    return dwDirectXVersion;
}
