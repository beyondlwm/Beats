#include "wx/combo.h"

#define SYMBOL_MyFrame_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_MyFrame_TITLE _("CodeReview")
#define SYMBOL_MyFrame_SIZE wxSize(350, 300)
#define SYMBOL_MyFrame_POSITION wxDefaultPosition


class MainFrame : public wxFrame
{
public:
    MainFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = SYMBOL_MyFrame_TITLE, const wxPoint& pos = SYMBOL_MyFrame_POSITION, const wxSize& size = SYMBOL_MyFrame_SIZE, long style = SYMBOL_MyFrame_STYLE);
    ~MainFrame();

    void    Init();
    void    CreateControls();

    void    OnQuit(wxCommandEvent& event);
    void    OnAbout(wxCommandEvent& event);
    void    OnButtonClick(wxCommandEvent& event);
    wxString FormatFunc(const wxString& str);

private:
    wxArrayString   m_arrItems;
    wxButton*       m_pButton;
    wxChoice*       m_pChoice;
    wxTextCtrl*     m_pTextCtrl;
    DECLARE_EVENT_TABLE();
};

