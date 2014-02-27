#include "stdafx.h"
#include "TitlePage.h"
#include "cegui.h"
#include "shlobj.h"
#include "../Game.h"
#include "../../UI/UIManager.h"
#include "../Tasks/TitleTask.h"
#include "../../Control/InputManager.h"
#include "../../Function/Bind.h"

CTitlePage::CTitlePage()
: super(eUIPN_Title)
, m_pOwner(NULL)
{
}

CTitlePage::~CTitlePage()
{

}

void CTitlePage::Init()
{
    INIT_FSM(CTitlePage, Welcome);
}

bool CTitlePage::OpenImpl()
{
    return true;
}

bool CTitlePage::CloseImpl()
{
    return true;
}

CEGUI::ItemEntry* CTitlePage::GetSelectItem()
{
    CEGUI::ItemListbox* pList = static_cast<CEGUI::ItemListbox*>(m_pComponents[eTPC_TrackList]);
    BEATS_ASSERT(pList != NULL, _T("List is null!"));
    return pList->getFirstSelectedItem();
}

void CTitlePage::InitUIEvent()
{
    m_pComponents[eTPC_CreateBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickCreateBtn, this));
    m_pComponents[eTPC_PlayBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickPlayBtn, this));
    m_pComponents[eTPC_SelectBackBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickSelectBackBtn, this));
    m_pComponents[eTPC_BrowseBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickBrowseBtn, this));
    m_pComponents[eTPC_SelectOkBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickSelectOkBtn, this));
    m_pComponents[eTPC_TrackList]->subscribeEvent(CEGUI::ItemListbox::EventSelectionChanged, CEGUI::Event::Subscriber(&CTitlePage::OnSelectItem, this));

    m_pComponents[eTPC_BackBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickBackBtn, this));
    m_pComponents[eTPC_SelectAudioFileBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickSelectAudioBtn, this));
    m_pComponents[eTPC_SelectBRFileBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickSelectBRFBtn, this));
    m_pComponents[eTPC_StartBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CTitlePage::OnClickStartBtn, this));

}

bool CTitlePage::OnClickCreateBtn(const CEGUI::EventArgs& /*args*/)
{
    SWITCH_FSM_STATE(true, CTitlePage, CreateTrack);
    ClearTrackList();
    return true;
}

void CTitlePage::Update()
{
    UPDATE_FSM();
}

bool CTitlePage::OnClickPlayBtn( const CEGUI::EventArgs& /*args*/ )
{
    SWITCH_FSM_STATE(true, CTitlePage, PlayTrack);
    return true;
}

bool CTitlePage::OnClickSelectBackBtn( const CEGUI::EventArgs& /*args*/ )
{
    SWITCH_FSM_STATE(true, CTitlePage, Welcome);
    return true;
}

bool CTitlePage::OnClickBrowseBtn( const CEGUI::EventArgs& /*args*/ )
{
    TString audioPath;
    bool result = false;
    if (AcquireAudioPath(audioPath))
    {
        CGame::GetInstance()->SetAudioPath(audioPath);
        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFile(audioPath.append(_T("\\*.mp3")).c_str(),&ffd);
        ClearTrackList();
        if (hFind != INVALID_HANDLE_VALUE)
        {
            AddToTrackList(ffd.cFileName);
            while(FindNextFile(hFind,&ffd))
            {
                AddToTrackList(ffd.cFileName);
            }
            FindClose(hFind);
        }
        result = true;
    }

    return result;
}

bool CTitlePage::AcquireAudioPath(TString& path)
{
    BROWSEINFO bf;
    memset(&bf,0,sizeof(BROWSEINFO));  
    bf.hwndOwner = CGame::GetInstance()->GetHwnd();  
    bf.lpszTitle = _T("Ñ¡ÔñMP3ÎÄ¼þ¼Ð");
    bf.ulFlags = BIF_RETURNONLYFSDIRS;
    ///Force show cursor
    CUIManager::GetInstance()->ShowWinCursor(true);
    LPITEMIDLIST pItem = SHBrowseForFolder(&bf); 
    CUIManager::GetInstance()->ShowWinCursor(false);
    TCHAR Buffer[MAX_PATH];
    bool result = false;
    if (pItem)
    {
        SHGetPathFromIDList(pItem, Buffer);
        path.assign(Buffer);
        result = true;
    }
    return result;
}

void CTitlePage::AddToTrackList( const TCHAR* name )
{
    CEGUI::ItemListbox* pList = static_cast<CEGUI::ItemListbox*>(m_pComponents[eTPC_TrackList]);
    char szBuffer[1024];
    CStringHelper::GetInstance()->ConvertToCHAR(name, szBuffer, 1024);
    CEGUI::ItemEntry* pItemEntry = new CEGUI::ItemEntry("Falagard/ItemEntry", szBuffer);
    pItemEntry->setWindowRenderer("Falagard/ItemEntry");
    pItemEntry->setLookNFeel("TaharezLook/ListboxItem");

    pItemEntry->setText(szBuffer);
    pItemEntry->setFont("STZHONGS");

    pList->addItem(pItemEntry);
    m_listEntries.push_back(pItemEntry);
}

void CTitlePage::ClearTrackList()
{
    CEGUI::ItemListbox* pList = static_cast<CEGUI::ItemListbox*>(m_pComponents[eTPC_TrackList]);
    for (size_t i = 0; i < m_listEntries.size(); ++i )
    {
        pList->removeItem(m_listEntries[i]);
    }
    m_listEntries.clear();
}

