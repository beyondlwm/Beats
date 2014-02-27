#include "stdafx.h"
#include "game.h"
#include "../Render/RenderManager.h"
#include "../Control/InputManager.h"
#include "../Control/Keyboard.h"
#include "../ui/UIManager.h"
#include "../ui/UIPageBase.h"
#include "../Physics/PhysicsManager.h"
#include "Object/ObjectManager.h"
#include "Tasks/TaskBase.h"
#include "../DebugTool/DebugTool.h"
#include <Shlwapi.h>

CGame* CGame::m_pInstance = NULL;

CGame::CGame()
: m_hwnd(NULL)
, m_active(true)
, m_deltaTimeMs(0)
, m_frameCounter(0)
, m_pCurTask(NULL)
{

}

CGame::~CGame()
{
    std::map<int, CTaskBase*>::iterator iter = m_tasks.begin();
    for (; iter != m_tasks.end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    m_tasks.clear();
}

void CGame::Init(HWND hwnd)
{
    m_hwnd = hwnd;
    HMODULE processModule = GetModuleHandle(NULL);
    TCHAR szModuleFilePath[MAX_PATH];
    GetModuleFileName(processModule, szModuleFilePath, MAX_PATH);
    PathRemoveFileSpec(szModuleFilePath);
    m_strWorkRootPath = szModuleFilePath;
    m_strWorkRootPath.append(_T("\\"));

    SetTask(eTT_Title);
    BEATS_PERFORMDETECT_INIT(NBDT::perfromanceStr, NBDT::ePT_Count);
#ifdef _DEBUG
    CDebugTool::GetInstance()->SwitchDebugMsgPrinting(true);
#endif

}

bool CGame::Update()
{
#ifdef _DEBUG
    m_deltaTimeMs = 16.6666f;//default ms for each frame.
    //CDebugTool::GetInstance()->PrintDebugMsg();
#else
    static DWORD lastRunningTime = timeGetTime();
    DWORD runningTime = timeGetTime();
    m_deltaTimeMs = (float)(runningTime - lastRunningTime);
    lastRunningTime = runningTime;
    BEATS_PRINT(_T("delta Time: %f\n"), m_deltaTimeMs);
#endif
    if (m_active)
    {
        ++m_frameCounter;
        BEATS_PERFORMDETECT_START(NBDT::ePT_RuntimePerformance);

        BEATS_PERFORMDETECT_START(NBDT::ePT_Input);
        CInputManager::GetInstance()->Update();
        BEATS_PERFORMDETECT_STOP(NBDT::ePT_Input);

        BEATS_PERFORMDETECT_START(NBDT::ePT_Render);
        CRenderManager::GetInstance()->Update(m_deltaTimeMs);
        BEATS_PERFORMDETECT_STOP(NBDT::ePT_Render);

        BEATS_PERFORMDETECT_START(NBDT::ePT_Physics);
        CPhysicsManager::GetInstance()->Update(m_deltaTimeMs);
        BEATS_PERFORMDETECT_STOP(NBDT::ePT_Physics);

        BEATS_PERFORMDETECT_START(NBDT::ePT_Object);
        CObjectManager::GetInstance()->Update(m_deltaTimeMs);
        BEATS_PERFORMDETECT_STOP(NBDT::ePT_Object);

        BEATS_PERFORMDETECT_START(NBDT::ePT_Task);
        if (m_pCurTask != NULL)
        {
            m_pCurTask->Update();
        }
        BEATS_PERFORMDETECT_STOP(NBDT::ePT_Task);

        BEATS_PERFORMDETECT_STOP(NBDT::ePT_RuntimePerformance);

        BEATS_PERFORMDETECT_RESET();
    }
    return m_active; 
}

HWND CGame::GetHwnd()
{
    return m_hwnd;
}

void CGame::SetActive( bool active )
{
    m_active = active;
}

void CGame::SetAudioPath( const TString& path )
{
    m_audioFullPath = path;
}

void CGame::RegisterTask( int type, CTaskBase* taskBase )
{
    BEATS_ASSERT(m_tasks.find(type) == m_tasks.end(), _T("the task is already registered"));
    m_tasks[type] = taskBase;
}

void CGame::SetTask( ETaskType type)
{
    BEATS_ASSERT(m_tasks.find(type) != m_tasks.end(), _T("Can't find task with type %d"), type);
    if (m_pCurTask != m_tasks[type])
    {
        if(m_pCurTask)
        {
            m_pCurTask->UnInit();
            BEATS_PRINT(_T("Change Game State From %s to %s\n"), TaskName[m_pCurTask->GetType()], TaskName[type]);
        }
        m_pCurTask = m_tasks[type];
        m_pCurTask->Init();
    }
}

void CGame::SetAudioName( const TString& name )
{
    size_t pos = m_audioFullPath.rfind('.');
    if (pos != TString::npos)
    {
        TString suffix = m_audioFullPath.substr(pos+1);
        BEATS_STR_UPER((TCHAR*)suffix.c_str(), suffix.length()+1);
        if (suffix == _T("WAV") || suffix == _T("MP3"))
        {
            pos = m_audioFullPath.rfind(_T("\\"));
            m_audioFullPath = m_audioFullPath.substr(0, pos);
        }
    }
    m_audioFullPath += _T("\\");
    m_audioFullPath += name;
}

void CGame::SetAudioFullPath( const TString& fullPath )
{
    m_audioFullPath = fullPath;
}

void CGame::SetBRFFilePath( const TString& fullPath )
{
    m_brfFilePath = fullPath;
}

const TString& CGame::GetBRFFilePath() const
{
    return m_brfFilePath;
}

const TString& CGame::GetAudioFullPath() const
{
    return m_audioFullPath;
}

void CGame::Exit()
{
    m_active = false;
}

float CGame::GetDeltaTime() const
{
    return m_deltaTimeMs;
}

long CGame::GetFrameCount() const
{
    return m_frameCounter;
}

const TString& CGame::GetWorkRootPath() const
{
    return m_strWorkRootPath;
}