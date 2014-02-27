#ifndef BEATS_BEATS_DEBUG_TOOL_BDTWXFRAME_H__INCLUDE
#define BEATS_BEATS_DEBUG_TOOL_BDTWXFRAME_H__INCLUDE

#include <wx/menu.h>
#include <wx/treectrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/notebook.h>
#include <wx/aui/auibook.h>
#include <wx/frame.h>
#include <wx/grid.h>
#include <wx/statbmp.h>
#include <wx/srchctrl.h>
#include <wx/splitter.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <dbghelp.h>
#include "PerformDetector/PerformDetectorDef.h"
#include "BeatsDebugTool.h"

class CRightsDialog;
class CComponentProjectDirectory;

enum EMemoryViewType
{
    eMVT_Location,
    eMVT_Size,
    eMVT_AllocTime,
    eMVT_Addr,

    eMVT_Count,
    eMVT_ForceTo32Bit = 0xffffffff
};

static const TCHAR* MemoryViewTypeStr[] = 
{
    _T("Location"),
    _T("Size"),
    _T("Usage"),
    _T("Address")
};

class CInfoPagePanel;
class CWxwidgetsPropertyBase;
class CComponentRenderWindow;
class CComponentEditorProxy;
class CReflectBase;
class CPropertyDescriptionBase;
class TiXmlElement;

class CBDTWxFrame : public wxFrame
{
    enum ETreeCtrlIconType
    {
        eTCIT_File,
        eTCIT_FileSelected,
        eTCIT_Folder,
        eTCIT_FolderSelected,
        eTCIT_FolderOpened,
        eTCIT_Count,
        eTCIT_Force32Bit = 0xFFFFFFFF
    };

public:
    CBDTWxFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    virtual ~CBDTWxFrame();

    void Init(size_t funcSwitcher = 0xffffffff);
    void UnInit();
    bool IsInit();

    //performance
    void UpdatePerformData();
    void RefreshPerformBoard(const SPerformanceRecord* pRecord );
    void RefreshInfo(const SPerformanceRecord* pRecord);
    void RefreshItemGridRowLab(const SPerformanceRecord* pRecord);
    void RefreshItemGridCell(const SPerformanceRecord* pRecord);

    void UpdatePerformGraphicData(const SPerformanceResult* pResult);
    void ClearGrid();
    void ClearGridRow(size_t startRowIndex, size_t count);
    void ClearPeakValue(SPerformanceRecord* pRecord);

    //memory
    void UpdateMemoryData();
    size_t GetMemoryViewType();
    bool FilterTest(size_t eip, IMAGEHLP_LINE& info, bool& getAddrSuccess );
    bool IsAutoUpdateMemory();
    bool IsMemoryUIChanged();
    bool IsMemoryRecordChanged(size_t recordCount, size_t latestRecordTime);
    void Exit(bool bRemind = false);
    int GetMemoryGridViewStartPos();
    int GetMemoryGridViewStartOffset();
    int GetMemoryGridItemCountLimit();
    bool IsMemoryGridNeedUpdateByScroll();

    //Component
    void InsertInPropertyGrid(const std::vector<CPropertyDescriptionBase*>& pProperties, wxPGProperty* pParent = NULL);
    void InsertComponentsInPropertyGrid(CComponentEditorProxy* pComponent, wxPGProperty* pParent = NULL);
    // To create this impl for care about both changing property on UI by event and in code.
    void OnComponentPropertyChangedImpl(wxPGProperty* pProperty);
    void UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pProperty);
    wxPGProperty* GetPGPropertyByBase(CPropertyDescriptionBase* pBase);
    void SelecteComponent(CComponentEditorProxy* pComponentInstance);
    CComponentEditorProxy* GetSelectedComponent();
    wxPGEditor* GetPtrEditor();
    void RequestToUpdatePropertyGrid();

    void ActivateFile(const TCHAR* pszFileName);

    //Info
    void AddInfoLog(const TCHAR* pLog, const TCHAR* pCatalog = 0, size_t logPos = 0, size_t color = 0);
    CInfoPagePanel* GetInfoPage(const TCHAR* pPageName, bool bAutoAdd = true);
    void SetPropertyInfo(const TCHAR* pCatalog, const TCHAR* pPropertyName, size_t propertyId, void* value, EPropertyType type);
    void DeleteProperty(const TCHAR* pCatalog, const TCHAR* pPropertyName);
    void RegisterPropertyChangedHandler(TPropertyChangedHandler pFunc);
    TPropertyChangedHandler GetPropertyChangedHandler();

