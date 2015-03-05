#include "stdafx.h"
#include "UIPageBase.h"
#include "UIManager.h"
#include "../Game/Game.h"
#include "CEGUI.h"

CUIPageBase::CUIPageBase(EUIPageName pageName)
: m_isOpen(false)
, m_name(pageName)
, m_pParent(NULL)
, m_pRootWindow(NULL)
{
}

CUIPageBase::~CUIPageBase()
{

}

bool CUIPageBase::IsOpen()
{
    return m_isOpen;
}

void CUIPageBase::Open()
{
    m_isOpen = OpenImpl();
    Activate(true);
    CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pRootWindow);
}

void CUIPageBase::Close()
{
    m_isOpen = !CloseImpl();
    Activate(false);
    CEGUI::System::getSingleton().getGUISheet()->removeChildWindow(m_pRootWindow);
}

void CUIPageBase::SetRoot( CEGUI::Window* pRoot )
{
    m_pRootWindow = pRoot;
    BEATS_ASSERT(m_pRootWindow, _T("Set Root Failed!"));
}

void CUIPageBase::Activate(bool active)
{
    CUIManager::GetInstance()->SetPageActive(m_name, active);
}

void CUIPageBase::Init(const TCHAR* layoutFileName, int compCount, const TCHAR* compNames[])
{
    //restore current working directory before cegui load files(relative path).
    const TString& strWorkRootPath = CGame::GetInstance()->GetWorkRootPath();
    SetCurrentDirectory(strWorkRootPath.c_str());

    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
#ifdef _UNICODE
    char buffer[64];
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)layoutFileName, -1,(char *)buffer, 64, NULL, NULL);
    SetRoot(winMgr.loadWindowLayout(buffer));
#else
    SetRoot(winMgr.loadWindowLayout(layoutFileName));
#endif
    InitComponents(compCount, compNames);
    SetAllComponentsVisible(false);
    InitUIEvent();
    Init();
}

void CUIPageBase::Init()
{
    //do nothing
}

void CUIPageBase::InitComponents( int count, const TCHAR* names[] )
{
    for (int i = 0; i < count; ++i)
    {
        m_pComponents.push_back(m_pRootWindow->getChildRecursive(names[i]));
    }
}

void CUIPageBase::InitUIEvent()
{
    //do nothing.
}

void CUIPageBase::SetAllComponentsVisible( bool visible )
{
    for (uint32_t i = 0; i < m_pComponents.size(); ++i)
    {
        if (m_pComponents[i] != NULL)
        {
            m_pComponents[i]->setVisible(visible);
        }
    }
}

CEGUI::Window* CUIPageBase::GetRoot() const
{
    return m_pRootWindow;
}