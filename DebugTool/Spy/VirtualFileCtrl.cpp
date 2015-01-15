#include "stdafx.h"
#include "VirtualFileCtrl.h"
#include "../../Utility/UtilityManager.h"
#include <wx/generic/dirctrlg.h>
#include "FilePath/FilePathTool.h"

CVirtualFileCtrl::CVirtualFileCtrl()
{

}

CVirtualFileCtrl::CVirtualFileCtrl(wxWindow *parent,
                                   wxWindowID id /*= wxID_ANY*/,
                                   const wxPoint& pos/* = wxDefaultPosition*/,
                                   const wxSize& size/* = wxDefaultSize*/,
                                   long style/* = wxLC_ICON*/,
                                   const wxValidator& validator/* = wxDefaultValidator*/,
                                   const wxString& name/* = wxListCtrlNameStr*/)
: super(parent, id, pos, size, style, validator, name)
, m_pCurrentDirectory(NULL)
, m_pDiskInfo(NULL)
{
    wxImageList *imageList = wxTheFileIconsTable->GetSmallImageList();
    SetImageList( imageList, wxIMAGE_LIST_SMALL );
    InsertColumn(0, _T("名称"));
    InsertColumn(1, _T("大小"));
    InsertColumn(2, _T("类型"));
    InsertColumn(3, _T("UnDefine"));

}

CVirtualFileCtrl::~CVirtualFileCtrl()
{

}

void CVirtualFileCtrl::SetCurrentDirectory(const SDirectory* pCurDirectory)
{
    m_pCurrentDirectory = pCurDirectory;
    m_strCurDirectoryPath = pCurDirectory->m_szPath;
    UpdateFiles();
}

void CVirtualFileCtrl::SetDiskInfo(const SDiskInfo* pDiskInfo)
{
    m_pDiskInfo = pDiskInfo;
    m_strCurDirectoryPath.clear();
    UpdateFiles();
}

