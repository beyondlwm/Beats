#ifndef BEATS_DEBUGTOOL_SPY_VIRTUALFILECTRL_H__INCLUDE
#define BEATS_DEBUGTOOL_SPY_VIRTUALFILECTRL_H__INCLUDE

#include <wx/listctrl.h>

struct SDirectory;
struct SDiskInfo;

class CVirtualFileCtrl : public wxListCtrl
{
    typedef wxListCtrl super;
public:

    CVirtualFileCtrl();
    CVirtualFileCtrl(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxLC_ICON,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxListCtrlNameStr);

    virtual ~CVirtualFileCtrl();

public:
    void SetCurrentDirectory(const SDirectory* pCurDirectory);
    void SetDiskInfo(const SDiskInfo* pDiskInfo);
    void UpdateFiles();
    const TString& GetCurrentDirectoryPath() const;

private:
    int GetDriverIconByType(const char type) const;
    wxString GetBytesDisplayString(unsigned long long uSizeInBytes);

private:
    const SDirectory* m_pCurrentDirectory;
    const SDiskInfo* m_pDiskInfo;
    TString m_strCurDirectoryPath;
};

#endif