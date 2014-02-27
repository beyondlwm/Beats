#include "stdafx.h"
#include "RecordTrackPage.h"
#include "cegui.h"
#include "../Game.h"
#include "../Tasks/RecordTrack.h"
#include "../../ui/UIManager.h"

CRecordTrackPage::CRecordTrackPage()
: super(eUIPN_RecordTrack)
, m_scrollUpdateSwitch(true)
, m_pOwner(NULL)
{

}

CRecordTrackPage::~CRecordTrackPage()
{

}

void CRecordTrackPage::Init()
{
}

bool CRecordTrackPage::OpenImpl()
{
    BEATS_ASSERT(m_pOwner != NULL, _T("Can't Open Record Track Page without setting owner!"));
    SetAllComponentsVisible(true);
    INIT_FSM(CRecordTrackPage, Recording);
    return true;
}

bool CRecordTrackPage::CloseImpl()
{
    SetAllComponentsVisible(false);
    EXIT_FSM();
    return true;
}

void CRecordTrackPage::Update()
{
    UPDATE_FSM();
}

void CRecordTrackPage::InitUIEvent()
{
    m_pRootWindow->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CRecordTrackPage::OnPressKeyBoard, this));
    m_pComponents[eRTPC_PlayBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CRecordTrackPage::OnClickPlayBtn, this));
    m_pComponents[eRTPC_PauseBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CRecordTrackPage::OnClickPauseBtn, this));
    m_pComponents[eRTPC_StopBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CRecordTrackPage::OnClickStopBtn, this));
    m_pComponents[eRTPC_ExitBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CRecordTrackPage::OnClickExitBtn, this));
    m_pComponents[eRTPC_AddBeatBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CRecordTrackPage::OnClickAddBeatBtn, this));
    m_pComponents[eRTPC_PlayScroll]->subscribeEvent(CEGUI::Scrollbar::EventThumbTrackEnded, CEGUI::Event::Subscriber(&CRecordTrackPage::OnDragScrollBarEnd, this));
    m_pComponents[eRTPC_PlayScroll]->subscribeEvent(CEGUI::Scrollbar::EventThumbTrackStarted, CEGUI::Event::Subscriber(&CRecordTrackPage::OnDragScrollBarBegin, this));
    m_pComponents[eRTPC_BeatsList]->subscribeEvent(CEGUI::Listbox::EventMouseClick, CEGUI::Event::Subscriber(&CRecordTrackPage::OnListClicked, this));
}

void CRecordTrackPage::UpdatePlayScroll( float progress )
{
    if (m_scrollUpdateSwitch)
    {
        CEGUI::Scrollbar* pScrollBar = static_cast<CEGUI::Scrollbar*>(m_pComponents[eRTPC_PlayScroll]);
        pScrollBar->setScrollPosition(progress);
    }
}

bool CRecordTrackPage::OnClickPlayBtn( const CEGUI::EventArgs& /*e*/ )
{
    m_pOwner->Play();
    return true;
}

bool CRecordTrackPage::OnClickPauseBtn( const CEGUI::EventArgs& /*e*/ )
{
    m_pOwner->Pause();
    return true;
}

bool CRecordTrackPage::OnClickExitBtn( const CEGUI::EventArgs& /*e*/ )
{
    m_pOwner->Exit();
    return true;
}

bool CRecordTrackPage::OnClickAddBeatBtn( const CEGUI::EventArgs& /*e*/ )
{
    m_pOwner->AddBeats(eKC_SPACE);
    return true;
}

bool CRecordTrackPage::OnDragScrollBarEnd( const CEGUI::EventArgs& /*e*/ )
{
    CEGUI::Scrollbar* pScrollBar = static_cast<CEGUI::Scrollbar*>(m_pComponents[eRTPC_PlayScroll]);
    float progress = pScrollBar->getScrollPosition();
    m_pOwner->SetPlayTimeByProgress(progress);
    m_scrollUpdateSwitch = true;
    return true;
}

bool CRecordTrackPage::OnDragScrollBarBegin( const CEGUI::EventArgs& /*e*/ )
{
    m_scrollUpdateSwitch = false;
    return true;
}

bool CRecordTrackPage::OnClickStopBtn( const CEGUI::EventArgs& /*e*/ )
{
    m_pOwner->Stop();
    return true;
}

bool CRecordTrackPage::OnListClicked( const CEGUI::EventArgs& /*e*/ )
{
    CEGUI::Listbox* pListBox = static_cast<CEGUI::Listbox*>(m_pComponents[eRTPC_BeatsList]);
    CEGUI::ListboxItem* pSelectItem = pListBox->getFirstSelectedItem();
    if (pSelectItem)
    {
        m_pOwner->Pause();
        m_pOwner->SetPlayTimeByPos((unsigned long)((long long)(pSelectItem->getUserData())));
    }
    return true;
}