void CVirtualFileCtrl::UpdateFiles()
{
    DeleteAllItems();

    if (m_strCurDirectoryPath.length() == 0)
    {
        wxListItem columnItem;
        if(GetColumn(3, columnItem))
        {
            columnItem.SetText(_T("剩余空间"));
            SetColumn(3, columnItem);
        }

        wxListItem item;
        BEATS_ASSERT(m_pDiskInfo != NULL, _T("Disk Info Can't be NULL!"));
        for (uint32_t i = 0; i < m_pDiskInfo->m_logicDriverInfo.size(); ++i)
        {
            item.m_itemId = i;
            item.m_col = 0;
            const SLogicDriverInfo& info = m_pDiskInfo->m_logicDriverInfo.at(i);
            item.SetText(info.m_diskName.c_str());
            item.SetImage(GetDriverIconByType(info.m_type));
            InsertItem(item);

            item.SetMask(item.GetMask() & ~wxLIST_MASK_IMAGE);
            ++item.m_col;
            item.SetText(GetBytesDisplayString(info.m_diskSize));
            SetItem(item);

            ++item.m_col;
            item.SetText(wxString::Format(_T("%d"), info.m_type));
            SetItem(item);

            ++item.m_col;
            item.SetText(GetBytesDisplayString(info.m_diskFreeSize));
            SetItem(item);

        }
    }
    else
    {
        wxListItem columnItem;
        if(GetColumn(3, columnItem))
        {
            columnItem.SetText(_T("修改日期"));
            SetColumn(3, columnItem);
        }

        uint32_t uItemCounter = 0;

        wxListItem item;
        item.SetId(uItemCounter++);
        item.SetColumn(0);
        item.SetText(_T(".."));
        item.SetImage(wxFileIconsTable::folder);
        InsertItem(item);
        BEATS_ASSERT(m_pCurrentDirectory != NULL, _T("Directory Info Can't be NULL!"));
        for (uint32_t i = 0; i < m_pCurrentDirectory->m_pDirectories->size(); ++i)
        {
            item.SetId(uItemCounter++);
            item.SetColumn(0);
            const SDirectory* pChildDirectory = m_pCurrentDirectory->m_pDirectories->at(i);
            item.SetText(pChildDirectory->m_data.cFileName);
            item.SetImage(wxFileIconsTable::folder);
            InsertItem(item);

            item.SetMask(item.GetMask() & ~wxLIST_MASK_IMAGE);
            ++item.m_col;
            item.SetText(_T(""));
            SetItem(item);

            ++item.m_col;
            item.SetText(_T("文件夹"));
            SetItem(item);

            ++item.m_col;
            FILETIME localFileTime;
            SYSTEMTIME localSystemTime;
            FileTimeToLocalFileTime(&pChildDirectory->m_data.ftLastWriteTime, &localFileTime);
            FileTimeToSystemTime(&localFileTime, &localSystemTime);
            item.SetText(wxString::Format(_T("%d-%d-%d %d:%.2d"), 
                localSystemTime.wYear,
                localSystemTime.wMonth,
                localSystemTime.wDay,
                localSystemTime.wHour,
                localSystemTime.wMinute));
            SetItem(item);
        }
        item.Clear();
        for (uint32_t i = 0; i < m_pCurrentDirectory->m_pFileList->size(); ++i)
        {
            const WIN32_FIND_DATA* pData = m_pCurrentDirectory->m_pFileList->at(i);
            item.SetId(uItemCounter++);
            item.SetColumn(0);
            item.SetText(pData->cFileName);
            item.SetImage(wxFileIconsTable::file);
            InsertItem(item);

            item.SetMask(item.GetMask() & ~wxLIST_MASK_IMAGE);
            ++item.m_col;
            unsigned long long uFileSize = pData->nFileSizeHigh;
            uFileSize = uFileSize << 32;
            uFileSize += pData->nFileSizeLow;
            item.SetText(GetBytesDisplayString(uFileSize));
            SetItem(item);

            ++item.m_col;
            item.SetText(CFilePathTool::GetInstance()->Extension(pData->cFileName).c_str());
            SetItem(item);

            ++item.m_col;
            FILETIME localFileTime;
            SYSTEMTIME localSystemTime;
            FileTimeToLocalFileTime(&pData->ftLastWriteTime, &localFileTime);
            FileTimeToSystemTime(&localFileTime, &localSystemTime);
            item.SetText(wxString::Format(_T("%d-%d-%d %d:%.2d"), 
                            localSystemTime.wYear,
                            localSystemTime.wMonth,
                            localSystemTime.wDay,
                            localSystemTime.wHour,
                            localSystemTime.wMinute));
            SetItem(item);
        }
    }

    //SortItems(m_sort_field, m_sort_forward);
}

const TString& CVirtualFileCtrl::GetCurrentDirectoryPath() const
{
    return m_strCurDirectoryPath;
}

int CVirtualFileCtrl::GetDriverIconByType(const char type) const
{
    int iRet = 0;
    switch (type)
    {
    case DRIVE_REMOVABLE:
        iRet = wxFileIconsTable::removeable;
        break;
    case DRIVE_CDROM:
        iRet = wxFileIconsTable::cdrom;
        break;
    case DRIVE_RAMDISK:
        iRet = wxFileIconsTable::floppy;
        break;
    case DRIVE_FIXED:
    case DRIVE_REMOTE:
    case DRIVE_UNKNOWN:
    case DRIVE_NO_ROOT_DIR:
    default:
        iRet = wxFileIconsTable::drive;
        break;
    }
    return iRet;
}

wxString CVirtualFileCtrl::GetBytesDisplayString(unsigned long long uSizeInBytes)
{
    wxString strResult = _T("UnKnown");
    static const unsigned long long uBytesOf10GB = (unsigned long long) 10 * 1024 * 1024 * 1024;
    if (uSizeInBytes > uBytesOf10GB)
    {
        strResult = wxString::Format(_T("%.1f GB"), (float)uSizeInBytes / uBytesOf10GB * 10);
    }
    else
    {
        strResult = wxString::Format(_T("%lld"), (unsigned long long)(ceilf((float)uSizeInBytes / 1024)));
        uint32_t uStrCounter = 0;
        uint32_t uStrLength = strResult.Length();
        while (uStrLength > uStrCounter + 3)
        {
            uStrCounter += 3;
            strResult.insert(uStrLength - uStrCounter, _T(","));
        }
        strResult.append(_T(" KB"));
    }
    return strResult;
}
