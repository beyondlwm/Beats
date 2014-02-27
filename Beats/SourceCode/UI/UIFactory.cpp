#include "stdafx.h"
#include "UIFactory.h"
#include "../game/UIPage/TitlePage.h"
#include "../Game/UIPage/RecordTrackPage.h"
#include "../Game/UIPage/PlayTrackPage.h"
#include "UIManager.h"

CUIFactory* CUIFactory::m_pInstance = NULL;

CUIFactory::CUIFactory()
{

}

CUIFactory::~CUIFactory()
{

}

CUIPageBase* CUIFactory::CreateUI( EUIPageName name )
{
    CUIPageBase* result = NULL;
    switch (name)
    {
    case eUIPN_Title:
        result = new CTitlePage();
        result->Init(_T("Title.layout"), eTPC_Count, TitlePageCompName);
        break;
    case eUIPN_RecordTrack:
        result = new CRecordTrackPage();
        result->Init(_T("RecordTrack.layout"), eRTPC_Count, RecordTrackPageCompName);
        break;
    case eUIPN_PlayTrack:
        result = new CPlayTrackPage();
        result->Init(_T("PlayTrack.layout"), ePTPC_Count, PlayTrackPageCompName);
        break;
    default:
        break;
    }
    BEATS_ASSERT(result != NULL, _T("Create UI Failed!"));
    CUIManager::GetInstance()->RegisterPage(name, result);
    return result;
}

void CUIFactory::Init()
{

}