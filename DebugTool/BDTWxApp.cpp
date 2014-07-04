#include "stdafx.h"
#include "BDTWxApp.h"
#include "BDTWxFrame.h"
#include "PerformDetector/PerformDetector.h"
#include "MemoryDetector/MemoryDetector.h"
#include "SystemDetector/SystemDetector.h"
#include "WmiDetector/WmiDetector.h"
#include "DXVerDetector/DXVerDetector.h"
#include "StartUpFrame.h"
#include "BeatsDebugTool.h"
#include "../Utility/UtilityManager.h"
#include "../Utility/StringHelper/StringHelper.h"
#include <math.h>
#include "FilePath/FilePathTool.h"

int CBDTWxApp::CMD_SHOW_WINDOW = 0;
int CBDTWxApp::CMD_TERMINATE = 0;
bool CBDTWxApp::m_bInit = false;

CBDTWxApp::CBDTWxApp()
: m_pStartUpFrame(NULL)
, m_pBDTWxFrame(NULL)
, m_performUpdateCount(0)
, m_clientMode(0)
, m_functionSwitchBit(0)
, m_launchEvent(NULL)
, m_bIsAutoUpdateLastFrame(true)
{
    CBDTWxApp::CMD_SHOW_WINDOW = wxNewId();
    CBDTWxApp::CMD_TERMINATE = wxNewId();

    // Keep the wx "main" thread running even without windows. This greatly
    // simplifies threads handling, because we don't have to correctly
    // implement wx-thread restarting.
    //
    // Note that this only works if you don't explicitly call ExitMainLoop(),
    // except in reaction to wx_dll_cleanup()'s message. wx_dll_cleanup()
    // relies on the availability of wxApp instance and if the event loop
    // terminated, wxEntry() would return and wxApp instance would be
    // destroyed.
    //
    // Also note that this is efficient, because if there are no windows, the
    // thread will sleep waiting for a new event. We could safe some memory
    // by shutting the thread down when it's no longer needed, though.
    SetExitOnFrameDelete(false);
    SetClientMode(CDebugToolLauncher::GetMode());

    Connect(wxEVT_IDLE, wxIdleEventHandler(CBDTWxApp::OnIdle));
    Connect(CMD_SHOW_WINDOW, wxEVT_THREAD, wxThreadEventHandler(CBDTWxApp::OnShowWindow));
    Connect(CMD_TERMINATE, wxEVT_THREAD, wxThreadEventHandler(CBDTWxApp::OnTerminate));
    m_launchEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
    InitializeCriticalSection(&m_bdtCSLock);

    // Set working path.
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    m_szWorkingPath = CFilePathTool::GetInstance()->ParentPath(szPath);
}

CBDTWxApp::~CBDTWxApp()
{
    //Force to destroy all utility singleton in every refrenced module.
    CUtilityManager::GetInstance()->Destroy();

    CPerformDetector::Destroy();
    CMemoryDetector::Destroy();
    CSystemDetector::Destroy();
    CDXVerDetector::Destroy();
    CWmiDetector::Destroy();
    Disconnect(wxEVT_IDLE, wxIdleEventHandler(CBDTWxApp::OnIdle));
    Disconnect(CMD_SHOW_WINDOW, wxEVT_THREAD, wxThreadEventHandler(CBDTWxApp::OnShowWindow));
    Disconnect(CMD_TERMINATE, wxEVT_THREAD, wxThreadEventHandler(CBDTWxApp::OnTerminate));
    DeleteCriticalSection(&m_bdtCSLock);
    CloseHandle(m_launchEvent);
}

void CBDTWxApp::CacheDataForUIThread(std::vector<SPerformanceResult*>& resultThisFrame)
{
    EnterCriticalSection(&m_bdtCSLock);
    m_resultCache.push_back(resultThisFrame);
    CMemoryDetector* pMemoryDetector = CMemoryDetector::GetInstance();
    //TODO: temp place update memory logic.
    if (m_pBDTWxFrame != NULL && 
        pMemoryDetector->IsInit())
    {
        bool bNeedUpdate = m_pBDTWxFrame->IsMemoryUIChanged();
        if (!bNeedUpdate && m_pBDTWxFrame->IsAutoUpdateMemory())
        {
            size_t latestTime = pMemoryDetector->GetLatestMemoryBlockTime();
            size_t allMemoryRecordCount = pMemoryDetector->GetAllocMemorySize();
            bNeedUpdate = m_pBDTWxFrame->IsMemoryRecordChanged(allMemoryRecordCount, latestTime);
        }
        if (!m_pBDTWxFrame->IsAutoUpdateMemory() && m_bIsAutoUpdateLastFrame)
        {
            pMemoryDetector->GetRecordCache(NULL, m_memoryRecordSnapshot, eMFRT_All);
            m_memoryRecordSnapshot.type = eMFRT_All;
        }
        m_bIsAutoUpdateLastFrame = m_pBDTWxFrame->IsAutoUpdateMemory();

        if (bNeedUpdate)
        {
            //temp place for memory
            EMemoryFrameRecordType type;
            switch (m_pBDTWxFrame->GetMemoryViewType())
            {
            case eMVT_Location:
                {
                    type = eMFRT_Location;
                }
                break;
            case eMVT_Size:
                {
                    type = eMFRT_Size;
                }
                break;
            case eMVT_AllocTime:
                {
                    type = eMFRT_Time;
                }
                break;
            case eMVT_Addr:
                {
                    type = eMFRT_Address;
                }
                break;
            default:
                BEATS_ASSERT(false, _T("Never reach here!"));
                break;
            }
            int startPos = m_pBDTWxFrame->GetMemoryGridViewStartPos();
            pMemoryDetector->GetRecordCache(m_pBDTWxFrame->IsAutoUpdateMemory() ? NULL : &m_memoryRecordSnapshot, m_memoryRecordForUICache, type, startPos, m_pBDTWxFrame->GetMemoryGridItemCountLimit());
        }
    }
    LeaveCriticalSection(&m_bdtCSLock);
    WakeUpIdle();
}

