#ifndef BEATS_COMPONENTS_DEPENDENCYDESCRIPTION_H__INCLUDE
#define BEATS_COMPONENTS_DEPENDENCYDESCRIPTION_H__INCLUDE

#include "expdef.h"

class CDependencyDescriptionLine;
class CComponentEditorProxy;
class TiXmlElement;

template class COMPONENTS_DLL_DECL std::allocator<CDependencyDescriptionLine*>;
template class COMPONENTS_DLL_DECL std::vector<CDependencyDescriptionLine*, std::allocator<CDependencyDescriptionLine*> >;

enum EDependencyType
{
    eDT_Strong,
    eDT_Weak,

    eDT_Count,
    eDT_Force32Bit = 0xFFFFFFFF
};

enum EDependencyChangeAction
{
    eDCA_Add,
    eDCA_Delete,
    eDCA_Change,

    eDCA_Count,
    eDCA_Force32Bit = 0xFFFFFFFF
};

class COMPONENTS_DLL_DECL CDependencyDescription
{
public:
    CDependencyDescription(EDependencyType type, size_t dependencyGuid, CComponentEditorProxy* pOwner, size_t uIndex, bool bIsList);
    virtual ~CDependencyDescription();

    CDependencyDescriptionLine* GetDependencyLine(size_t uIndex = 0) const;
    CDependencyDescriptionLine* SetDependency(size_t uIndex, CComponentEditorProxy* pComponent);
    size_t GetDependencyLineCount() const;
    void SetOwner(CComponentEditorProxy* pOwner);
    CComponentEditorProxy* GetOwner();
    size_t GetDependencyGuid() const;
    void Hide();
    void Show();
    bool IsVisible() const;
    bool IsListType() const;
    size_t GetIndex() const;

    const TCHAR* GetDisplayName();
    void SetDisplayName(const TCHAR* pszName);
    const TCHAR* GetVariableName();
    void SetVariableName(const TCHAR* pszName);
    CDependencyDescriptionLine* AddDependency(CComponentEditorProxy* pComponentInstance);
    void RemoveDependencyLine(CDependencyDescriptionLine* pComponentInstance);
    void RemoveDependencyByIndex(size_t uIndex);
    bool IsInDependency(CComponentEditorProxy* pComponentInstance);
    void SaveToXML(TiXmlElement* pParentNode);
    void LoadFromXML(TiXmlElement* pNode);
    EDependencyType GetType();
    size_t GetSelectedDependencyIndex() const;
    void SetSelectedDependencyIndex(size_t index);
    bool IsMatch(CComponentEditorProxy* pDependencyComponent);
    void Serialize(CSerializer& serializer);

    //Get the last dependency change action record.
    void GetCurrActionParam(EDependencyChangeAction& action, CComponentEditorProxy*& pProxy);

private:
    void OnDependencyChanged();

private:
    EDependencyType m_type;
    EDependencyChangeAction m_changeAction;
    CComponentEditorProxy* m_pChangeActionProxy;
    size_t m_uIndex;
    CComponentEditorProxy* m_pOwner;
    size_t m_uDependencyGuid;
    bool m_bHideRender;
    bool m_bIsListType;
    TString m_displayName;
    TString m_variableName;
    std::vector<CDependencyDescriptionLine*> m_dependencyLine;

};
#endif