#include "stdafx.h"
#include "codereviewer.h"
#include "wx/clipbrd.h"
#include <wx/dir.h>
#include <wx/ffile.h>

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT,  MainFrame::OnQuit)
END_EVENT_TABLE()

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_pButton(NULL)
    , m_pChoice(NULL)
    , m_pTextCtrl(NULL)
{
    wxFrame::Create( parent, id, caption, pos, size, style );
    Init();
    CreateControls();
}

MainFrame::~MainFrame()
{

}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MainFrame::Init()
{
    static const int size = 1024;
    TCHAR szBuffer[size] = {0};
    FILE* file = _tfopen(_T("name.txt"),_T("rt"));
    if (file)
    {
        setlocale(0, "chs");
        while(!feof(file))
        {
            memset(szBuffer, 0, size);
            _fgetts(szBuffer, size, file);
            int len = wcslen(szBuffer);
            if (len > 1)
            {
                m_arrItems.Add( szBuffer );
            }
        }
        fclose(file);
    }
}

void MainFrame::CreateControls()
{
    wxBoxSizer* pSizerBase = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(pSizerBase);

    wxPanel* pPanelBase = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* pSizerLayer = new wxBoxSizer(wxVERTICAL);

    pSizerBase->Add(pPanelBase, 1, wxGROW, 5);
    pPanelBase->SetSizer(pSizerLayer);

    wxBoxSizer* pSizerUp = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pSizerDown = new wxBoxSizer(wxHORIZONTAL);

    pSizerLayer->Add(pSizerDown, 1, wxGROW|wxALL, 5);
    pSizerLayer->Add(pSizerUp, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    m_pChoice = new wxChoice(pPanelBase, wxID_ANY, wxDefaultPosition, wxSize(0, -1), m_arrItems);
    m_pChoice->SetSelection(0);
    
    m_pButton = new wxButton(pPanelBase, wxID_ANY, "Ìá½»", wxDefaultPosition, wxSize(0, -1), 0);
    m_pButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnButtonClick), NULL, this);

    pSizerUp->Add(m_pChoice, 1, wxGROW|wxALL, 5);
    pSizerUp->Add(5, 5, 2, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    pSizerUp->Add(m_pButton, 1, wxGROW|wxALL, 5);
    
    m_pTextCtrl = new wxTextCtrl(pPanelBase, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_AUTO_URL);
    pSizerDown->Add(m_pTextCtrl, 1, wxGROW|wxALL, 5);
}

void MainFrame::OnButtonClick(wxCommandEvent& /*event*/)
{
    wxString tmp = m_pTextCtrl->GetValue();
    wxString textout;
    if (tmp.Len() != 0)
    {
        tmp.Last()!=_('\n') ? tmp+=_("\n") : tmp;
        textout = FormatFunc(tmp);
        textout += m_pChoice->GetString(m_pChoice->GetSelection());
        m_pTextCtrl->SetLabelText(textout);
        if (wxTheClipboard->Open())
        {
            wxTextDataObject* data = new wxTextDataObject;
            data->SetText(textout);
            wxTheClipboard->SetData( data );
            wxTheClipboard->Close();
        }
    }
}

wxString MainFrame::FormatFunc(const wxString& str)
{
    int num = 0;
    wxString tmp = str;
    wxString textout;
    while(1)
    {
        int i = tmp.Find(_("\n"));
        if (i == -1)
        {
            break;
        }
        num++;
        textout += tmp.Format("%d. -%s",num,tmp.Left(i + 1).c_str());
        tmp = tmp.Mid(i + 1);
    }
    textout += "\nreviewer:";
    return textout;
}