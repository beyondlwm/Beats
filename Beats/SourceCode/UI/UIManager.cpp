#include "stdafx.h"
#include "UIManager.h"
#include "UISystem/CEGUISystemImpl.h"
#include "UIPagebase.h"
#include "UIFactory.h"

CUIManager* CUIManager::m_pInstance = NULL;

CUIManager::CUIManager()
: m_winCursorVisible(true) 
, m_pSystem(new CCEGUISystemImpl())
{

}

CUIManager::~CUIManager()
{
    BEATS_SAFE_DELETE(m_pSystem);
    std::map<EUIPageName, CUIPageBase*>::iterator iter = m_pagePool.begin();
    for (;iter != m_pagePool.end(); ++iter)
    {
        BEATS_SAFE_DELETE((iter->second));
    }
    m_pagePool.clear();
}

void CUIManager::Init()
{
    m_pSystem->Init();
}

void CUIManager::Update(float /*deltaTime*/)
{
    BEATS_PERFORMDETECT_START(NBDT::ePT_UpdateUISystem);
    m_pSystem->Update();
    BEATS_PERFORMDETECT_STOP(NBDT::ePT_UpdateUISystem);

    BEATS_PERFORMDETECT_START(NBDT::ePT_UpdateUIPages);
    CUIPageBase* pPage = NULL;
    for (std::set<EUIPageName>::iterator iter = m_activePage.begin(); iter != m_activePage.end(); ++iter)
    {    
        pPage = m_pagePool[*iter];
        if (pPage && pPage->IsOpen())
        {
            pPage->Update();
        }
    }
    BEATS_PERFORMDETECT_STOP(NBDT::ePT_UpdateUIPages);
}

void CUIManager::RegisterPage( EUIPageName name, CUIPageBase* page )
{
    BEATS_ASSERT(m_pagePool.find(name) == m_pagePool.end(), _T("The page is already registered!"));
    m_pagePool[name] = page;
}

CUIPageBase* CUIManager::GetUIPage( EUIPageName name)
{
    if (m_pagePool.find(name) == m_pagePool.end())
    {
        CUIFactory::GetInstance()->CreateUI(name);
    }
    return m_pagePool[name];
}

//TODO: figure out why no refrence of active
void CUIManager::SetPageActive( EUIPageName name, bool /*active*/ )
{
    BEATS_ASSERT(m_pagePool.find(name) != m_pagePool.end(), _T("Can't find the page! %d"), name);
    m_activePage.insert(name);
}

void CUIManager::ShowWinCursor( bool visible )
{
    /// to avoid call ShowCursor(true/false) when it is already true/false.
    /// or the win API will not work any more.
    if (visible != m_winCursorVisible)
    {
        ShowCursor(visible);
        m_winCursorVisible = visible;
    }
}
