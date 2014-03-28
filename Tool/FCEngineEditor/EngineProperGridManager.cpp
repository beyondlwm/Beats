#include "stdafx.h"
#include "mainapp.h"
#include "EngineProperGridManager.h"
#include "EnginePropertyGrid.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentEditorProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
#include "wxWidgetsPropertyBase.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"
#include "ComponentGraphics_GL.h"
#include "resource/ResourcePathManager.h"

EnginePropertyGirdManager::EnginePropertyGirdManager()
    : m_bNeedUpdatePropertyGrid(false)
{
    
}

EnginePropertyGirdManager::~EnginePropertyGirdManager()
{
}

wxPropertyGrid* EnginePropertyGirdManager::CreatePropertyGrid() const
{
    EnginePropertyGrid* pGrid = new EnginePropertyGrid();
    pGrid->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_ALWAYS);
    pGrid->SetManager(const_cast<EnginePropertyGirdManager*>(this));
    return pGrid;
}
    

void EnginePropertyGirdManager::InsertComponentsInPropertyGrid( CComponentEditorProxy* pComponent, wxPGProperty* pParent /*= NULL*/ )
{
    // Set Guid property.
    wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
    pPGProperty->SetValue(wxString::Format(_T("0x%x"), pComponent->GetGuid()));
    pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
    pPGProperty->SetName(_T("Guid"));
    pPGProperty->SetLabel(_T("Guid"));
    GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
    // Set Property for Id
    if (pComponent->GetId() != -1)
    {
        wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
        pPGProperty->SetValue(wxString::Format(_T("%d"), pComponent->GetId()));
        pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
        pPGProperty->SetName(_T("Id"));
        pPGProperty->SetLabel(_T("Id"));
        GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
    }
    InsertInPropertyGrid(*pComponent->GetPropertyPool(), pParent);
}

void EnginePropertyGirdManager::InsertInPropertyGrid( const std::vector<CPropertyDescriptionBase*>& pProperties, wxPGProperty* pParent /*= NULL*/ )
{
    for (size_t i = 0; i < pProperties.size(); ++i)
    {
        CWxwidgetsPropertyBase* pPropertyBase = const_cast<CWxwidgetsPropertyBase*>(static_cast<CWxwidgetsPropertyBase*>(pProperties[i]));
        wxPGProperty* pPGProperty = pPropertyBase->CreateWxProperty();
        BEATS_ASSERT(pPGProperty != NULL);
        BEATS_ASSERT(pParent != pPGProperty, _T("Can't insert a property in itself"));

        pPGProperty->SetName(pPropertyBase->GetBasicInfo().m_variableName);
        pPGProperty->SetLabel(pPropertyBase->GetBasicInfo().m_displayName);
        pPGProperty->ChangeFlag(wxPG_PROP_READONLY, !pPropertyBase->GetBasicInfo().m_bEditable || pPropertyBase->GetType() == ePT_Ptr);
        pPGProperty->SetHelpString(pPropertyBase->GetBasicInfo().m_tip);
        GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
        // This function can only be called after property be inserted to grid, or it will crash. It's a wxwidgets rule.
        pPGProperty->SetBackgroundColour(pPropertyBase->GetBasicInfo().m_color);
        InsertInPropertyGrid(pPropertyBase->GetChildren(), pPGProperty);
    }
    // Update the visibility after all the properties have been inserted in the grid.
    for (size_t i = 0; i < pProperties.size(); ++i)
    {
        UpdatePropertyVisiblity(static_cast<CWxwidgetsPropertyBase*>(pProperties[i]));
    }
    Refresh();
}

void EnginePropertyGirdManager::UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pPropertyBase)
{
    // Do visible check logic.
    std::set<CWxwidgetsPropertyBase*>& effectProperties = pPropertyBase->GetEffectProperties();
    std::set<CWxwidgetsPropertyBase*>::iterator iter = effectProperties.begin();
    for (; iter != effectProperties.end(); ++iter)
    {
        bool bShouldBeVisible = (*iter)->CheckVisibleTrigger();
        wxPGProperty* pPGProperty = GetPGPropertyByBase(*iter);
        BEATS_ASSERT(pPGProperty != NULL, _T("Can't find property %s in data grid!"), (*iter)->GetBasicInfo().m_displayName.c_str());
        if (pPGProperty != NULL)
        {
            pPGProperty->Hide(!bShouldBeVisible);
        }
    }
}

wxPGProperty* EnginePropertyGirdManager::GetPGPropertyByBase(CPropertyDescriptionBase* pBase)
{
    wxPGProperty* pRet = NULL;
    for (wxPropertyGridIterator it = GetCurrentPage()->GetIterator(); !it.AtEnd(); ++it)
    {
        if (it.GetProperty()->GetClientData() == pBase)
        {
            pRet = it.GetProperty();
            break;
        }
    }
    return pRet;
}

void EnginePropertyGirdManager::RequestToUpdatePropertyGrid( )
{
    if (!m_bNeedUpdatePropertyGrid)
    {
        m_bNeedUpdatePropertyGrid = true;
    }
}

CComponentEditorProxy* CreateComponentProxy(CComponentGraphic* pGraphics, size_t guid, size_t parentGuid, TCHAR* pszClassName)
{
    return new CComponentEditorProxy(pGraphics, guid, parentGuid, pszClassName);
}

CComponentGraphic* CreateGraphic()
{
    CComponentGraphic_GL* pGraphics = new CComponentGraphic_GL();
    return pGraphics;
}

void EnginePropertyGirdManager::InitComponentsPage()
{
    const TString& strWorkPath = CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Work);
    CComponentManager::GetInstance()->DeserializeTemplateData(strWorkPath.c_str(), CreateComponentProxy, CreateGraphic);
    const std::map<size_t, CComponentBase*>* pComponentsMap = CComponentManager::GetInstance()->GetComponentTemplateMap();

    std::map<size_t, CComponentBase*>::const_iterator iter =  pComponentsMap->find(0x1547ABEC);
    CComponentEditorProxy* pTemp = (CComponentEditorProxy*)iter->second;
    InsertComponentsInPropertyGrid(pTemp);
}
