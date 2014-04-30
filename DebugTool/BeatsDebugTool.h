#ifndef BEATS_BEATSDEBUGTOOL_H__INCLUDE
#define BEATS_BEATSDEBUGTOOL_H__INCLUDE

#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _USRDLL
#ifdef DEBUGTOOL_EXPORTS
#define MY_DLL_DECL __declspec(dllexport)
#else
#define MY_DLL_DECL __declspec(dllimport)
#endif
#else
#define MY_DLL_DECL
#endif

    // launch wx UI from some application that may or may not be written in wx
    MY_DLL_DECL bool BEATS_INIT_DEBUG_TOOL_FUNC();

    // run this to shutdown running threads etc.
    MY_DLL_DECL void BEATS_UNINIT_DEBUG_TOOL_FUNC();

    MY_DLL_DECL void BEATS_PERFORMDETECT_INIT_FUNC(const wchar_t* typeName[], size_t size);

    MY_DLL_DECL void BEATS_PERFORMDETECT_START_IMPL_FUNC(int type, bool isStaticRecord = false, size_t id = 0);
    MY_DLL_DECL float BEATS_PERFORMDETECT_STOP_IMPL_FUNC(int type);
    MY_DLL_DECL void BEATS_PERFORMDETECT_RESET_FUNC();

    MY_DLL_DECL void BEATS_INFOBOARD_UPDATEPROPERTY_UI_IMPL_FUNC(size_t propertyId, const TCHAR* displayName, const TCHAR* pCatalog, void* value, enum EReflectPropertyType type);
    MY_DLL_DECL void BEATS_INFOBOARD_DELETEPROPERTY_UI_IMPL_FUNC( const TCHAR* displayName, const TCHAR* pCatalog);

    typedef void(*TPropertyChangedHandler)(EReflectPropertyType, size_t prpertyId, const TCHAR* pCatalog, void* value);
    MY_DLL_DECL void BEATS_INFOBOARD_REGISTER_PROPERTYCHANGE_HANDLER(TPropertyChangedHandler handlerFunc);
    MY_DLL_DECL void BEATS_INFOBOARD_ADDLOG_IMPL_FUNC(const TCHAR* pCatalog, const TCHAR* log, size_t logPos = 0, size_t fontColor = 0);

#ifdef __cplusplus
}
#endif

//describe the project setting of client process. pass the info to debugTool.dll.
enum EClientProcessMode
{
    eCPM_Debug = 1,
    eCPM_Unicode = 2,

    eCPM_Count,
    eCPM_ForceTo32bit = 0xffffffff
};

//dynamic load launcher.
class CDebugToolLauncher
{
private:
    CDebugToolLauncher()
        : pBEATS_INIT_DEBUG_TOOL(NULL)
        , pBEATS_PERFORMDETECT_RESET(NULL)
        , pBEATS_UNINIT_DEBUG_TOOL(NULL)
        , pBEATS_PERFORMDETECT_INIT(NULL)
        , pBEATS_PERFORMDETECT_START_IMPL(NULL)
        , pBEATS_PERFORMDETECT_STOP_IMPL(NULL)
        , m_initialized(false)
        , BDTModule(NULL)
    {
        Launch(true);
    }

    ~CDebugToolLauncher()
    {
        if (m_initialized)
        {
            pBEATS_UNINIT_DEBUG_TOOL();
            FreeLibrary(BDTModule);
            BDTModule = NULL;
        }
    }

public:
    static CDebugToolLauncher& GetInstance()
    {
        static CDebugToolLauncher m_pInstance;
        return m_pInstance;
    }

    static size_t GetMode()
    {
        size_t mode = 0;
#ifdef _DEBUG
        mode |= eCPM_Debug;
#endif
#ifdef _UNICODE
        mode |= eCPM_Unicode;
#endif
        return mode;
    }

