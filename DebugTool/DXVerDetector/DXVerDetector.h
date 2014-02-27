
#ifndef BEATS_DEBUGTOOL_DXVERDETECTOR_DXVERDETECTOR_H__INCLUDE
#define BEATS_DEBUGTOOL_DXVERDETECTOR_DXVERDETECTOR_H__INCLUDE

class CDXVerDetector
{
    BEATS_DECLARE_SINGLETON(CDXVerDetector);
public:
    bool GetVersion(DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion = NULL, int cchDirectXVersion = 0 );

private:
    bool GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
    bool GetDirectXVerionViaFileVersions( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
    bool GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion );
    ULARGE_INTEGER MakeInt64( WORD a, WORD b, WORD c, WORD d );
    int CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 );

    //////////////////////////////////////////////////////////////////////////
    ///version code
    //  0x00000000 = No DirectX installed
    //  0x00010000 = DirectX 1.0 installed
    //  0x00020000 = DirectX 2.0 installed
    //  0x00030000 = DirectX 3.0 installed
    //  0x00030001 = DirectX 3.0a installed
    //  0x00050000 = DirectX 5.0 installed
    //  0x00060000 = DirectX 6.0 installed
    //  0x00060100 = DirectX 6.1 installed
    //  0x00060101 = DirectX 6.1a installed
    //  0x00070000 = DirectX 7.0 installed
    //  0x00070001 = DirectX 7.0a installed
    //  0x00080000 = DirectX 8.0 installed
    //  0x00080100 = DirectX 8.1 installed
    //  0x00080101 = DirectX 8.1a installed
    //  0x00080102 = DirectX 8.1b installed
    //  0x00080200 = DirectX 8.2 installed
    //  0x00090000 = DirectX 9.0 installed
    size_t GetVersionID(DWORD dwDirectXVersionMajor, DWORD dwDirectXVersionMinor, TCHAR cDirectXVersionLetter);
};

#endif 