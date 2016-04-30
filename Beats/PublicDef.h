#ifndef BEATS_PUBLICDEF_H__INCLUDE
#define BEATS_PUBLICDEF_H__INCLUDE

#include <stdint.h>
#include <assert.h>
#include <string>
#include <vector>
#include "BeatsTchar.h"

#define DEVELOP_VERSION

#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
#include <android/log.h>
#elif (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
extern HWND BEYONDENGINE_HWND;
#endif
//////////////////////////////////////////////////////////////////////////
//Assert
//To call messagebox so we can afford info for the engine user.
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    #ifdef _UNICODE
        #define MessageBox MessageBoxW
    #else
        #define MessageBox MessageBoxA
    #endif
#endif
#ifndef BEATS_LOG
#define BEATS_LOG(type, logString, ...)
#endif
static const uint16_t BEATS_PRINT_BUFFER_SIZE = 4096;
#if defined(DEVELOP_VERSION) || defined(_DEBUG)
    #if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    #if defined(_DEBUG)
        #define BEATS_ASSERT_IMPL\
        int iRet = MessageBox(BEYONDENGINE_HWND, szInfoBuffer, _T("Beats assert"), MB_ABORTRETRYIGNORE | MB_ICONERROR); \
        if (iRet == IDABORT)\
        {\
            __debugbreak(); \
        }\
        else if (iRet == IDIGNORE)\
        {\
            bIgnoreThisAssert = true; \
        }
    #else
        #define BEATS_ASSERT_IMPL bIgnoreThisAssert = true;
    #endif
    #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            static bool bIgnoreThisAssert = false;\
            if (!bIgnoreThisAssert)\
            {\
                TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE]; \
                _stprintf_s(szInfoBuffer, ##__VA_ARGS__,""); \
                TCHAR szLocationBuffer[1024];\
                _stprintf_s(szLocationBuffer, _T("\nErrorID:%d at %s %d\nCondition: %s\n"), GetLastError(), _T(__FILE__), __LINE__, _T(#condition)); \
                _tcscat_s(szInfoBuffer, 1024, szLocationBuffer); \
                OutputDebugString(szInfoBuffer); \
                BEATS_LOG(ELogType::eLT_Error, szInfoBuffer, 0, nullptr); \
                BEATS_ASSERT_IMPL\
            }\
        }
    #elif(BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
        #if defined(_DEBUG)
        #define BEATS_ASSERT_IMPL(title, data) __android_log_assert(title, "BeyondEngine_Assert", "%s", data);
        #else
            #define BEATS_ASSERT_IMPL(title, data)
        #endif
        #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE]; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, "");\
            TCHAR szLocationBuffer[1024];\
            _stprintf(szLocationBuffer, _T("\nat %s %d\nCondition: %s\n"), __FILE__, __LINE__, _T(#condition)); \
            _tcscat(szInfoBuffer, szLocationBuffer);\
            BEATS_LOG(ELogType::eLT_Error, szInfoBuffer, 0, nullptr); \
            BEATS_ASSERT_IMPL(#condition, szInfoBuffer);\
        }
    #elif(BEATS_PLATFORM == BEATS_PLATFORM_IOS)
        #if defined(_DEBUG)
        #define BEATS_ASSERT_IMPL(data) assert(false && data)
        #else
        #define BEATS_ASSERT_IMPL(data)
        #endif
        #define BEATS_ASSERT(condition, ...)\
        if (!(condition))\
        {\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE]; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, "");\
            TCHAR szLocationBuffer[1024];\
            _stprintf(szLocationBuffer, _T("\nat %s %d\nCondition: %s\n"), __FILE__, __LINE__, _T(#condition)); \
            _tcscat(szInfoBuffer, szLocationBuffer);\
            _tprintf(szInfoBuffer);\
            BEATS_LOG(ELogType::eLT_Error, szInfoBuffer, 0, nullptr); \
            BEATS_ASSERT_IMPL(#condition);\
        }
    #else
        #define BEATS_ASSERT(condition, ...) {assert(condition);}
    #endif
#else
    #define BEATS_ASSERT(condition, ...)
#endif

//////////////////////////////////////////////////////////////////////////
//Warning
#if defined(DEVELOP_VERSION) || defined(_DEBUG)
#define BEATS_WARNING(condition, ...)\
    if (!(condition))\
    {\
        BEATS_PRINT(__VA_ARGS__);\
        BEATS_PRINT(_T("\tAt line :%d in file: %s\n"), __LINE__, _T(__FILE__));\
    }
#else
#define BEATS_WARNING(condition, ...)
#endif

//////////////////////////////////////////////////////////////////////////
//DEVELOP_PRINT
#if defined(DEVELOP_VERSION) || defined(_DEBUG)
    #if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
        #define BEATS_PRINT_IMPL(...)\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE]; \
            _stprintf_s(szInfoBuffer, ##__VA_ARGS__, _T("")); \
            OutputDebugString(szInfoBuffer);

    #elif (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
        #define BEATS_PRINT_IMPL(...)\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE]; \
            _stprintf(szInfoBuffer, ##__VA_ARGS__, _T(""));\
            __android_log_print(ANDROID_LOG_INFO,_T("BEATS_PRINT"), _T("%s"), szInfoBuffer);

    #else
        #define BEATS_PRINT_IMPL(...)\
            TCHAR szInfoBuffer[BEATS_PRINT_BUFFER_SIZE]; \
            _stprintf_s(szInfoBuffer, ##__VA_ARGS__, _T(""));\
            _tprintf(szInfoBuffer);

    #endif
    #define BEATS_PRINT(...)\
    {\
        BEATS_PRINT_IMPL(__VA_ARGS__);\
        BEATS_LOG(ELogType::eLT_Info, szInfoBuffer, 0, nullptr); \
    }

    #define BEATS_PRINT_EX(catalog, color, ...)\
    {\
        BEATS_PRINT_IMPL(__VA_ARGS__); \
        BEATS_LOG(ELogType::eLT_Info, szInfoBuffer, color, catalog); \
    }
#else
    #define BEATS_PRINT(...)
#endif


//////////////////////////////////////////////////////////////////////////
// SingletonDef
#define BEATS_DECLARE_SINGLETON(className)\
private:\
    className();\
    ~className();\
    static className* m_pInstance;\
public:\
    static className* GetInstance()\
    {\
    if (m_pInstance == NULL)\
    m_pInstance = new className;\
    return m_pInstance;\
    }\
    static void Destroy()\
    {\
        if (m_pInstance != NULL)\
        {\
            BEATS_SAFE_DELETE(m_pInstance);\
        }\
    }\
    static bool HasInstance()\
    {\
        return m_pInstance != NULL;\
    }\
private:\

//////////////////////////////////////////////////////////////////////////
// Delete
#define BEATS_SAFE_DELETE(p) \
    if ((p) != NULL)\
{\
    delete (p);\
    (p) = NULL;\
}

#define BEATS_SAFE_DELETE_COMPONENT(p) \
if ((p) != NULL)\
{\
    if (p->IsInitialized())\
    {\
        p->Uninitialize(); \
    }\
    delete (p); \
    (p) = NULL; \
}

#define BEATS_SAFE_DELETE_ARRAY(p) if (p!=NULL){delete[] p; p = NULL;}

#define BEATS_SAFE_DELETE_VECTOR(p)\
    for(uint32_t i = 0; i < (p).size(); ++i)\
    { BEATS_SAFE_DELETE((p)[i]);}\
    (p).clear();

#define BEATS_SAFE_DELETE_MAP(p)\
    for(auto iter = (p).begin(); iter != (p).end(); ++iter)\
    { BEATS_SAFE_DELETE((iter)->second);}\
    (p).clear();

#define BEATS_SAFE_RELEASE(p)\
    if (p != NULL)\
    {\
        p->Release();\
    }

//////////////////////////////////////////////////////////////////////////
// Utility
#define BEATS_CLIP_VALUE(var, minValue, maxValue)\
    {\
    if ((var) < (minValue)) {(var)=(minValue);}\
    else if ((var) > (maxValue)) {(var)=(maxValue);}\
    }
#define BEATS_FLOAT_EPSILON 1.192092896e-07F        //FLT_EPSILON
#define BEATS_FLOAT_EQUAL(value, floatValue) (fabs((value) - (floatValue)) < BEATS_FLOAT_EPSILON)
#define BEATS_FLOAT_GREATER(value, floatValue) (((value) - (floatValue)) >  BEATS_FLOAT_EPSILON)
#define BEATS_FLOAT_GREATER_EQUAL(value, floatValue) (BEATS_FLOAT_EQUAL(value, floatValue) || BEATS_FLOAT_GREATER(value, floatValue))
#define BEATS_FLOAT_LESS(value, floatValue) (((value) - (floatValue)) < -BEATS_FLOAT_EPSILON)
#define BEATS_FLOAT_LESS_EQUAL(value, floatValue) (BEATS_FLOAT_EQUAL(value, floatValue) || BEATS_FLOAT_LESS(value, floatValue))

#define BEATS_FLOAT_EQUAL_EPSILON(value, floatValue, epsilon) (fabs((value) - (floatValue)) < epsilon)
#define BEATS_FLOAT_GREATER_EPSILON(value, floatValue, epsilon) (((value) - (floatValue)) >  epsilon)
#define BEATS_FLOAT_GREATER_EQUAL_EPSILON(value, floatValue, epsilon) (BEATS_FLOAT_EQUAL_EPSILON(value, floatValue, epsilon) || BEATS_FLOAT_GREATER_EPSILON(value, floatValue, epsilon))
#define BEATS_FLOAT_LESS_EPSILON(value, floatValue, epsilon) (((value) - (floatValue)) < -epsilon)
#define BEATS_FLOAT_LESS_EQUAL_EPSILON(value, floatValue, epsilon) (BEATS_FLOAT_EQUAL_EPSILON(value, floatValue, epsilon) || BEATS_FLOAT_LESS_EPSILON(value, floatValue, epsilon))

#define BEATS_RAND_RANGE(minValue, maxValue) (((float)rand() / RAND_MAX) * (maxValue - minValue) + minValue)

//////////////////////////////////////////////////////////////////////////
/// Unicode Marco
#ifdef _UNICODE
#define BEATS_STR_UPER _wcsupr_s
#define BEATS_STR_LOWER _wcslwr_s

#else
#define BEATS_STR_UPER _strupr_s
#define BEATS_STR_LOWER _strlwr_s

#endif 

//////////////////////////////////////////////////////////////////////////
///Assimbly
#if BEATS_PLATFORM == BEATS_PLATFORM_WIN32
    #define BEATS_ASSI_GET_EIP__(intVariable, LineNum) _asm call label##LineNum _asm label##LineNum: _asm pop intVariable __asm{}
    #define BEATS_ASSI_GET_EIP_(intVariable, LineNum) BEATS_ASSI_GET_EIP__(intVariable, LineNum)
    #define BEATS_ASSI_GET_EIP(intVariable) BEATS_ASSI_GET_EIP_(intVariable, __LINE__)
    #define BEATS_ASSI_GET_EBP(intVariable) __asm{mov intVariable, Ebp}
    #ifdef _DEBUG
        #define BEYONDENGINE_CHECK_HEAP BEATS_ASSERT(_CrtCheckMemory() != FALSE, "_CrtCheckMemory Failed!\nFile:%s Line:%d", __FILE__, __LINE__)
    #else
        #define BEYONDENGINE_CHECK_HEAP
    #endif
#else

    #define BEATS_ASSI_GET_EIP(intVariable) 
    #define BEATS_ASSI_GET_EBP(intVariable) 
    #define BEYONDENGINE_CHECK_HEAP
#endif

#define BEYONDENGINE_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define BEYONDENGINE_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define BEYONDENGINE_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(i) ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? BEYONDENGINE_SWAP32(i) : (i) )
#define BEYONDENGINE_SWAP_INT16_LITTLE_TO_HOST(i) ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? BEYONDENGINE_SWAP16(i) : (i) )
#define BEYONDENGINE_SWAP_INT32_BIG_TO_HOST(i)    ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? (i) : BEYONDENGINE_SWAP32(i) )
#define BEYONDENGINE_SWAP_INT16_BIG_TO_HOST(i)    ((BEYONDENGINE_HOST_IS_BIG_ENDIAN == true)? (i):  BEYONDENGINE_SWAP16(i) )
#endif