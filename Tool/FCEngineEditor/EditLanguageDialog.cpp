#include "stdafx.h"
#include "EditLanguageDialog.h"
#include "Utility\BeatsUtility\Serializer.h"
#include <wx/srchctrl.h>
#include "Resource/ResourcePathManager.h"

BEGIN_EVENT_TABLE(EditLanguageDialog, EditDialogBase)
    EVT_BUTTON(ID_BUTTON_ADD, EditLanguageDialog::OnButtonAdd)
    EVT_BUTTON(ID_BUTTON_EXPORT, EditLanguageDialog::OnButtonExport)
    EVT_BUTTON(ID_BUTTON_CLEAR, EditLanguageDialog::OnButtonClear)
    EVT_SEARCHCTRL_SEARCH_BTN(ID_SERACH_LANGUAGE, EditLanguageDialog::OnSearchLanguage)
    EVT_TEXT_ENTER(ID_SERACH_LANGUAGE,EditLanguageDialog::OnSearchEnterLanguange)
    EVT_SEARCHCTRL_CANCEL_BTN(ID_SERACH_LANGUAGE, EditLanguageDialog::OnCancleSrch)
    EVT_TEXT(ID_SERACH_LANGUAGE, EditLanguageDialog::OnSrchUpdate)
    EVT_TEXT(ID_TEXT_ENUM, EditLanguageDialog::OnEnumTextUpdate)
    EVT_IDLE(EditLanguageDialog::OnSrchIdle)
    EVT_DATAVIEW_SELECTION_CHANGED(ID_DATAVIEW_LANGUAGE, EditLanguageDialog::OnSelectDataView)
    EVT_DATAVIEW_ITEM_CONTEXT_MENU(ID_DATAVIEW_LANGUAGE, EditLanguageDialog::OnContextMenu)
    EVT_MENU(wxID_ANY, EditLanguageDialog::OnMenuEvent)
END_EVENT_TABLE()

