#ifndef FCENGINEEDITOR_COMPONENTTREEITEMDATA_H__INCLUDE
#define FCENGINEEDITOR_COMPONENTTREEITEMDATA_H__INCLUDE

#include <wx/treectrl.h>

class CComponentTreeItemData : public wxTreeItemData
{
public:
    CComponentTreeItemData(bool bIsDirectory, size_t guid);
    virtual ~CComponentTreeItemData();

    bool IsDirectory();
    size_t GetGUID();
private:
    bool m_bIsDirectory;
    size_t m_componentGUID;
};

#endif