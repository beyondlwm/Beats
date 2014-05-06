#include "stdafx.h"
#include "wxPtrButtonEditor.h"
#include "../../Components/Component/ComponentProxyManager.h"
#include "../../Components/Component/ComponentEditorProxy.h"
#include "../../DebugTool/BDTWxApp.h"
#include "../../DebugTool/BDTWxFrame.h"
#include "../../Utility/Serializer/Serializer.h"
#include "../wxWidgetsProperty/PtrPropertyDescription.h"
#include "../wxWidgetsProperty/ListPropertyDescriptionEx.h"
#include "../wxWidgetsProperty/MapPropertyDescription.h"

IMPLEMENT_DYNAMIC_CLASS(wxPtrButtonEditor, wxPGTextCtrlEditor)

wxPtrButtonEditor::wxPtrButtonEditor()
{

}

wxPtrButtonEditor::~wxPtrButtonEditor()
{

}

wxPGWindowList wxPtrButtonEditor::CreateControls( wxPropertyGrid* propGrid,
                                                 wxPGProperty* property,
                                                 const wxPoint& pos,
                                                 const wxSize& sz ) const
{
    wxPGMultiButton* buttons = new wxPGMultiButton( propGrid, sz );
    void* pClientData = property->GetClientData();
    BEATS_ASSERT(pClientData != NULL);
    CPropertyDescriptionBase* pPropertyDescription = static_cast<CPropertyDescriptionBase*>(pClientData);
    if (pPropertyDescription->GetType() == eRPT_Ptr)
    {
        CPtrPropertyDescription* pPtrProperty = static_cast<CPtrPropertyDescription*>(pPropertyDescription);
        buttons->Add( pPtrProperty->GetInstanceComponent() == NULL ? _T("+") : _T("-"));
    }
    else if (pPropertyDescription->IsContainerProperty())
    {
        buttons->Add(_T("+"));
        if (property->GetChildCount() > 0)
        {
            buttons->Add(_T("-"));
        }
    }
    CPropertyDescriptionBase* pParent = pPropertyDescription->GetParent();
    // it is a list element.
    if (pParent && pParent->IsContainerProperty())
    {
        buttons->Add(_T("x"));
    }
    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls( propGrid, property, pos, buttons->GetPrimarySize() );

    buttons->Finalize(propGrid, pos);

    wndList.SetSecondary( buttons );
    return wndList;
}