void CBDTWxApp::OnShowWindow(wxThreadEvent& /*event*/)
{
    m_pStartUpFrame = new CStartUpFrame("StartUpFrame", wxPoint(50, 50), wxSize(400, 300));
    m_pStartUpFrame->Show();
    this->SetTopWindow(m_pStartUpFrame);
}

void CBDTWxApp::OnTerminate(wxThreadEvent& WXUNUSED(event))
{
    Terminate();
}

void CBDTWxApp::OnIdle( wxIdleEvent& event )
{
    EnterCriticalSection(&m_bdtCSLock);
    wxApp::OnIdle(event);
    if (m_pBDTWxFrame && m_pBDTWxFrame->IsInit())
    {
        if (m_resultCache.size() > 0 && IsEnable(eBDTFT_Performance) && !CPerformDetector::GetInstance()->IsPaused())
        {
            m_pBDTWxFrame->UpdatePerformData();
            ++m_performUpdateCount;
        }

        if (!m_memoryRecordForUICache.IsEmpty() && IsEnable(eBDTFT_MemoryDetect))
        {
            m_pBDTWxFrame->UpdateMemoryData();
            m_memoryRecordForUICache.Clear();
        }
    }
    LeaveCriticalSection(&m_bdtCSLock);
}

void CBDTWxApp::SetFramePtr( CBDTWxFrame* pFrame )
{
    EnterCriticalSection(&m_bdtCSLock);
    m_pBDTWxFrame = pFrame;
    LeaveCriticalSection(&m_bdtCSLock);
}

bool CBDTWxApp::IsFrameValid()
{
    return m_pBDTWxFrame != NULL;
}

bool CBDTWxApp::GetFirstResult(std::vector<SPerformanceResult*>& outParam)
{
    outParam.clear();
    bool result = false;
    if (m_resultCache.size() > 0)
    {
        outParam = m_resultCache.front();
        m_resultCache.pop_front();

        result = true;
    }
    return result;
}

size_t CBDTWxApp::GetPerformUpdateCount()
{
    return m_performUpdateCount;
}

SMemoryFrameRecord& CBDTWxApp::GetMemoryUICache()
{
    return m_memoryRecordForUICache;
}

void CBDTWxApp::SetClientMode( size_t mode )
{
    m_clientMode = mode;
}

size_t CBDTWxApp::GetClientMode()
{
    return m_clientMode;
}

HANDLE CBDTWxApp::GetLaunchEvent()
{
    return m_launchEvent;
}

bool CBDTWxApp::Launch(size_t uFuncSwitcher, std::vector<std::string>* pMemHookModuleList )
{
    m_functionSwitchBit = uFuncSwitcher;
    m_bInit = uFuncSwitcher != 0;
    if (m_bInit)
    {
        m_pStartUpFrame->Show(false);
        m_pBDTWxFrame = new CBDTWxFrame("BeatsDebugTool", wxPoint(50, 50), wxSize(800, 600));
        m_pBDTWxFrame->Init(uFuncSwitcher);
        m_pBDTWxFrame->Show();
        this->SetTopWindow(m_pBDTWxFrame);
        if (uFuncSwitcher & (1 << eBDTFT_MemoryDetect))
        {
            if (pMemHookModuleList->size() > 0)
            {
                CMemoryDetector::GetInstance()->Init(pMemHookModuleList);
            }
        }
    }
    SetEvent(static_cast<CBDTWxApp*>(CBDTWxApp::GetInstance())->GetLaunchEvent());
    return true;
}

bool CBDTWxApp::IsEnable( EBDTFuncType funcType )
{
    return (m_functionSwitchBit & (1 << funcType)) > 0;
}

void CBDTWxApp::AddInfoLog( const TCHAR* pLog, const TCHAR* pCatalog /*= 0*/, size_t logPos /*=0*/, size_t color /*= 0*/)
{
    m_pBDTWxFrame->AddInfoLog(pLog, pCatalog, logPos, color);
}

void CBDTWxApp::UpdatePropertyInfo(const TCHAR* pCatalog, const TCHAR* pPropertyName, size_t propertyId, void* value, EReflectPropertyType type )
{
    m_pBDTWxFrame->SetPropertyInfo(pCatalog, pPropertyName, propertyId, value, type);
}

void CBDTWxApp::DeleteProperty(const TCHAR* pCatalog, const TCHAR* pPropertyName)
{
    m_pBDTWxFrame->DeleteProperty(pCatalog, pPropertyName);
}

void CBDTWxApp::RegisterPropertyChangeHandler( TPropertyChangedHandler pFunc )
{
    m_pBDTWxFrame->RegisterPropertyChangedHandler(pFunc);
}

CBDTWxFrame* CBDTWxApp::GetMainFrame()
{
    return m_pBDTWxFrame;
}

void CBDTWxApp::Terminate()
{
    m_bInit = false;
    ExitMainLoop();
}

const TString& CBDTWxApp::GetWorkingPath()
{
    return m_szWorkingPath;
}

void CBDTWxApp::ClearResultCache()
{
    EnterCriticalSection(&m_bdtCSLock);
    m_resultCache.clear();
    LeaveCriticalSection(&m_bdtCSLock);
}
