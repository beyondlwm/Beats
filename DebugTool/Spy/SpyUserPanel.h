#ifndef BEATS_DEBUGTOOL_SPY_SPYUSERPANEL_H__INCLUDE
#define BEATS_DEBUGTOOL_SPY_SPYUSERPANEL_H__INCLUDE

struct SBeatsSocket;
struct SDiskInfo;
struct SDirectory;
class CVirtualFileCtrl;
class CSpyCommandBase;
class wxListEvent;

static const wxString Command_Type_String[] = { wxT("ÏûÏ¢µ¯´°") };

enum ESpyCommandType
{
    eSCT_SendMsg,

    eSCT_Count,
    eSCT_Force32Bit = 0xFFFFFFFF
};

class CSpyUserPanel : public wxPanel
{
    typedef wxPanel super;
    enum ESpyUserPanelUIID
    {
        eSUPUIID_DownloadMenu,
        eSUPUIID_UploadMenu,

        eSUPUIID_Count,
        eSUPUIID_Force32Bit = 0xFFFFFFFF
    };
public:
    CSpyUserPanel(const SBeatsSocket* pSockInfo,
                    HMODULE hSpyDLLHandle,
                    wxWindow *parent,
                    wxWindowID winid = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                    const wxString& name = wxPanelNameStr);


    virtual ~CSpyUserPanel();

private:
    void SendCommand(const char* pszCommand);
    void OnSpyConsoleCommandEnter(wxCommandEvent& event);
    void OnSpyConsoleCommandKeypress(wxKeyEvent& event);
    void OnSpyCommandExecuteBtnClick(wxCommandEvent& event);
    void OnSpyFileItemActivated(wxListEvent& event);
    void OnSpyRefreshFileBtnClick(wxCommandEvent& event);
    void OnSpyRightClickFileCtrl(wxContextMenuEvent& event);
    void OnSpySelectFileCtrlMenu(wxMenuEvent& event);

    void AddFeedback(const char* pszFeedback);
    void GetFileInfo(const TCHAR* pszFilePath, bool bRefresh);

    // We cache the directory data to reuse.
    // When there is a directory data from network, it must be a "fresh" value, we will call UpdateDirectoryCache to update the cache.
    // When we requires a directory data, we will first try to find the cache, if we got, no need to request it but use the cache instead.
    // We also create "place holder" directories of which the parent is NULL and data is emtpy, to build the path to the desired directory.
    void UpdateDirectoryCache(const SDirectory* pDirectory);
    SDirectory* GetCache(const TString& strPath) const;

    void UpdateFileUI(char type, void* pData);

    static void OnSystemCommandFeedback(const char* pszFeedback, void* pUserData);
    static void OnGetFileInfo(char type, void* pFileInfo, void* pUserData);
private:
    const SBeatsSocket* m_pSocketInfo;
    SDiskInfo* m_pDiskInfoCache;
    SDirectory* m_pRootDirectoryCache;
    wxStaticText* m_pPeerAddrLabel;
    wxStaticText* m_pPeerNameLabel;
    wxStaticText* m_pConnectTimeInfoLabel;
    wxStaticText* m_pCurrentStateLabel;
    wxStaticText* m_pUploadSpeedLabel;
    wxStaticText* m_pDownloadSpeedLabel;
    wxStaticText* m_pUploadDataCountLabel;
    wxStaticText* m_pDownloadDataCountLabel;
    wxTextCtrl* m_pConsoleTextCtrl;
    wxTextCtrl* m_pConsoleInputTextCtrl;
    wxTextCtrl* m_pFileLocationText;
    wxButton* m_pExecuteCommandButton;
    wxButton* m_pRefreshFileInfoButton;
    wxChoice* m_pCommandChoice;
    CVirtualFileCtrl* m_pRemoteFileCtrl;
    HMODULE m_hSpyDllHandle;
    size_t m_uCurSelectHistoryIndex;
    std::vector<std::string> m_historyCommand;
};

#endif