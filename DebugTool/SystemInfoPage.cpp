#include "stdafx.h"
#include "BDTWxFrame.h"
#include "SystemDetector/SystemDetector.h"
#include <Psapi.h>
#include <process.h>
#include <Dbghelp.h>
#include <boost/filesystem.hpp>

void CBDTWxFrame::CreateSystemInfoPage()
{
    m_pOverviewPanel = new wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    wxBoxSizer* pOverviewSizer = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBoxSizer* pHardwareInfoSizer = new wxStaticBoxSizer( new wxStaticBox( m_pOverviewPanel, wxID_ANY, wxT("电脑概览") ), wxVERTICAL );

    wxBoxSizer*    pTextInfoSizer = new wxBoxSizer( wxVERTICAL );

    TString info;
    bool ret = false;
    ret = CSystemDetector::GetInstance()->GetComputerModel(info);
    BEATS_ASSERT(ret, _T("GetComputerModel failed!"));
    m_pComputerModelLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("电脑型号: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pComputerModelLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pComputerModelLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetOSName(info);
    BEATS_ASSERT(ret, _T("GetOSName failed!"));
    TString dxVer;
    ret = CSystemDetector::GetInstance()->GetDirectXInfo(dxVer);
    BEATS_ASSERT(ret, _T("GetDirectXInfo failed!"));
    m_pOperatingSystemLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("操作系统: %s %s", info.c_str(), dxVer.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pOperatingSystemLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pOperatingSystemLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetOSVersion(info);
    BEATS_ASSERT(ret, _T("GetOSVersion failed!"));
    m_pOperatingSystemVersionLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("系统版本: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pOperatingSystemVersionLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pOperatingSystemVersionLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetProcessorInfo(info);
    BEATS_ASSERT(ret, _T("GetProcessorInfo failed!"));
    m_pProcessorLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("处理器: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pProcessorLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pProcessorLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetBaseBoardInfo(info);
    BEATS_ASSERT(ret, _T("GetBaseBoardInfo failed!"));
    m_pBaseBoardLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("主板: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pBaseBoardLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pBaseBoardLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetMemoryInfo(info);
    BEATS_ASSERT(ret, _T("GetMemoryInfo failed!"));
    m_pPhysMemoryLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("内存: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pPhysMemoryLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pPhysMemoryLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetHardDiskInfo(info);
    BEATS_ASSERT(ret, _T("GetHardDiskInfo failed!"));
    m_pMainHardDiskLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("硬盘: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pMainHardDiskLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pMainHardDiskLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetGraphicsCardInfo(info);
    BEATS_ASSERT(ret, _T("GetGraphicsCardInfo failed!"));
    m_pDisplayCardLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("显卡信息: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pDisplayCardLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pDisplayCardLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetMonitorInfo(info);
    BEATS_ASSERT(ret, _T("GetMonitorInfo failed!"));
    m_pMonitorLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("显示器: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pMonitorLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pMonitorLab, 0, wxALL, 5 );

    ret = CSystemDetector::GetInstance()->GetSoundCardInfo(info);
    BEATS_ASSERT(ret, _T("GetSoundCardInfo failed!"));
    m_pSoundCardLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("声卡: %s", info.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pSoundCardLab->Wrap( -1 );
    pTextInfoSizer->Add( m_pSoundCardLab, 0, wxALL, 5 );

    pHardwareInfoSizer->Add(pTextInfoSizer, 1, wxALL, 5);

    wxStaticBoxSizer* pHotfixSizer = new wxStaticBoxSizer( new wxStaticBox( m_pOverviewPanel, wxID_ANY, wxT("系统补丁") ), wxVERTICAL );

    m_pHotFixListBox = new wxListBox( m_pOverviewPanel, wxID_ANY, wxDefaultPosition, wxSize(350, 400), 0, NULL, 0 ); 
    pHotfixSizer->Add( m_pHotFixListBox, 1, wxEXPAND, 5 );

    std::vector<TString> hotFixList;
    CSystemDetector::GetInstance()->GetHotFixList(hotFixList);
    for (size_t i = 0; i < hotFixList.size(); ++i)
    {
        if (hotFixList[i].compare(_T("File 1")) != 0 )//filter unknow hotfix names
        {
            m_pHotFixListBox->AppendAndEnsureVisible(wxString(hotFixList[i].c_str()));
        }
    }
    pHardwareInfoSizer->Add( pHotfixSizer, 1, wxEXPAND, 5 );


    wxStaticBoxSizer* pProcessInfoSizer = new wxStaticBoxSizer( new wxStaticBox( m_pOverviewPanel, wxID_ANY, wxT("进程信息") ), wxVERTICAL );
    TCHAR path[1024] = {0};
    HMODULE processModule = GetModuleHandle(NULL);
    DWORD pathLength = GetModuleFileName(processModule, path, 1024);
    BEATS_ASSERT(pathLength != 0, _T("Get process path failed!"));
    m_pProcessNameLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("进程路径: %s", path), wxDefaultPosition, wxDefaultSize, 0 );
    m_pProcessNameLab->Wrap( -1 );
    pProcessInfoSizer->Add( m_pProcessNameLab, 0, wxALL, 5 );

    m_pProcessSpaceLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxString::Format("进程PID: %d", _getpid()), wxDefaultPosition, wxDefaultSize, 0 );
    m_pProcessSpaceLab->Wrap( -1 );
    pProcessInfoSizer->Add( m_pProcessSpaceLab, 0, wxALL, 5 );

    wxStaticBoxSizer* pMoudleInfoSizer = new wxStaticBoxSizer( new wxStaticBox( m_pOverviewPanel, wxID_ANY, wxT("模块信息") ), wxVERTICAL );
    
    wxBoxSizer* pLoadTypeSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pModuleLoadTypeLab = new wxStaticText( m_pOverviewPanel, wxID_ANY, wxT("加载方式"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pModuleLoadTypeLab->Wrap( -1 );
    pLoadTypeSizer->Add( m_pModuleLoadTypeLab, 0, wxALIGN_CENTER | wxALL, 5 );

    wxString sChoices[] = { wxT("静态链接"), wxT("动态加载") };
    int sChoicesLength = sizeof( sChoices ) / sizeof( wxString );
    m_pModuleLoadTypeChoice = new wxChoice( m_pOverviewPanel, wxID_ANY, wxDefaultPosition, wxSize(150, -1), sChoicesLength, sChoices, 0 );
    pLoadTypeSizer->Add( m_pModuleLoadTypeChoice, 0, wxALIGN_CENTER | wxALL, 5 );

    m_pModuleShowFullPath = new wxCheckBox( m_pOverviewPanel, wxID_ANY, wxT("显示路径"), wxDefaultPosition, wxDefaultSize, 0 );
    pLoadTypeSizer->Add( m_pModuleShowFullPath, 0, wxALIGN_CENTER | wxALL, 5 );

    pMoudleInfoSizer->Add(pLoadTypeSizer, 0, wxALL, 5);

    m_pModuleSelectLab = new wxStaticText( m_pOverviewPanel, wxID_ANY,  wxT("选择模块"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pModuleSelectLab->Wrap( -1 );

    pMoudleInfoSizer->Add( m_pModuleSelectLab, 0, wxALL, 5 );

    m_pModuleList = new wxListBox( m_pOverviewPanel, wxID_ANY, wxDefaultPosition, wxSize( 350,120 ), 0, NULL, wxLB_SINGLE | wxLB_HSCROLL );
    pMoudleInfoSizer->Add( m_pModuleList, 0, wxALL, 5 );
    InitStaticLinkModuList();
    InitDynamicLinkModuList();

    m_pModuleSymbolLab = new wxStaticText( m_pOverviewPanel, wxID_ANY,  wxT("加载符号"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pModuleSymbolLab->Wrap( -1 );
    pMoudleInfoSizer->Add( m_pModuleSymbolLab, 0, wxALL, 5 );

    m_pModuleSymbolList = new wxListBox( m_pOverviewPanel, wxID_ANY, wxDefaultPosition, wxSize( 350,120 ), 0, NULL, wxLB_SINGLE | wxLB_HSCROLL );
    pMoudleInfoSizer->Add( m_pModuleSymbolList, 0, wxALL, 5 );
    InitSymbolList();
    m_pModuleLoadTypeChoice->SetSelection( 0 );
    wxCommandEvent cmdEvent;
    cmdEvent.SetInt(0);
    OnModuleLoadTypeChanged(cmdEvent);

    pProcessInfoSizer->Add( pMoudleInfoSizer, 1, wxEXPAND, 5 );
    pOverviewSizer->Add( pHardwareInfoSizer, 1, wxEXPAND, 5 );
    pOverviewSizer->Add( pProcessInfoSizer, 1, wxEXPAND, 5 );

    m_pOverviewPanel->SetSizer( pOverviewSizer );
    m_pOverviewPanel->Layout();
    pOverviewSizer->Fit( m_pOverviewPanel );

    m_pNoteBook->AddPage( m_pOverviewPanel, wxT("系统概览"), false );

    m_pModuleLoadTypeChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnModuleLoadTypeChanged), NULL, this );
    m_pModuleShowFullPath->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnFullPathSelected ), NULL, this );
    m_pModuleList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnModuleSelectChanged ), NULL, this );
}

void CBDTWxFrame::DestroySystemInfoPage()
{
    m_pModuleLoadTypeChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnModuleLoadTypeChanged), NULL, this );
    m_pModuleShowFullPath->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnFullPathSelected ), NULL, this );
    m_pModuleList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnModuleSelectChanged ), NULL, this );
}

void CBDTWxFrame::OnModuleLoadTypeChanged( wxCommandEvent& event )
{
    m_pModuleList->Clear();
    m_pModuleSymbolList->Clear();
    std::map<size_t, TString>& operateMap = event.GetSelection() == 0 ? 
                                            m_staticLinkDllModu : 
                                            m_dynamicLinkDllModu;
    std::map<size_t, TString>::iterator iter = operateMap.begin();
    for (; iter != operateMap.end(); ++iter)
    {
        boost::filesystem::path curPath(iter->second.c_str());
        m_pModuleList->AppendAndEnsureVisible(m_pModuleShowFullPath->IsChecked() ? 
            iter->second.c_str():
            curPath.filename().c_str());
    }
}

void CBDTWxFrame::InitStaticLinkModuList()
{
    IMAGE_SECTION_HEADER* section = NULL;
    ULONG size = 0;
    HMODULE processModule = GetModuleHandle(NULL);
    IMAGE_IMPORT_DESCRIPTOR* idte = 
        (IMAGE_IMPORT_DESCRIPTOR*)ImageDirectoryEntryToDataEx( (PVOID)processModule,
        TRUE,
        IMAGE_DIRECTORY_ENTRY_IMPORT, 
        &size, 
        &section);
    //it must import something so it can't be null.
    if (idte != NULL) 
    {
        while (idte->OriginalFirstThunk != 0x0) 
        {
            PCHAR moduleName = (PCHAR)((PBYTE)processModule + idte->Name);
            HMODULE handle = GetModuleHandleA(moduleName);
            BEATS_ASSERT(m_staticLinkDllModu.find(size_t(handle)) == m_staticLinkDllModu.end(),
                        _T("the module is already in static link module list."));
            TCHAR pathCache[256];
            GetModuleFileName(handle, pathCache, 256);
            m_staticLinkDllModu[(size_t)handle] = TString(pathCache);
            ++idte;
        }
    }
}

void CBDTWxFrame::GetSymbol(IMAGE_THUNK_DATA* pITDdata, HMODULE processModule, HMODULE dllModule)
{
    if (pITDdata != NULL && pITDdata->u1.AddressOfData != 0)
    {
        bool exportByName = (pITDdata->u1.AddressOfData & (1 << 31))== 0;//test if it is export by name.
        if (exportByName)
        {
            IMAGE_IMPORT_BY_NAME* pIBNdata = (IMAGE_IMPORT_BY_NAME*)R2VA(processModule, pITDdata->u1.AddressOfData);
            char* symbolName = (char*)(&pIBNdata->Name);
            m_symbolList[(size_t)dllModule].push_back(symbolName);
            GetSymbol(++pITDdata, processModule, dllModule);
        }
        else
        {
            IMAGE_SECTION_HEADER* section = NULL;
            ULONG size = 0;

            IMAGE_EXPORT_DIRECTORY* iede = 
                (IMAGE_EXPORT_DIRECTORY*)ImageDirectoryEntryToDataEx( 
                (PVOID)dllModule,
                TRUE,
                IMAGE_DIRECTORY_ENTRY_EXPORT, 
                &size, 
                &section);
            //if the name is hidden.
            if (iede->AddressOfNames == 0 || iede->AddressOfNameOrdinals == 0)
            {
                m_symbolList[(size_t)dllModule].push_back("无符号名称信息");
            }
            else
            {
                //export by id
                size_t id = pITDdata->u1.AddressOfData & (~(1 << 31));
                if ( id != 0)
                {
                    WORD* pExportFuncIndexRVA = (WORD*)R2VA(size_t(dllModule), iede->AddressOfNameOrdinals);
                    DWORD* pExportFuncNameRVA = (DWORD*)R2VA(size_t(dllModule), iede->AddressOfNames);
                    while (*pExportFuncIndexRVA != id - iede->Base)
                    {
                        ++pExportFuncIndexRVA;
                        ++pExportFuncNameRVA;
                    }
                    m_symbolList[(size_t)dllModule].push_back((char*)R2VA(size_t(dllModule), *pExportFuncNameRVA));

                    GetSymbol(++pITDdata, processModule, dllModule);
                }
            }
        }
    }
}

void CBDTWxFrame::InitSymbolList()
{
    for (std::map<size_t, TString>::iterator iter = m_staticLinkDllModu.begin(); iter != m_staticLinkDllModu.end(); ++iter)
    {
        boost::filesystem::path curPath(iter->second.c_str());
        TString dllName = curPath.filename().c_str();
        if (dllName.length() > 0)
        {
            IMAGE_SECTION_HEADER* section = NULL;
            ULONG size = 0;
            HMODULE processModule = GetModuleHandle(NULL);
            IMAGE_IMPORT_DESCRIPTOR* idte = 
                (IMAGE_IMPORT_DESCRIPTOR*)ImageDirectoryEntryToDataEx( (PVOID)processModule,
                TRUE,
                IMAGE_DIRECTORY_ENTRY_IMPORT, 
                &size, 
                &section);
            wxString targetName((char*)R2VA(processModule, idte->Name));
            while (idte->OriginalFirstThunk != NULL &&
                targetName.CompareTo(dllName.c_str(), wxString::ignoreCase) != 0 )
            {
                ++idte;
                targetName = ((char*)R2VA(processModule, idte->Name));
            }
            if (idte->OriginalFirstThunk != NULL)
            {
                IMAGE_THUNK_DATA* pITDdata =(IMAGE_THUNK_DATA*)R2VA(processModule, idte->OriginalFirstThunk);
                GetSymbol(pITDdata, processModule, (HMODULE)iter->first);
            }
        }
    }
}

void CBDTWxFrame::InitDynamicLinkModuList()
{
    HMODULE moduleCache[1024];
    DWORD cacheNeeded = 0;
    HANDLE hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, _getpid() );

    EnumProcessModules(hProcess, moduleCache, sizeof(moduleCache), &cacheNeeded);
    BEATS_ASSERT(cacheNeeded < sizeof(moduleCache), _T("Moudule cache is too small!"));
    size_t moduleCount = cacheNeeded / sizeof(HMODULE);
    for (size_t i = 0; i < moduleCount; ++i)
    {
        if (moduleCache[i] != GetModuleHandle(NULL))//filter exe it self.
        {
            if (m_staticLinkDllModu.find((size_t)moduleCache[i]) == m_staticLinkDllModu.end())
            {   
                TCHAR nameCache[256];
                GetModuleFileName(moduleCache[i],nameCache, sizeof(nameCache));
                BEATS_ASSERT(m_dynamicLinkDllModu.find((size_t)moduleCache[i]) == m_dynamicLinkDllModu.end(),
                    _T("the module is already in dynamic link dll module list"));
                m_dynamicLinkDllModu[(size_t)moduleCache[i]] = nameCache;
            }
        }
    }
}

void CBDTWxFrame::OnModuleSelectChanged( wxCommandEvent& /*event*/ )
{
    if (m_pModuleLoadTypeChoice->GetSelection() == 0)
    {
        m_pModuleSymbolList->Clear();
        wxString selectedModuleName = m_pModuleList->GetStringSelection();
        HMODULE module = GetModuleHandle(selectedModuleName);
        std::map<size_t, std::vector<char*>>::iterator iter = m_symbolList.find(size_t(module));
        if (iter != m_symbolList.end())
        {
            for (size_t i = 0; i < iter->second.size(); ++i)
            {
                m_pModuleSymbolList->Append(iter->second[i]);
            }
        }
    }
}

void CBDTWxFrame::OnFullPathSelected( wxCommandEvent& event )
{
    FillModuleList(m_pModuleLoadTypeChoice->GetSelection() == 1, event.GetSelection() == 1);
}

void CBDTWxFrame::FillModuleList( bool isDynamicList, bool fullpath )
{
    m_pModuleList->Clear();
    std::map<size_t, TString>& operateMap = isDynamicList ? 
                                            m_dynamicLinkDllModu: 
                                            m_staticLinkDllModu;
    std::map<size_t, TString>::iterator iter = operateMap.begin();
    for (; iter != operateMap.end(); ++iter)
    {
        boost::filesystem::path curPath(iter->second.c_str());
        m_pModuleList->AppendAndEnsureVisible(fullpath ? 
                                                iter->second.c_str():
                                                curPath.filename().c_str());
    }
}