bool CRecordTrackPage::OnPressKeyBoard( const CEGUI::EventArgs& e )
{
    const CEGUI::KeyEventArgs* pEventArg = static_cast<const CEGUI::KeyEventArgs*>(&e);
    EKeyCode keyCode = (EKeyCode)(pEventArg->scancode);
    if (keyCode == eKC_DELETE)
    {
        CEGUI::Listbox* pListBox = static_cast<CEGUI::Listbox*>(m_pComponents[eRTPC_BeatsList]);
        CEGUI::ListboxItem* pSelectItem = pListBox->getFirstSelectedItem();
        if (pSelectItem)
        {
            m_pOwner->RemoveBeats((unsigned long)((long long)(pSelectItem->getUserData())));
        }
    }
    else if (keyCode != eKC_ESCAPE)
    {
        m_pOwner->AddBeats((WORD)(keyCode));
    }
    return true;
}

void CRecordTrackPage::SetOwner( CRecordTrack* pOwner )
{
    m_pOwner = pOwner;
}

void CRecordTrackPage::AddDataToList(unsigned long playPos, EKeyCode code)
{
    CEGUI::Listbox* pListBox = static_cast<CEGUI::Listbox*>(m_pComponents[eRTPC_BeatsList]);
    TCHAR playPosBuff[64];
    _stprintf_s(playPosBuff, _T("%d  %s"), playPos, KeyCodeLabel[code]);

#ifdef _UNICODE
    char ceguiResult[64];
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)playPosBuff, -1,(char *)ceguiResult, 64, NULL, NULL);
    CEGUI::ListboxTextItem* pTextItem = new CEGUI::ListboxTextItem(ceguiResult);
#else
    CEGUI::ListboxTextItem* pTextItem = new CEGUI::ListboxTextItem(playPosBuff);
#endif

    pTextItem->setTextColours(CEGUI::colour(1,0,0));
    pTextItem->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
    pTextItem->setUserData((void*)((long long)playPos));//use long long to avoid compiler warning
    CEGUI::ListboxItem* prevItem = NULL;
    size_t index = CalculateItemIndex(playPos);

    if(index < pListBox->getItemCount())
    {
        prevItem = pListBox->getListboxItemFromIndex(index);
        pListBox->insertItem(pTextItem, prevItem);
    }
    else
    {
        pListBox->addItem(pTextItem);
    }

    pListBox->ensureItemIsVisible(pTextItem);
    pListBox->setItemSelectState(pTextItem, true);
}

void CRecordTrackPage::RemoveDataFromList( CEGUI::ListboxItem* pSelectItem )
{
    CEGUI::Listbox* pListBox = static_cast<CEGUI::Listbox*>(m_pComponents[eRTPC_BeatsList]);
    if (pListBox->getItemCount() > 1)
    {
        size_t itemIndex = pListBox->getItemIndex(pSelectItem);
        bool isLastItem = pListBox->getItemCount() == itemIndex + 1;
        pListBox->setItemSelectState(isLastItem ? itemIndex - 1 : itemIndex + 1, true);
    }
    if (pSelectItem != NULL)
    {
        pListBox->removeItem(pSelectItem);
    }
}

void CRecordTrackPage::RemoveDataFromList( unsigned long playPos )
{
    size_t index = CalculateItemIndex(playPos);
    CEGUI::Listbox* pListBox = static_cast<CEGUI::Listbox*>(m_pComponents[eRTPC_BeatsList]);
    RemoveDataFromList(pListBox->getListboxItemFromIndex(index));
}

size_t CRecordTrackPage::CalculateItemIndex( unsigned long pos )
{
    size_t result = 0;
    CEGUI::Listbox* pListBox = static_cast<CEGUI::Listbox*>(m_pComponents[eRTPC_BeatsList]);
    size_t itemCount = pListBox->getItemCount();
    if (itemCount > 0)
    {
        unsigned long minPos = (unsigned long)((long long)(pListBox->getListboxItemFromIndex(0)->getUserData()));
        unsigned long maxPos = (unsigned long)((long long)(pListBox->getListboxItemFromIndex(itemCount - 1)->getUserData()));
        if (pos > maxPos)
        {
            result = itemCount;
        }
        else if (pos == maxPos)
        {
            result = itemCount - 1;
        }
        else if (pos <= minPos)
        {
            result = 0;
        }
        else
        {
            //Binary chop
            CEGUI::ListboxItem* pItem = NULL;
            size_t lastLowerIndex = 0;
            size_t lastGreaterIndex = itemCount - 1;
            do
            {
                size_t i = (size_t)((lastLowerIndex + lastGreaterIndex)* 0.5f);
                pItem = pListBox->getListboxItemFromIndex(i);
                unsigned long itemPos = (unsigned long)((long long)(pItem->getUserData()));
                itemPos >= pos ? lastGreaterIndex = i : lastLowerIndex = i;
                result = lastGreaterIndex;
            }while( lastGreaterIndex - lastLowerIndex != 1 && lastGreaterIndex > lastLowerIndex);
        }
    }
    return result;
}

