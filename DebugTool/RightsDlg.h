#ifndef BEATS_RIGHTSDLG_H__INCLUDE
#define BEATS_RIGHTSDLG_H__INCLUDE

class CRightsDialog : public wxDialog 
{
public:

    CRightsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 683,339 ), long style = wxDEFAULT_DIALOG_STYLE ); 
    ~CRightsDialog();
private:
    void SetLogoImage();

private:

    wxStaticText* m_pAboutLabel;
    wxStaticBitmap* m_pLogoBitmap;
};

#endif