EditLanguageDialog::EditLanguageDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : EditDialogBase(parent, id, title, pos, size, style, name)
    , m_selectRow(-1)
    , m_uLastEnumSearchTextUpdateTime(0)
    , m_bEnumSearchTextUpdate(false)
    , m_strChangeEnum(_T(""))
    , m_tmpBeforeStr(_T(""))
    , m_bAddSaveSuc(false)
    , m_bIsChange(false)
    , m_bISLanguageSwitch(false)
    , m_bIsSave(false)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pBottomSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pLeftChildeSizer1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pLeftChildeSizer2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pLeftChildeSizer3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pLeftChildeSizer4 = new wxBoxSizer(wxHORIZONTAL);

    m_pDataViewListLanguage = new wxDataViewListCtrl(this, ID_DATAVIEW_LANGUAGE);
    m_pButtonClear = new wxButton(this, ID_BUTTON_CLEAR, _T("Clear"), wxDefaultPosition,wxSize(60,30));
    m_pButtonAdd = new wxButton(this, ID_BUTTON_ADD, _T("Add"), wxDefaultPosition,wxSize(60,30));
    m_pButtonExport = new wxButton(this, ID_BUTTON_EXPORT, _T("Export"), wxDefaultPosition,wxSize(60,30));
    m_pTextEnum = new wxTextCtrl(this, ID_TEXT_ENUM, _T("eL_"));
    m_pTextChinese = new wxTextCtrl(this, ID_TEXT_CHINESE);
    m_pTextEnglish = new wxTextCtrl(this, ID_TEXT_ENGLISH);
    m_pStaticTextEnum = new wxStaticText(this, wxID_ANY, _T("  Enum "));
    m_pStaticTextChinese = new wxStaticText(this, wxID_ANY, _T("Chinese"));
    m_pStaticTextEnglish = new wxStaticText(this, wxID_ANY, _T(" English"));
    m_pSrchCtrl = new wxSearchCtrl(this, ID_SERACH_LANGUAGE, _T(""),wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    wxStaticText* pSrchText = new wxStaticText(this, wxID_ANY, _T("枚举搜索："));

    pSizer->Add(pTopSizer, 0.5, wxGROW|wxALL, 5);
    pSizer->Add(pBottomSizer, 1, wxGROW|wxALL, 5);

    pTopSizer->Add(pLeftChildeSizer1, 1, wxGROW|wxALL, 20);
    pTopSizer->Add(pLeftChildeSizer2, 1, wxGROW|wxALL, 20);
    pTopSizer->Add(pLeftChildeSizer3, 1, wxGROW|wxALL, 20);
    pTopSizer->Add(pLeftChildeSizer4, 1, wxGROW|wxALL, 10);

    pLeftChildeSizer1->Add(m_pStaticTextEnum, 0, wxGROW|wxALL, 5);
    pLeftChildeSizer1->Add(m_pTextEnum, 1, wxGROW|wxALL, 5);
    pLeftChildeSizer2->Add(m_pStaticTextChinese, 0, wxGROW|wxALL, 5);
    pLeftChildeSizer2->Add(m_pTextChinese, 1, wxGROW|wxALL, 5);
    pLeftChildeSizer3->Add(m_pStaticTextEnglish, 0, wxGROW|wxALL, 5);
    pLeftChildeSizer3->Add(m_pTextEnglish, 1, wxGROW|wxALL, 5);
    pLeftChildeSizer4->AddStretchSpacer(1);
    pLeftChildeSizer4->Add(m_pButtonClear, 0, wxGROW|wxALL, 10);
    pLeftChildeSizer4->Add(m_pButtonAdd, 0, wxGROW|wxALL, 10);
    pLeftChildeSizer4->Add(m_pButtonExport, 0, wxGROW|wxALL, 10);
    pLeftChildeSizer4->AddStretchSpacer(1);

    pBottomSizer->Add(m_pDataViewListLanguage, 1, wxGROW|wxALL, 5);
    pBottomSizer->Add(pSrchText, 0, wxGROW|wxALL, 5);
    pBottomSizer->Add(m_pSrchCtrl, 0, wxGROW|wxALL, 5);

    m_pSrchCtrl->ShowCancelButton(true);
    InitLanguageMap();
    InitDataViewListCtrl();
    SetTextCtrlEnum();
}

EditLanguageDialog::~EditLanguageDialog()
{

}

void EditLanguageDialog::InitLanguageMap()
{
    TString tmpPath = _T("Language.bin");
    TString FilePath = CResourcePathManager::GetInstance()->GetFullPathForFilename("Language.bin");
    if (tmpPath == FilePath)
    {
        return;
    }
    int count = 0;
    CSerializer tmp(FilePath.c_str());
    tmp >> count;
    for (int i = 0; i < count; i++)
    {
        TString tmpstrEnum;
        tmp >> tmpstrEnum;
        TString tmpstrChinese;
        tmp >> tmpstrChinese;
        TString tmpstrEnglish;
        tmp >> tmpstrEnglish;
        std::vector<TString> mLanguageVector;
        mLanguageVector.push_back(tmpstrChinese);
        mLanguageVector.push_back(tmpstrEnglish);
        m_languageMap.insert(std::map<TString, std::vector<TString>>::value_type(tmpstrEnum,mLanguageVector));
    }
}

void EditLanguageDialog::InitDataViewListCtrl()
{
    m_pDataViewListLanguage->AppendTextColumn("枚举");
    m_pDataViewListLanguage->AppendTextColumn("中文");
    m_pDataViewListLanguage->AppendTextColumn("英文");
    AppendDataViewListCtrl();
}

