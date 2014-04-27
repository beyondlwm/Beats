#ifndef FCENGINEEDITOR_COMPONENTFILETREEITEMDATA_H__INCLUDE
#define FCENGINEEDITOR_COMPONENTFILETREEITEMDATA_H__INCLUDE

#include <wx/treectrl.h>
class CComponentProjectDirectory;

class CComponentFileTreeItemData : public wxTreeItemData
{
public:
    CComponentFileTreeItemData(CComponentProjectDirectory* pData, const TString& fileName);
    virtual ~CComponentFileTreeItemData();
    bool IsDirectory();
    const TString& GetFileName() const;
    CComponentProjectDirectory* GetProjectDirectory() const;
    void SetData(CComponentProjectDirectory* pData);
private:
    TString m_fileName;
    CComponentProjectDirectory* m_pData;
};

#endif