private:

    //create pages
    void CreateSystemInfoPage();
    void DestroySystemInfoPage();

    void CreatePerformancePage();
    void DestroyPerformancePage();

    void CreateMemoryPage();
    void DestroyMemoryPage();

    void CreateComponentPage();
    void DestroyComponentPage();

    void CreateInfoSystemPage();
    void DestroyInfoSystemPage();

    void CreateSpyPage();
    void DestroySpyPage();

    wxPropertyGridManager* CreatePropertyGrid(wxWindow* pParent);
    void SetGridRowsCount(wxGrid* gird, size_t count);

    virtual void OnCloseWindow(wxCloseEvent& event);
    virtual void OnClickAbout( wxCommandEvent& event );
    virtual void OnClickExit( wxCommandEvent& event );
    void OnNoteBookChanged( wxBookCtrlEvent& event );

    //performance
    void AddPerformNode(const SPerformanceRecord* pRecordToAdd);
    SPerformanceRecord* GetRecord(const wxTreeItemId& treeItemId);
    void OnTreeItemChanged( wxTreeEvent& event );
    void OnClickClearPeakValueBtn( wxCommandEvent& event );
    void OnClickPerformancePauseBtn(wxCommandEvent& event);
    void OnClickPerformanceClearBtn(wxCommandEvent& event);

    //memory
    void OnSamplingFrenquecnyChanged(wxScrollEvent& event);
    void OnLineMaxValueChanged(wxScrollEvent& event);
    void OnLineStepChanged(wxScrollEvent& event);
    void OnMemoryChoiceChanged( wxCommandEvent& event );
    void OnGridCellSelected( wxGridEvent& event );
    void OnSetMemoryStackOffset(wxSpinEvent& event);
    void OnCallStackListDoubleClicked(wxCommandEvent& event);
    
    void OnViewMemoryByAddr();
    void OnViewMemoryByLocation();
    void OnViewMemoryBySize();
    void OnViewMemoryByAllocTime();
    void RefreshViewMemoryByAddr();
    void RefreshViewMemoryByLocation();
    void RefreshViewMemoryBySize();
    void RefreshViewMemoryByTime();
    void ClearAllColumnLabel();
    void RefreshMemoryGrid(size_t type);
    void AdjustMemoryGridColumnCount(size_t count);

    //overview
    void OnModuleLoadTypeChanged(wxCommandEvent& event );
    void OnModuleSelectChanged(wxCommandEvent& event);
    void OnFullPathSelected( wxCommandEvent& event );
    void OnFilterTextEnter(wxCommandEvent& event);
    void OnFilterTextSelected(wxCommandEvent& event);
    void InitStaticLinkModuList();
    void InitSymbolList();
    void InitDynamicLinkModuList();
    void FillModuleList(bool dynamicList, bool fullpath);
    void GetSymbol(IMAGE_THUNK_DATA* pITDdata, HMODULE Module, HMODULE dllModule);

    //Components
    void InitComponentsPage();
    void ResetPropertyValue(wxPGProperty* property);
    void UpdatePropertyGrid();
    void OpenProjectFile(const TCHAR* pPath);
    void InitializeComponentTree(CComponentProjectDirectory* pProjectData, const wxTreeItemId& id);
    void CloseProjectFile();
    void DeleteItemInComponentFileList(wxTreeItemId itemId, bool bDeletePhysicalFile);
    void OpenComponentFile(const TCHAR* pFilePath);
    void CloseComponentFile(bool bRemindSave = true);
    void ResolveIdConflict(const std::map<size_t, std::vector<size_t>>& conflictIdMap);

    void OnComponentStartDrag( wxTreeEvent& event );
    void OnComponentEndDrag( wxTreeEvent& event );
    void OnComponentFileStartDrag( wxTreeEvent& event );
    void OnComponentFileEndDrag( wxTreeEvent& event );
    void OnTemplateComponentItemChanged(wxTreeEvent& event);
    void OnRightClickComponentFileList(wxTreeEvent& event);
    void OnActivateComponentFile(wxTreeEvent& event);
    void OnExpandComponentFile(wxTreeEvent& event);    
    void OnCollapseComponentFile(wxTreeEvent& event);    
    void OnComponentSearchTextUpdate( wxCommandEvent& event );
    void OnComponentFileSearchTextUpdate( wxCommandEvent& event );
    void OnComponentSearchIdle( wxCommandEvent& event );
    void OnComponentFileSearchIdle( wxCommandEvent& event );

    void OnComponentPropertyChanged(wxPropertyGridEvent& event);
    void OnComponentPropertySelected(wxPropertyGridEvent& event);
    void OnComponentPropertyRightClick(wxPropertyGridEvent& event);
    void OnComponentPropertyGridIdle(wxIdleEvent& event);
    void OnComponentPropertyMenuClicked(wxMenuEvent& event);
    void OnComponentFileListMenuClicked(wxMenuEvent& event);
    void OnComponentSaveButtonClick(wxCommandEvent& event);
    void OnOperateFileButtonClick(wxCommandEvent& event);
    void OnExportButtonClick(wxCommandEvent& event);

    //Info
    wxPanel* AddPage(const TCHAR* pPageName);
    void OnInfoNoteBookChanged(wxBookCtrlEvent& event);
    void OnCreateInfoPanel(wxThreadEvent& event);

    //Spy
    void OnSpyConnectButtonClick(wxCommandEvent& event);