bool wxPtrButtonEditor::OnEvent( wxPropertyGrid* propGrid,
                                wxPGProperty* property,
                                wxWindow* ctrl,
                                wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        wxPGMultiButton* buttons = (wxPGMultiButton*) propGrid->GetEditorControlSecondary();
        CWxwidgetsPropertyBase* pPropertyDescription = static_cast<CWxwidgetsPropertyBase*>(property->GetClientData());
        CBDTWxFrame* pMainFrame = static_cast<CBDTWxApp*>(wxApp::GetInstance())->GetMainFrame();
        const wxWindow* pButton = NULL;
        for (size_t i = 0; i < buttons->GetCount(); ++i)
        {
            if (event.GetId() == buttons->GetButtonId(i))
            {
                pButton = buttons->GetButton(i);
                break;
            }
        }
        if (pButton != NULL)
        {
            if (pPropertyDescription->GetType() == eRPT_Ptr)
            {
                CPtrPropertyDescription* pPtrPropertyDescription = static_cast<CPtrPropertyDescription*>(pPropertyDescription);
                wxString valueStr = property->GetValueAsString();
                bool bValueChanged = false;
                if (pButton->GetLabel().CmpNoCase(_T("+")) == 0)
                {
                    size_t ptrGuid = pPtrPropertyDescription->GetPtrGuid();
                    std::vector<size_t> derivedClassGuid;
                    CComponentProxyManager::GetInstance()->QueryDerivedClass(ptrGuid, derivedClassGuid, true);

                    CComponentBase* pBase = CComponentProxyManager::GetInstance()->GetComponentTemplate(ptrGuid);
                    bValueChanged = derivedClassGuid.size() == 0;
                    if (!bValueChanged)
                    {
                        wxPGChoices choice;
                        if (pBase != NULL)
                        {
                            choice.Add(pBase->GetClassStr(), pBase->GetGuid());
                        }
                        for (auto i : derivedClassGuid)
                        {
                            pBase = CComponentProxyManager::GetInstance()->GetComponentTemplate(i);
                            choice.Add(pBase->GetClassStr(), pBase->GetGuid());
                        }
                        wxString strSelectItem = ::wxGetSingleChoice(wxT("TypeChoice"), wxT("Caption"), choice.GetLabels(),
                            NULL, wxDefaultCoord, wxDefaultCoord, false, wxCHOICE_WIDTH, wxCHOICE_HEIGHT);
                        if ( !strSelectItem.empty() )
                        {
                            int nSelectIndex = choice.Index(strSelectItem);
                            size_t uDerivedGuid = choice.GetValue(nSelectIndex);
                            pPtrPropertyDescription->SetDerivedGuid(uDerivedGuid);
                            bValueChanged = true;
                        }
                    }
                    if (bValueChanged)
                    {
                        bool bCreateInstance = pPtrPropertyDescription->CreateInstance();
                        BEATS_ASSERT(bCreateInstance);
                        CComponentEditorProxy* pCompBase = static_cast<CComponentEditorProxy*>(pPtrPropertyDescription->GetInstanceComponent());
                        BEATS_ASSERT(pCompBase != NULL);
                        BEATS_ASSERT(pMainFrame != NULL);
                        TString* pStrValue = (TString*)pPtrPropertyDescription->GetValue(eVT_CurrentValue);
                        property->SetValueFromString(pStrValue->c_str());
                        pMainFrame->InsertComponentsInPropertyGrid(pCompBase, property);
                        buttons->GetButton(0)->SetLabel(_T("-"));
                        valueStr.insert(0, _T("+"));
                    }
                }
                else if (pButton->GetLabel().CmpNoCase(_T("-")) == 0)
                {
                    property->DeleteChildren();
                    bool bDeleteInstance = pPtrPropertyDescription->DestroyInstance();
                    // Destroy instance may cause the value changed, so we fetch it again.
                    char szTmp[MAX_PATH];
                    pPtrPropertyDescription->GetValueAsChar(eVT_CurrentValue, szTmp);
                    valueStr = szTmp;
                    pPtrPropertyDescription->GetChildren().clear();
                    BEATS_ASSERT(bDeleteInstance);
                    buttons->GetButton(0)->SetLabel(_T("+"));
                    valueStr.Remove(0, 1); // Remove the "+" prefix
                    bValueChanged = true;
                }
                if (bValueChanged)
                {
                    wxVariant newValue(valueStr);
                    pPtrPropertyDescription->SetValue(newValue, false);
                    property->SetValue(newValue);
                    propGrid->Refresh();
                }
            }
            if (pPropertyDescription->IsContainerProperty())
            {
                if (pButton->GetLabel().CmpNoCase(_T("+")) == 0)
                {
                    CPropertyDescriptionBase* pNewChild = pPropertyDescription->AddChild(NULL);
                    if (pNewChild != NULL)
                    {
                        std::vector<CPropertyDescriptionBase*> value;
                        value.push_back(pNewChild);
                        pMainFrame->InsertInPropertyGrid(value, property);
                    }
                }
                else if (pButton->GetLabel().CmpNoCase(_T("-")) == 0)
                {
                    pPropertyDescription->DeleteAllChild();
                    property->DeleteChildren();
                }
                char valueStr[256];
                pPropertyDescription->GetValueAsChar(eVT_CurrentValue, valueStr);
                property->SetValue(valueStr);
                property->SetModifiedStatus(pPropertyDescription->GetChildrenCount() > 0);
                property->RecreateEditor();
            }

            if (pButton->GetLabel().CmpNoCase(_T("x")) == 0)
            {
                BEATS_ASSERT(pPropertyDescription->GetParent() != NULL);
                CPropertyDescriptionBase* pParent = pPropertyDescription->GetParent();
                pParent->DeleteChild(pPropertyDescription);
                // NOTICE: We have deleted this already!
                pPropertyDescription = NULL;

                property->SetClientData(NULL);
                char valueStr[256];
                pParent->GetValueAsChar(eVT_CurrentValue, valueStr);
                property->GetParent()->SetValue(valueStr);
                //TODO: I can't refresh property here, because we are trying to delete property of which callback we are in.
                pMainFrame->RequestToUpdatePropertyGrid();
            }
            return true;
        }
    }
    return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);
}