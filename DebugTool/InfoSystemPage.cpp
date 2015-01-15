#include "stdafx.h"
#include "BDTWxApp.h"
#include "BDTWxFrame.h"
#include "InfoSystem/InfoPagePanel.h"

static const TCHAR* DEFAULT_PAGE_NAME = _T("Default");

void CBDTWxFrame::CreateInfoSystemPage()
{
    m_pGameInfoPanel = new wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bInfoSizer;
    bInfoSizer = new wxBoxSizer( wxVERTICAL );
    m_pInfoBoardNoteBook = new wxAuiNotebook( m_pGameInfoPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_CLOSE_ON_ACTIVE_TAB );
    bInfoSizer->Add( m_pInfoBoardNoteBook, 1, wxEXPAND | wxALL, 5 );
    
    m_pGameInfoPanel->SetSizer( bInfoSizer );
    m_pGameInfoPanel->Layout();
    bInfoSizer->Fit( m_pGameInfoPanel );

    m_pNoteBook->AddPage( m_pGameInfoPanel, wxT("сно╥пео╒"), true );
    m_pInfoBoardNoteBook->Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( CBDTWxFrame::OnInfoNoteBookChanged ), NULL, this );
    m_pageCreateWaitEvent = CreateEvent(NULL, false, false, NULL);
    m_createInfoPageEventId = wxNewId();
    Connect(m_createInfoPageEventId, wxEVT_THREAD, wxThreadEventHandler(CBDTWxFrame::OnCreateInfoPanel));

    AddPage(DEFAULT_PAGE_NAME);
}

void CBDTWxFrame::DestroyInfoSystemPage()
{
    Disconnect(m_createInfoPageEventId, wxEVT_THREAD, wxThreadEventHandler(CBDTWxFrame::OnCreateInfoPanel));
}

CInfoPagePanel* CBDTWxFrame::GetInfoPage(const TCHAR* pPageName, bool bAutoAdd)
{
    CInfoPagePanel* pInfoPage = NULL;
    if (pPageName == NULL)
    {
        pPageName = DEFAULT_PAGE_NAME;
    }
    bool bFind = false;
    for (uint32_t i = 0; i < m_pInfoBoardNoteBook->GetPageCount(); ++i)
    {
        if (m_pInfoBoardNoteBook->GetPageText(i).CmpNoCase(pPageName) == 0)
        {
            pInfoPage = dynamic_cast<CInfoPagePanel*>(m_pInfoBoardNoteBook->GetPage(i));
            bFind = true;
            break;
        }
    }
    if (!bFind && bAutoAdd)
    {
        wxThreadEvent *event = new wxThreadEvent(wxEVT_THREAD, m_createInfoPageEventId);
        event->SetInt((int)(&pInfoPage));
        event->SetString(pPageName);
        wxQueueEvent(this, event);
        WaitForSingleObject(m_pageCreateWaitEvent, INFINITE);
        // Get the last page which is just new created by wxEVT_THREAD.
        pInfoPage = dynamic_cast<CInfoPagePanel*>(m_pInfoBoardNoteBook->GetPage(m_pInfoBoardNoteBook->GetPageCount() - 1));
    }
    return pInfoPage;
}

wxPanel* CBDTWxFrame::AddPage( const TCHAR* pPageName )
{
    CInfoPagePanel* pDefaultInfoPanel = new CInfoPagePanel( m_pInfoBoardNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_pInfoBoardNoteBook->AddPage( pDefaultInfoPanel, pPageName, false );
    BEATS_ASSERT(pDefaultInfoPanel != NULL);
    return pDefaultInfoPanel;
}

void CBDTWxFrame::OnInfoNoteBookChanged( wxBookCtrlEvent& /*event*/ )
{
}

void CBDTWxFrame::AddInfoLog( const TCHAR* pLog, const TCHAR* pCatalog /*= 0*/, uint32_t logPos /*= 0*/, uint32_t color /*= 0*/ )
{
    CInfoPagePanel* pPanel = GetInfoPage(pCatalog);
    wxColor textColor;
    textColor.SetRGBA(color);
    wxTextAttr textAttr(textColor);
    pPanel->AppendLogText(pLog, logPos, &textAttr);
}

void CBDTWxFrame::SetPropertyInfo(const TCHAR* pCatalog, const TCHAR* pPropertyName, uint32_t propertyId, void* value, EReflectPropertyType type )
{
    CInfoPagePanel* pPanel = GetInfoPage(pCatalog);
    pPanel->UpdateProperty(pPropertyName, propertyId, value, type);
}

void CBDTWxFrame::DeleteProperty(const TCHAR* pCatalog, const TCHAR* pPropertyName)
{
    CInfoPagePanel* pPanel = GetInfoPage(pCatalog, false);
    if (pPanel)
    {
        pPanel->DeleteProperty(pPropertyName);
    }
}

void CBDTWxFrame::OnCreateInfoPanel( wxThreadEvent& event )
{
    CInfoPagePanel* panel = *(CInfoPagePanel**)(event.GetInt());
    panel = static_cast<CInfoPagePanel*>(AddPage(event.GetString()));
    SetEvent(m_pageCreateWaitEvent);
}