    bool Launch(bool bGlobalAuto = false)
    {
        if (!m_initialized)
        {
#ifndef BEATS_DEBUG_TOOL_DISABLE
            size_t eipForHandleFetch = 0;
            BEATS_ASSI_GET_EIP(eipForHandleFetch);
            HMODULE curModule;
            bool getModuleSuccess = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR) eipForHandleFetch, &curModule) == TRUE;
            BEATS_ASSERT(getModuleSuccess, _T("Get Module Failed!"));
            TCHAR moduleName[MAX_PATH] = {0};
            GetModuleFileName(curModule, moduleName, MAX_PATH);
            size_t strLength = _tcslen(moduleName);
            TCHAR* pSuffix = moduleName + (strLength - 3);
            bool bIsRunningUnderExe = _tcsicmp(pSuffix, _T("Exe")) == 0;
            if (!bGlobalAuto || bIsRunningUnderExe)
            {
#ifdef _DEBUG
                BDTModule = LoadLibrary(_T("DebugTool_d.dll"));
#else
                BDTModule = LoadLibrary(_T("DebugTool_r.dll"));
#endif
                if (BDTModule != NULL)
                {
                    pBEATS_INIT_DEBUG_TOOL = (TFuncType1)GetProcAddress(BDTModule, "BEATS_INIT_DEBUG_TOOL_FUNC");
                    pBEATS_PERFORMDETECT_RESET = (TFuncType1)GetProcAddress(BDTModule, "BEATS_PERFORMDETECT_RESET_FUNC");
                    pBEATS_UNINIT_DEBUG_TOOL = (TFuncType1)GetProcAddress(BDTModule, "BEATS_UNINIT_DEBUG_TOOL_FUNC");
                    pBEATS_PERFORMDETECT_INIT = (TFuncType2)GetProcAddress(BDTModule, "BEATS_PERFORMDETECT_INIT_FUNC");
                    pBEATS_PERFORMDETECT_START_IMPL = (TFuncType3)GetProcAddress(BDTModule, "BEATS_PERFORMDETECT_START_IMPL_FUNC");
                    pBEATS_PERFORMDETECT_STOP_IMPL = (TFuncType4)GetProcAddress(BDTModule, "BEATS_PERFORMDETECT_STOP_IMPL_FUNC");
                    pBEATS_INFOBOARD_REGISTER_UPDATEPROPERTY_HANDLER = (TFuncType7)GetProcAddress(BDTModule, "BEATS_INFOBOARD_REGISTER_PROPERTYCHANGE_HANDLER");
                    pBEATS_INFOBOARD_UPDATEPROPERTY_UI = (TFuncType5)GetProcAddress(BDTModule, "BEATS_INFOBOARD_UPDATEPROPERTY_UI_IMPL_FUNC");
                    pBEATS_INFOBOARD_DELETEPROPERTY_UI = (TFuncType8)GetProcAddress(BDTModule, "BEATS_INFOBOARD_DELETEPROPERTY_UI_IMPL_FUNC");
                    pBEATS_INFOBOARD_ADDLOG = (TFuncType6)GetProcAddress(BDTModule, "BEATS_INFOBOARD_ADDLOG_IMPL_FUNC");


                    m_initialized = pBEATS_INIT_DEBUG_TOOL &&
                        pBEATS_PERFORMDETECT_RESET &&
                        pBEATS_UNINIT_DEBUG_TOOL &&
                        pBEATS_PERFORMDETECT_INIT &&
                        pBEATS_PERFORMDETECT_START_IMPL &&
                        pBEATS_PERFORMDETECT_STOP_IMPL &&
                        pBEATS_INFOBOARD_REGISTER_UPDATEPROPERTY_HANDLER &&
                        pBEATS_INFOBOARD_UPDATEPROPERTY_UI &&
                        pBEATS_INFOBOARD_DELETEPROPERTY_UI &&
                        pBEATS_INFOBOARD_ADDLOG;
                    BEATS_ASSERT(m_initialized, _T("Get proc address failed in debugtool module."));

                    if (m_initialized)
                    {
                        m_initialized = pBEATS_INIT_DEBUG_TOOL();
                        if (!m_initialized)
                        {
                            MessageBox(NULL, _T("初始化DebugTool.dll失败，该插件将不会启动。"), _T("初始化失败"), MB_OK);
                            pBEATS_UNINIT_DEBUG_TOOL();
                            FreeLibrary(BDTModule);
                        }
                    }
                }
                else
                {
                    MessageBox(NULL, _T("加载DebugTool.dll失败，该插件将不会启动，请检查该文件是否存在。"), _T("加载失败"), MB_OK);
                }
            }
            else
            {
                MessageBox(NULL, _T("当前挂载环境不是EXE，请保证已经调用CDebugToolLauncher::GetInstance().Launch()进行加载"), _T("加载失败"), MB_OK);
            }
#endif
        }
        return m_initialized;
    }
    typedef bool(*TFuncType1)(void);
    TFuncType1 pBEATS_INIT_DEBUG_TOOL;
    TFuncType1 pBEATS_PERFORMDETECT_RESET;
    TFuncType1 pBEATS_UNINIT_DEBUG_TOOL;
    typedef void(*TFuncType2)(const wchar_t* typeName[], size_t size);
    TFuncType2 pBEATS_PERFORMDETECT_INIT;
    typedef void(*TFuncType3)(int type, bool, size_t);
    TFuncType3 pBEATS_PERFORMDETECT_START_IMPL;
    typedef float(*TFuncType4)(int);
    TFuncType4 pBEATS_PERFORMDETECT_STOP_IMPL;
    typedef void(*TFuncType5)(size_t, const TCHAR*, const TCHAR*, void*, EReflectPropertyType);
    TFuncType5 pBEATS_INFOBOARD_UPDATEPROPERTY_UI;
    typedef void(*TFuncType6)(const TCHAR*, const TCHAR*, size_t, size_t);
    TFuncType6 pBEATS_INFOBOARD_ADDLOG;
    typedef void(*TFuncType7)(TPropertyChangedHandler);
    TFuncType7    pBEATS_INFOBOARD_REGISTER_UPDATEPROPERTY_HANDLER;
    typedef void(*TFuncType8)(const TCHAR*, const TCHAR*);
    TFuncType8    pBEATS_INFOBOARD_DELETEPROPERTY_UI;

    HMODULE BDTModule;
    bool m_initialized;
};

