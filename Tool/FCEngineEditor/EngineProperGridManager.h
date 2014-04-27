#ifndef FCENGINEEDITOR_ENGINEPROPERTYGRIDMANAGER_H__INCLUDE
#define FCENGINEEDITOR_ENGINEPROPERTYGRIDMANAGER_H__INCLUDE

#include <wx/propgrid/manager.h>

class CPropertyDescriptionBase;
class CComponentEditorProxy;
class CWxwidgetsPropertyBase;
class CComponentProjectDirectory;
class wxTreeItemId;
class wxTreeCtrl;

class EnginePropertyGirdManager : public wxPropertyGridManager
{
public:
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
    EnginePropertyGirdManager();
    virtual ~EnginePropertyGirdManager();
    virtual wxPropertyGrid* CreatePropertyGrid() const;

    void InsertInPropertyGrid(const std::vector<CPropertyDescriptionBase*>& pProperties, wxPGProperty* pParent = NULL);
    void InsertComponentsInPropertyGrid(CComponentEditorProxy* pComponent, wxPGProperty* pParent = NULL);
    void UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pPropertyBase);
    void OnComponentPropertyChanged(wxPropertyGridEvent& event);
    void OnComponentPropertyChangedImpl(wxPGProperty* pProperty);
    void SetUpdateFlag(bool bFlag);
    bool IsNeedUpdatePropertyGrid();
    wxPGProperty* GetPGPropertyByBase(CPropertyDescriptionBase* pBase);

private:
    bool m_bNeedUpdatePropertyGrid;

DECLARE_EVENT_TABLE()
};

#endif