void CRecordTrackPage::UpdateData( unsigned long playPos, EKeyCode code )
{
    size_t index = CalculateItemIndex(playPos);
    CEGUI::Listbox* pListBox = static_cast<CEGUI::Listbox*>(m_pComponents[eRTPC_BeatsList]);
    CEGUI::ListboxItem* pItem = pListBox->getListboxItemFromIndex(index);
    TCHAR playPosBuff[64];
    _stprintf_s(playPosBuff, _T("%d  %s"), playPos, KeyCodeLabel[code]);

#ifdef _UNICODE
    char ceguiResult[64];
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)playPosBuff, -1,(char *)ceguiResult, 64, NULL, NULL);
    pItem->setText(ceguiResult);
#else
    pItem->setText(playPosBuff);
#endif

    pListBox->invalidate();
}

IMPL_FSM_BEGIN(CRecordTrackPage, Recording)
IMPL_FSM_ENTER
{
    m_pComponents[eRTPC_PlayBtn]->setEnabled(false);
    m_pComponents[eRTPC_AddBeatBtn]->setEnabled(true);
    m_pComponents[eRTPC_PauseBtn]->setEnabled(true);
    m_pComponents[eRTPC_StopBtn]->setEnabled(true);
}
IMPL_FSM_UPDATE
{
    UpdatePlayScroll(m_pOwner->GetProgress());
}
IMPL_FSM_EXIT
{
    m_pComponents[eRTPC_PlayBtn]->setEnabled(true);
    m_pComponents[eRTPC_AddBeatBtn]->setEnabled(false);
    m_pComponents[eRTPC_PauseBtn]->setEnabled(false);
    m_pComponents[eRTPC_StopBtn]->setEnabled(false);
}
IMPL_FSM_END

IMPL_FSM_BEGIN(CRecordTrackPage, Pause)
IMPL_FSM_ENTER
{
    m_pComponents[eRTPC_PauseBtn]->setEnabled(false);    
    m_pComponents[eRTPC_PlayBtn]->setEnabled(true);
}
IMPL_FSM_EXIT
{
    m_pComponents[eRTPC_PauseBtn]->setEnabled(true);
    m_pComponents[eRTPC_PlayBtn]->setEnabled(false);
}
IMPL_FSM_END

IMPL_FSM_BEGIN(CRecordTrackPage, Exit)
IMPL_FSM_ENTER
{
    CEGUI::Listbox* pListBox = static_cast<CEGUI::Listbox*>(m_pComponents[eRTPC_BeatsList]);
    if (pListBox->getItemCount() > 0)
    {
        ///Force show cursor
        CUIManager::GetInstance()->ShowWinCursor(true);
        int result = MessageBox(CGame::GetInstance()->GetHwnd(), _T("是否存储当前记录？"), _T("Save"), MB_YESNO);
        if (result == IDYES)
        {
            m_pOwner->SaveTrack();
        }
        CUIManager::GetInstance()->ShowWinCursor(false);
    }

    pListBox->resetList();
    CGame::GetInstance()->SetTask(eTT_Title);
}
IMPL_FSM_END

IMPL_FSM_BEGIN(CRecordTrackPage, Stop)
IMPL_FSM_ENTER
{
    m_pComponents[eRTPC_StopBtn]->setEnabled(false);
    m_pComponents[eRTPC_PauseBtn]->setEnabled(false);
    m_pComponents[eRTPC_AddBeatBtn]->setEnabled(false);
    m_pComponents[eRTPC_PlayBtn]->setEnabled(true);
    CEGUI::Scrollbar* pScrollBar = static_cast<CEGUI::Scrollbar*>(m_pComponents[eRTPC_PlayScroll]);
    pScrollBar->setScrollPosition(0);
}
IMPL_FSM_EXIT
{
    m_pComponents[eRTPC_StopBtn]->setEnabled(true);
    m_pComponents[eRTPC_PauseBtn]->setEnabled(true);
    m_pComponents[eRTPC_AddBeatBtn]->setEnabled(true);
    m_pComponents[eRTPC_PlayBtn]->setEnabled(false);
}
IMPL_FSM_END