void EditLanguageDialog::OnButtonAdd( wxCommandEvent& /*event*/ )
{
    if (m_pButtonAdd->GetLabel() == _T("Add") ||
        m_pButtonAdd->GetLabel() == CLanguageManager::GetInstance()->GetText(eL_Add))
    {
        AddSaveButtonPrompt();
        if (m_bAddSaveSuc)
        {
            AddLanguage();
            AppendDataViewListCtrl();
            ClearTextCtrl();
            m_pSrchCtrl->SetValue(_T(""));
        }
        m_bAddSaveSuc = false;
    }
    else if (m_pButtonAdd->GetLabel() == _T("Save") ||
        m_pButtonAdd->GetLabel() == CLanguageManager::GetInstance()->GetText(eL_Save))
    {
        AddSaveButtonPrompt();
        if (m_bAddSaveSuc)
        {
            m_pButtonAdd->SetLabel(m_bISLanguageSwitch ? CLanguageManager::GetInstance()->GetText(eL_Add) : _T("Add"));
            DleLanguage();
            AddLanguage();
            AppendDataViewListCtrl();
            ClearTextCtrl();
            m_pSrchCtrl->SetValue(_T(""));
        }
        m_bAddSaveSuc = false;
        m_bIsChange = false;
        m_bIsSave = false;
    }
}

void EditLanguageDialog::OnButtonExport( wxCommandEvent& /*event*/ )
{
    // 1. Export Bin File.
    bool bExportBin = false;
    int count = m_languageMap.size();
    CSerializer tmp;
    tmp << count;
    for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); iter++)
    {
        tmp << iter->first;
        tmp << iter->second[0];
        tmp << iter->second[1];
    }
    TString FilePath = CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Language) + _T("Language.bin");
    bExportBin = tmp.Deserialize(FilePath.c_str());

    //2. Export enum file.
    bool bExporthead;
    std::string content;
    content.append("#ifndef FCENGINEEDITOR_LANGUAGEENUM_H__INCLUDE\n#define FCENGINEEDITOR_LANGUAGEENUM_H__INCLUDE\n").append("\nenum ELanguage\n{\n");
    char szBuffer[128];
    for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); iter++)
    {
        CStringHelper::GetInstance()->ConvertToCHAR(iter->first.c_str(), szBuffer, 128);
        content.append("    ").append(szBuffer).append(",\n");
    }
    content.append("\n    eL_Count,\n").append("    eL_Force32Bit = 0xFFFFFFFF\n};\n");
    content.append("\n#endif");
    tmp.Reset();
    tmp << content;
    FilePath = CResourcePathManager::GetInstance()->GetResourcePath(eRPT_SourceCode) + _T("Language.h");
    bExporthead = tmp.Deserialize(FilePath.c_str(), _T("wt+"));

    //Export hint.
    TString strMessage;
    if (bExportBin && bExporthead)
    {
        strMessage = _T("Export Bin File and enum file succeed.");
    }
    else
    {
        strMessage = _T("Export Bin File or enum file failed.");
    }
    wxMessageDialog* pMessageDlg = new wxMessageDialog(this, strMessage, _T("Edit Language"), wxOK|wxCENTRE|wxICON_NONE);
    pMessageDlg->ShowModal();
}

void EditLanguageDialog::OnSearchLanguage( wxCommandEvent& event )
{
    TString strEnum = event.GetString();
    SearchEnum(strEnum);
}

void EditLanguageDialog::OnSearchEnterLanguange( wxCommandEvent& event )
{
    TString strEnum = event.GetString();
    SearchEnum(strEnum);
}

void EditLanguageDialog::AppendDataViewListCtrl()
{
    m_pDataViewListLanguage->DeleteAllItems();
    wxVector<wxVariant> data;
    for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); iter++)
    {
        data.clear();
        data.push_back( iter->first );
        data.push_back( iter->second[0] );
        data.push_back( iter->second[1] );

        m_pDataViewListLanguage->AppendItem( data );
    }
}

void EditLanguageDialog::OnButtonClear( wxCommandEvent& /*event*/ )
{
    m_bIsSave = false;
    ClearTextCtrl();
    m_pButtonAdd->SetLabel(m_bISLanguageSwitch ? CLanguageManager::GetInstance()->GetText(eL_Add) : _T("Add"));
}

