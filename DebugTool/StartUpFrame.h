#ifndef BEATS_STARTUPDIALOG_H__INCLUDE
#define BEATS_STARTUPDIALOG_H__INCLUDE

class CStartUpFrame : public wxFrame 
{
public:        
    CStartUpFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    ~CStartUpFrame();

    size_t GetFuncSwitcher();
private:
    void FillModuleList();

private:
    wxRadioBox* m_pFunctionRadioBox;

protected:
    wxStaticText* m_pWelcomeText;
    wxCheckBox* m_pEnableCheckBox;
    wxButton* m_pStartBtn;
    wxButton* m_pExitBtn;
    wxPanel* m_pMainPanel;
    wxPanel* m_pHardwareParamPanel;
    wxPanel* m_pPerformParamPanel;
    wxPanel* m_pMemoryParamnel;
    wxStaticText* m_pDllListTxt;
    wxListBox* m_pHookDllList;

    // Virtual event handlers, overide them in your derived class
    virtual void OnFunctionButtonSelected( wxCommandEvent& event );
    virtual void OnFunctionEnabled( wxCommandEvent& event );
    virtual void OnLaunchBtnClick( wxCommandEvent& event );
    virtual void OnExitBtnClick( wxCommandEvent& event );
    size_t m_funcSwitcher;
};

#endif