private:
    wxMenuBar* m_pMenuBar;
    wxMenu* m_pFileMenu;
    wxMenu* m_pHelpMenu;
    wxBoxSizer* m_pMainBoxSizer;
    wxAuiNotebook* m_pNoteBook;
    wxPanel* m_pMainPanel;
    wxPanel* m_pOverviewPanel;
    wxPanel* m_pPerformancePanel;
    wxPanel* m_pMemoryPanel;
    wxPanel* m_pGameInfoPanel;
    wxPanel* m_pComponentPanel;
    wxPanel* m_pCrashRptPanel;
    wxPanel* m_pNetworkInfoPanel;
    wxPanel* m_pSpyPanel;
    wxGrid* m_pSubItemInfoGrid;

    //overview
    wxStaticText* m_pComputerModelLab;
    wxStaticText* m_pOperatingSystemLab;
    wxStaticText* m_pOperatingSystemVersionLab;

    wxStaticText* m_pProcessorLab;
    wxStaticText* m_pBaseBoardLab;
    wxStaticText* m_pPhysMemoryLab;
    wxStaticText* m_pMainHardDiskLab;
    wxStaticText* m_pDisplayCardLab;
    wxStaticText* m_pMonitorLab;
    wxStaticText* m_pSoundCardLab;
    wxListBox* m_pHotFixListBox;
    wxStaticText* m_pProcessNameLab;
    wxStaticText* m_pProcessSpaceLab;
    wxStaticText* m_pModuleLoadTypeLab;
    wxStaticText* m_pModuleSelectLab;
    wxStaticText* m_pModuleSymbolLab;
    wxChoice* m_pModuleLoadTypeChoice;
    wxListBox* m_pModuleList;
    wxListBox* m_pModuleSymbolList;
    wxCheckBox* m_pModuleShowFullPath;

    //performance:
    wxTreeCtrl* m_pPerformanceTreeCtrl;
    wxStaticText* m_pCurResultLabel;
    wxStaticText* m_pCurResultTxtLabel;
    wxStaticText* m_pAverageValueLab;
    wxStaticText* m_pAverageValueTxtLab;
    wxStaticText* m_pPeakValueLab;
    wxStaticText* m_pPeakValueTxtLab;
    wxStaticText* m_pSampleFrequencyLabel;
    wxButton* m_pClearPeakValueBtn;
    wxButton* m_pPausePerformanceBtn;
    wxButton* m_pClearPerformanceBtn;

    wxStaticText* m_totalRunTimeLab;
    wxStaticText* m_performLocationLab;
    wxStaticText* m_pTotalRunTimeTxtLab;
    wxStaticText* m_pFrameCountLab;
    wxStaticText* m_pFrameCountTxtLab;
    wxStaticText* m_pLineMaxValueLabel;
    wxStaticText* m_pLineStepLabel;

    wxStaticBitmap* m_pPerformanceGraphics;
    wxSlider* m_pSamplingFreqSlider;
    wxSlider* m_pLineMaxValueSlider;
    wxSlider* m_pLineStepSlider;

    //memory
    wxChoice* m_pMemoryViewChoice;
    wxGrid* m_pMemoryDataGrid;
    wxStaticText* m_pTotalAllocSizeLabel;
    wxStaticText* m_pAllocCountLabel;
    wxStaticText* m_pMaxAllocSizeLabel;
    wxStaticText* m_pFilterLabel;
    wxTextCtrl*    m_pStackOffsetTextCtrl;
    wxSpinButton* m_pStackOffsetSpinBtn;
    wxComboBox* m_pFilterTextComboBox;
    wxCheckBox* m_pAutoUpdateMemoryCheckBox;
    wxListBox* m_pCallStackListBox;

    //InfoBoard
    wxAuiNotebook* m_pInfoBoardNoteBook;
    HANDLE m_pageCreateWaitEvent;// Notice: we can only create wxWindow by event way when wxwidgets are running in dll mode.
    int m_createInfoPageEventId;
    TPropertyChangedHandler m_pFunc;

    CRightsDialog* m_pRightsDialog;

    size_t m_samplingFrequency;
    size_t m_curSamplingCount;
    size_t m_lineMaxValue;
    size_t m_lineStep;
    size_t m_memoryViewType;
    size_t m_latestRecordTimeForUI;
    size_t m_memoryRecordCountForUI;
    int m_lastSelectedFilterPos;
    int m_memoryRecordCacheStartPos;
    bool m_initFlag;
    bool m_bMemoryUINeedUpdate;

    //performance
    std::map<size_t, wxTreeItemId> m_performTreeItemMap;//use this map to quick find the wxItemId for a specific id of record
    std::map<size_t, float> m_updateValueMap;
    std::map<size_t, TString> m_staticLinkDllModu;
    std::map<size_t, std::vector<char*>> m_symbolList;
    std::map<size_t, TString> m_dynamicLinkDllModu;
    std::set<TString> m_filterTexts;

    //component
    size_t m_uLastComponentSearchTextUpdateTime;
    size_t m_uLastComponentFileSearchTextUpdateTime;
    bool m_bComponentSearchTextUpdate;
    bool m_bComponentFileSearchTextUpdate;
    wxSplitterWindow* m_pComponentSpliterWindow;
    wxNotebook* m_pComponentPageNoteBook;
    wxPanel* m_pComponentListPanel;
    wxPanel* m_pComponentFileSelectePanel;
    wxTreeCtrl* m_pComponentTreeControl;
    wxTreeCtrl* m_pComponentFileTreeControl;
    wxSearchCtrl* m_pComponentSearchCtrl;
    wxSearchCtrl* m_pComponentFileSearchCtrl;
    wxPropertyGridManager* m_pPropertyGridManager;
    CComponentRenderWindow* m_pComponentRenderWindow;
    CComponentEditorProxy* m_pSelectedComponent;
    wxPGEditor* m_pPtrButtonEditor;
    wxMenu* m_pPropertyMenu;
    wxMenu* m_pComponentFileMenu;
    wxStaticBoxSizer* m_pComponentRenderWindowSizer;
    wxButton* m_pComponentSaveButton;
    wxButton* m_pOperateFileButton;
    wxButton* m_pSerializeButton;
    bool m_bNeedUpdatePropertyGrid;
    std::map<size_t, wxTreeItemId> m_componentTreeIdMap; //use this map to quick find the wxItemId for a specific id of component
    std::map<TString, wxTreeItemId> m_componentCatalogNameMap; //use this map to quick find the wxItemId for a catalog
    std::map<TString, wxTreeItemId> m_componentFileListMap; //use this map to quick find the wxItemId for a component file name.

    // Spy
    HMODULE m_hSpyDllHandle;
    wxPanel* m_pSpyInfoPanel;
    wxNotebook* m_pSpyNoteBook;
    wxStaticText* m_pSpyNetAdapterInfoLabel;
    wxScrolledWindow* m_pSpyNetAdapterInfoWnd;
    wxStaticText* m_pSpyNetAdapterInfoText;
    wxStaticText* m_pSpyHostNameLabel;
    wxButton* m_pSpyConnectBtn;
};

#endif