void EditLanguageDialog::OnSelectDataView( wxDataViewEvent& /*event*/ )
{
    m_selectRow = m_pDataViewListLanguage->GetSelectedRow();
    if (m_selectRow == -1)
    {
        return;
    }
    wxString strEnum = m_pDataViewListLanguage->GetTextValue(m_selectRow, 0);
    m_strChangeEnum = strEnum;
}

void EditLanguageDialog::ClearTextCtrl()
{
    m_pTextEnum->SetValue(_T("eL_"));
    m_pTextChinese->Clear();
    m_pTextEnglish->Clear();
}

void EditLanguageDialog::OnCancleSrch( wxCommandEvent& /*event*/ )
{
    AppendDataViewListCtrl();
}

void EditLanguageDialog::SearchEnum( TString str )
{
    for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); iter++)
    {
        if (iter->first == str)
        {
            m_pDataViewListLanguage->DeleteAllItems();
            wxVector<wxVariant> data;
            data.push_back( iter->first );
            data.push_back( iter->second[0] );
            data.push_back( iter->second[1] );
            m_pDataViewListLanguage->AppendItem( data );
        }
    }
}

void EditLanguageDialog::OnSrchUpdate( wxCommandEvent& /*event*/ )
{
    m_uLastEnumSearchTextUpdateTime = GetTickCount();
    m_bEnumSearchTextUpdate = true;
}

void EditLanguageDialog::OnEnumTextUpdate( wxCommandEvent& /*event*/ )
{
    wxString tmpNowStr;
    tmpNowStr = m_pTextEnum->GetValue();
    if (tmpNowStr.size() < 3)
    {
         wxString tmpStr = _T("eL_");
         m_pTextEnum->SetValue(tmpStr);
    }
    m_pTextEnum->SetInsertionPointEnd();
}

void EditLanguageDialog::OnSrchIdle( wxIdleEvent& /*event*/ )
{
    if (m_bEnumSearchTextUpdate && GetTickCount() - m_uLastEnumSearchTextUpdateTime > 700)
    {
        m_bEnumSearchTextUpdate = false;
        wxString szText = m_pSrchCtrl->GetValue();
        m_pDataViewListLanguage->DeleteAllItems();
        for (auto iter = m_languageMap.begin(); iter != m_languageMap.end(); ++iter)
        {
            wxString lableText = iter->first;
            bool bMatch = lableText.Find(szText) != -1;
            if (bMatch)
            {
                wxVector<wxVariant> data;
                data.push_back( iter->first );
                data.push_back( iter->second[0] );
                data.push_back( iter->second[1] );
                m_pDataViewListLanguage->AppendItem( data );
            }
        }
    }
}

void EditLanguageDialog::OnContextMenu( wxDataViewEvent& /*event*/ )
{
    wxMenu menu;
    CLanguageManager* pLanguageManager = CLanguageManager::GetInstance();
    menu.Append( ID_POPMENU_CHANGE, m_bISLanguageSwitch ? pLanguageManager->GetText(eL_Change) : _T("Change"));
    menu.Append( ID_POPMENU_DLE, m_bISLanguageSwitch ? pLanguageManager->GetText(eL_Delete) : _T("Delete"));
    m_pDataViewListLanguage->PopupMenu(&menu);
}

void EditLanguageDialog::OnMenuEvent( wxCommandEvent& event )
{
    int id = event.GetId();
    if (id == ID_POPMENU_CHANGE)
    {
        if (m_selectRow != -1)
        {
            m_pButtonAdd->SetLabel(m_bISLanguageSwitch ? CLanguageManager::GetInstance()->GetText(eL_Save) : _T("Save"));
            ChangeLanguage();
            m_bIsChange = true;
            m_bIsSave = true;
        }
    }
    else if (id == ID_POPMENU_DLE)
    {
        DleLanguage();
        AppendDataViewListCtrl();
    }
}

