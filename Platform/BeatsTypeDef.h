#ifndef BEATS_BEATSTYPEDEF_H__INCLUDE
#define BEATS_BEATSTYPEDEF_H__INCLUDE

#if (BEATS_PLATFORM == PLATFORM_WIN32)
    typedef WIN32_FIND_DATA TFileData;
    
#else
    #define MAX_PATH 256
    typedef unsigned int HANDLE;
    typedef unsigned int HWND;
    typedef long long FILETIME;
    typedef int DWORD;
    typedef short WORD;

    struct SFileData
    {
        DWORD dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD nFileSizeHigh;
        DWORD nFileSizeLow;
        DWORD dwReserved0;
        DWORD dwReserved1;
        TCHAR  cFileName[ MAX_PATH ];
        TCHAR  cAlternateFileName[ 14 ];
        #if (BEATS_PLATFORM == PLATFORM_IOS)
            DWORD dwFileType;
            DWORD dwCreatorType;
            WORD  wFinderFlags;
        #endif
    };
    typedef SFileData TFileData;
#endif

#endif // !BEATS_BEATSTYPEDEF_H__INCLUDE