bool CTitlePage::OnClickSelectOkBtn( const CEGUI::EventArgs& /*args*/ )
{
    CEGUI::ItemEntry* pSelectedEntry = GetSelectItem();
    if (pSelectedEntry != NULL)
    {
        CEGUI::String text = pSelectedEntry->getText();
#ifdef _UNICODE
        wchar_t result[256];
        MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, result, 256);
        CGame::GetInstance()->SetAudioName(result);
#else
        CGame::GetInstance()->SetAudioName(text.c_str());
#endif
        CGame::GetInstance()->SetTask(eTT_RecordTrack);
    }
    return true;
}

bool CTitlePage::OnSelectItem( const CEGUI::EventArgs& /*args*/ )
{
    CEGUI::ItemEntry* pSelectedEntry = GetSelectItem();
    m_pComponents[eTPC_SelectOkBtn]->setEnabled(pSelectedEntry != NULL);
    return true;
}

bool CTitlePage::OnClickBackBtn( const CEGUI::EventArgs& /*e*/ )
{
    SWITCH_FSM_STATE(true, CTitlePage, Welcome);
    return true;
}

bool CTitlePage::OnClickStartBtn( const CEGUI::EventArgs& /*e*/ )
{
    CGame::GetInstance()->SetTask(eTT_PlayTrack);
    return true;
}

bool CTitlePage::OnClickSelectAudioBtn( const CEGUI::EventArgs& /*e*/ )
{
    ///Force show cursor
    CUIManager::GetInstance()->ShowWinCursor(true);
    m_pOwner->AcquireAudioFilePath();
    CUIManager::GetInstance()->ShowWinCursor(false);
    return true;
}

bool CTitlePage::OnClickSelectBRFBtn( const CEGUI::EventArgs& /*e*/ )
{
    ///Force show cursor
    CUIManager::GetInstance()->ShowWinCursor(true);
    m_pOwner->AcquireBeatsRecordPath();
    CUIManager::GetInstance()->ShowWinCursor(false);
    return true;
}

void CTitlePage::SetOwner( CTitleTask* pOwner )
{
    m_pOwner = pOwner;
}

void CTitlePage::SetAudioPathText( const TString& path )
{
    m_pComponents[eTPC_AudioFilePathTxt]->setFont("STZHONGS");
    char szBuffer[1024];
    CStringHelper::GetInstance()->ConvertToCHAR(path.c_str(), szBuffer, 1024);
    m_pComponents[eTPC_AudioFilePathTxt]->setText(szBuffer);
    if (m_pComponents[eTPC_BeatsRecordTxt]->getText() != "")
    {
        m_pComponents[eTPC_StartBtn]->setEnabled(true);
    }
}

void CTitlePage::SetBRFPathText( const TString& path )
{
    m_pComponents[eTPC_BeatsRecordTxt]->setFont("STZHONGS");
    char szBuffer[1024];
    CStringHelper::GetInstance()->ConvertToCHAR(path.c_str(), szBuffer, 1024);
    m_pComponents[eTPC_BeatsRecordTxt]->setText(szBuffer);
    if (m_pComponents[eTPC_AudioFilePathTxt]->getText() != "")
    {
        m_pComponents[eTPC_StartBtn]->setEnabled(true);
    }
}

void CTitlePage::ReverseFSM()
{
    REVERSE_FSM();
}

IMPL_FSM_BEGIN(CTitlePage, Welcome)
IMPL_FSM_ENTER
{
    m_pComponents[eTPC_CreateBtn]->setVisible(true);
    m_pComponents[eTPC_PlayBtn]->setVisible(true);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*CGame::GetInstance(), &CGame::Exit), true);
}
IMPL_FSM_EXIT
{
    m_pComponents[eTPC_CreateBtn]->setVisible(false);
    m_pComponents[eTPC_PlayBtn]->setVisible(false);
    CInputManager::GetInstance()->UnRegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*CGame::GetInstance(), &CGame::Exit));
}
IMPL_FSM_END

IMPL_FSM_BEGIN(CTitlePage, CreateTrack)
IMPL_FSM_ENTER
{
    m_pComponents[eTPC_SelectBackBtn]->setVisible(true);
    m_pComponents[eTPC_SelectOkBtn]->setVisible(true);
    m_pComponents[eTPC_TrackList]->setVisible(true);
    m_pComponents[eTPC_BrowseBtn]->setVisible(true);
    m_pComponents[eTPC_SelectOkBtn]->setEnabled(false);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CTitlePage::ReverseFSM), true);
}
IMPL_FSM_EXIT
{
    m_pComponents[eTPC_SelectBackBtn]->setVisible(false);
    m_pComponents[eTPC_SelectOkBtn]->setVisible(false);
    m_pComponents[eTPC_TrackList]->setVisible(false);
    m_pComponents[eTPC_BrowseBtn]->setVisible(false);
    CInputManager::GetInstance()->UnRegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CTitlePage::ReverseFSM));
}
IMPL_FSM_END

IMPL_FSM_BEGIN(CTitlePage, PlayTrack)
IMPL_FSM_ENTER
{
    m_pComponents[eTPC_BeatsRecordTxt]->setText("");
    m_pComponents[eTPC_AudioFilePathTxt]->setText("");
    m_pComponents[eTPC_StartBtn]->setEnabled(false);

    for (int i = eTPC_SelectAudioFileBtn; i <= eTPC_BackBtn; ++i)
    {
        m_pComponents[i]->setVisible(true);
    }
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CTitlePage::ReverseFSM), true);
}
IMPL_FSM_EXIT
{
    for (int i = eTPC_SelectAudioFileBtn; i <= eTPC_BackBtn; ++i)
    {
        m_pComponents[i]->setVisible(false);
    }
    CInputManager::GetInstance()->UnRegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CTitlePage::ReverseFSM));
}
IMPL_FSM_END