#ifndef DEBUGTOOL_EXPORTS
//this refrence is for calling the launcher before main.
static CDebugToolLauncher& gDebugToolLauncher = CDebugToolLauncher::GetInstance();
#endif

#ifdef BEATS_DEBUG_TOOL_DISABLE
#define BEATS_PERFORMDETECT_INIT(a, b)
#define BEATS_PERFORMDETECT_STOP(type)
#define BEATS_PERFORMDETECT_START(type)
#define BEATS_PERFORMDETECT_RESET()

#define BEATS_INFOBOARD_UPDATEPROPERTY_UI(id, displayName, catalog, property, type)
#define BEATS_INFOBOARD_REGISTER_UPDATEPROPERTY_HANDLER(handler)
#define BEATS_INFOBOARD_ADDLOG(catalog, log, color)

#else

#define BEATS_PERFORMDETECT_INIT(a, b) if(CDebugToolLauncher::GetInstance().m_initialized){CDebugToolLauncher::GetInstance().pBEATS_PERFORMDETECT_INIT(a, b);}
#define BEATS_PERFORMDETECT_STOP(type) if(CDebugToolLauncher::GetInstance().m_initialized){CDebugToolLauncher::GetInstance().pBEATS_PERFORMDETECT_STOP_IMPL(type);}
#define BEATS_PERFORMDETECT_START(type) if(CDebugToolLauncher::GetInstance().m_initialized){int eipValue=0; BEATS_ASSI_GET_EIP(eipValue);CDebugToolLauncher::GetInstance().pBEATS_PERFORMDETECT_START_IMPL(type, false, eipValue);}
#define BEATS_PERFORMDETECT_RESET() if(CDebugToolLauncher::GetInstance().m_initialized){CDebugToolLauncher::GetInstance().pBEATS_PERFORMDETECT_RESET();}

#define BEATS_INFOBOARD_UPDATEPROPERTY_UI(id, displayName, catalog, property, type) if(CDebugToolLauncher::GetInstance().m_initialized){CDebugToolLauncher::GetInstance().pBEATS_INFOBOARD_UPDATEPROPERTY_UI(id, displayName, catalog, property, (EReflectPropertyType)type);}
#define BEATS_INFOBOARD_DELETEPROPERTY_UI(displayName, catalog) if(CDebugToolLauncher::GetInstance().m_initialized){CDebugToolLauncher::GetInstance().pBEATS_INFOBOARD_DELETEPROPERTY_UI(displayName, catalog);}
#define BEATS_INFOBOARD_REGISTER_UPDATEPROPERTY_HANDLER(handler) if(CDebugToolLauncher::GetInstance().m_initialized){CDebugToolLauncher::GetInstance().pBEATS_INFOBOARD_REGISTER_UPDATEPROPERTY_HANDLER(handler);}
#define BEATS_INFOBOARD_ADDLOG(catalog, log, color) if(CDebugToolLauncher::GetInstance().m_initialized){int eipValue=0; BEATS_ASSI_GET_EIP(eipValue);CDebugToolLauncher::GetInstance().pBEATS_INFOBOARD_ADDLOG(catalog, log, eipValue, color);}

#endif
#endif