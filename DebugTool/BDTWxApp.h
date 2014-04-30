#ifndef BEATS_BEATS_DEBUG_TOOL_BDTWXAPP_H__INCLUDE
#define BEATS_BEATS_DEBUG_TOOL_BDTWXAPP_H__INCLUDE

#include "MemoryDetector/MemoryDetectorDef.h"
#include "BeatsDebugTool.h"

class CStartUpFrame;
class CBDTWxFrame;
struct SPerformanceResult;
struct SMemoryRecord;

// This enum decides function type order, UI note page order.
enum EBDTFuncType
{
    eBDTFT_SystemInfo,
    eBDTFT_Performance,
    eBDTFT_MemoryDetect,
    eBDTFT_ComponentSystem,
    eBDTFT_InfoBoard,
    eBDTFT_Spy,

    eBDTFT_Count,
    eBDTFT_Force32bit = 0xffffffff
};
class CWxwidgetsPropertyBase;
// Define a new application
class CBDTWxApp : public wxApp
{
public:
    CBDTWxApp();
    virtual ~CBDTWxApp();

    void CacheDataForUIThread(std::vector<SPerformanceResult*>& resultThisFrame);
    void SetFramePtr(CBDTWxFrame* pFrame);
    bool IsFrameValid();
    bool GetFirstResult(std::vector<SPerformanceResult*>& outParam);
    size_t GetPerformUpdateCount();
    SMemoryFrameRecord& GetMemoryUICache();
    void SetClientMode(size_t mode);
    size_t GetClientMode();
    HANDLE GetLaunchEvent();
    bool Launch(size_t uFuncSwitcher, std::vector<std::string>* pMemHookModuleList);
    bool IsEnable(EBDTFuncType funcType);
    void AddInfoLog(const TCHAR* pLog, const TCHAR* pCatalog = 0, size_t logPos = 0, size_t color = 0);
    void UpdatePropertyInfo(const TCHAR* pCatalog, const TCHAR* pPropertyName, size_t propertyId, void* value, EReflectPropertyType type);
    void DeleteProperty(const TCHAR* pCatalog, const TCHAR* pPropertyName);
    void RegisterPropertyChangeHandler(TPropertyChangedHandler pFunc);
    CBDTWxFrame* GetMainFrame();
    void Terminate();
    const TString& GetWorkingPath();
    void ClearResultCache();

    static CBDTWxApp* GetBDTWxApp(){return static_cast<CBDTWxApp*>(wxApp::GetInstance());}
    static bool m_bInit;
    static int CMD_SHOW_WINDOW;
    static int CMD_TERMINATE;


private:
    void OnShowWindow(wxThreadEvent& event);
    void OnTerminate(wxThreadEvent& event);
    void OnIdle(wxIdleEvent& event);

private:
    CStartUpFrame* m_pStartUpFrame;
    CBDTWxFrame* m_pBDTWxFrame;

    size_t m_performUpdateCount;
    size_t m_clientMode;
    size_t m_functionSwitchBit;
    HANDLE m_launchEvent;

    std::deque<std::vector<SPerformanceResult*>> m_resultCache;

private:
    //memory cache
    SMemoryFrameRecord m_memoryRecordForUICache;
    SMemoryFrameRecord m_memoryRecordSnapshot;
    bool m_bIsAutoUpdateLastFrame;
    TString m_szWorkingPath;

    CRITICAL_SECTION m_bdtCSLock;
};
#endif