void EditLanguageDialog::ChangeLanguage()
{
    TString strDataviewEnum = m_pDataViewListLanguage->GetTextValue(m_selectRow, 0);
    if (m_languageMap.find(strDataviewEnum) != m_languageMap.end())
    {
         m_pTextEnum->SetValue(m_languageMap.find(strDataviewEnum)->first);
         m_pTextChinese->SetValue(m_languageMap.find(strDataviewEnum)->second[0]);
         m_pTextEnglish->SetValue(m_languageMap.find(strDataviewEnum)->second[1]);
    }
}

void EditLanguageDialog::AddLanguage()
{
    TString strInputEnum = m_pTextEnum->GetValue();
    TString strInputChinese = m_pTextChinese->GetValue();
    TString strInputEnglish = m_pTextEnglish->GetValue();
    if (strInputEnum == _T("eL_") || strInputChinese == _T("") || strInputChinese == _T("") )
    {
        return;
    }
    std::vector<TString> mLanguageVector;
    mLanguageVector.push_back(strInputChinese);
    mLanguageVector.push_back(strInputEnglish);
    m_languageMap.insert(std::map<TString, std::vector<TString>>::value_type(strInputEnum,mLanguageVector));
}

void EditLanguageDialog::DleLanguage()
{
    std::map<TString, std::vector<TString>>::iterator iter;
    iter = m_languageMap.find(m_strChangeEnum);
    if (iter != m_languageMap.end())
    {
        m_languageMap.erase(iter);
    }
    m_strChangeEnum = _T("");
}

void EditLanguageDialog::AddSaveButtonPrompt()
{
    TString strMessage;
    TString strInputEnum = m_pTextEnum->GetValue();
    if (strInputEnum.size() == 3)
    {
        strMessage = _T("Enum not only as 'eL_'!");
    }
    else if (m_languageMap.find(strInputEnum) != m_languageMap.end() && m_bIsChange == false)
    {
        strMessage = _T("Enum has been exist!");
    }
    else if (m_pTextChinese->GetValue() == _T(""))
    {
        strMessage = _T("Chinese is empty!");
    }
    else if(m_pTextEnglish->GetValue() == _T(""))
    {
        strMessage = _T("English is empty!");
    }
    else
    {
        strMessage = _T("Added and saved successfully!");
        m_bAddSaveSuc = true;
    }
    wxMessageDialog* pMessageDlg = new wxMessageDialog(this, strMessage, _T("Edit Language"), wxOK|wxCENTRE|wxICON_NONE);
    pMessageDlg->ShowModal();
}

void EditLanguageDialog::SetTextCtrlEnum()
{
    wxArrayString includeList; 
    TCHAR szBuffer;
    for (int i = 65; i <= 90; ++i)
    {
        szBuffer = i;
        includeList.Add(szBuffer);
    }
    for (int i = 97; i <= 122; ++i)
    {
        szBuffer = i;
        includeList.Add(szBuffer);
    }
    for (int i = 48; i <= 57; ++i)
    {
        szBuffer = i;
        includeList.Add(szBuffer);
    }
    includeList.Add(_T("_"));
    wxTextValidator textValidator(wxFILTER_INCLUDE_CHAR_LIST);
    textValidator.SetIncludes(includeList);
    m_pTextEnum->SetValidator(textValidator);
}

void EditLanguageDialog::LanguageSwitch()
{
    m_bISLanguageSwitch = true;
    CLanguageManager* pLanguageManager = CLanguageManager::GetInstance();

    m_pStaticTextEnum->SetLabel(pLanguageManager->GetText(eL_Enum));
    m_pStaticTextChinese->SetLabel(pLanguageManager->GetText(eL_Chinese));
    m_pStaticTextEnglish->SetLabel(pLanguageManager->GetText(eL_English));
    m_pButtonClear->SetLabel(pLanguageManager->GetText(eL_Clear));
    m_pButtonExport->SetLabel(pLanguageManager->GetText(eL_Export));
    SetLanguage();
}

void EditLanguageDialog::SetLanguage()
{
    m_pButtonAdd->SetLabel(CLanguageManager::GetInstance()->GetText(m_bIsSave ? eL_Save: eL_Add));
}
