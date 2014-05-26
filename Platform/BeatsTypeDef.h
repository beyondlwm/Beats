#ifndef BEATS_BEATSTYPEDEF_H__INCLUDE
#define BEATS_BEATSTYPEDEF_H__INCLUDE

#if (BEATS_PLATFORM == PLATFORM_WIN32)
    typedef WIN32_FIND_DATA TFileData;

#else
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
    typedef HANDLE unsigned int;
    typedef HWND unsigned int;
#endif

#endif // !BEATS_BEATSTYPEDEF_H__INCLUDE
