#include "stdafx.h"
#include "Button.h"
#include "TextLabel.h"
#include "ListBox.h"
#include "ComboBox.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "GUI/WindowManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

using namespace FCGUI;

ComboBox::ComboBox(const TString &name)
    : Window(name)
{
    Init();
}

 ComboBox::ComboBox( CSerializer serializer )
     :Window( serializer )
 {
     Init();
     DECLARE_PROPERTY(serializer, _fDropDownButtonWidth, true, 0xFFFFFFFF, _T("下拉按钮宽度"), NULL, NULL, NULL);
     DECLARE_PROPERTY(serializer, _fListBoxHeight, true, 0xFFFFFFFF, _T("列表框高度"), NULL, NULL, NULL);
 }

ComboBox::~ComboBox()
{

}

void ComboBox::IntiChild()
{
    _pDropDwonButton = WindowManager::GetInstance()->Create<Button>(_T("SubDropDown"), this);
    _pDropDwonButton->SubscribeEvent(Button::EVENT_CLICKED, &ComboBox::OnDropDown, this);
    _pDropDwonButton->SetArea( - _fDropDownButtonWidth, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f);

    _pListBox = WindowManager::GetInstance()->Create<ListBox>(_T("SubListbox"), this);
    _pListBox->SetArea( 0.f, 0.f, 0.f, 1.f, 0.f, _fListBoxHeight, 1.f, 1.f );
    _pListBox->SubscribeEvent(ListBox::EVENT_SELCHANGE, &ComboBox::OnSelectOk, this);
    _pListBox->Hide();

    _pSelectItem = WindowManager::GetInstance()->Create<TextLabel>(_T("SubTextLabel"), this);
    _pSelectItem->SetArea( 0.f, 0.f, 0.f, 0.f, - _fDropDownButtonWidth, 0.f, 1.f, 1.f );
}

void ComboBox::Init()
{
    _fDropDownButtonWidth = 20.f;
    _fListBoxHeight = 100.f;
    IntiChild();
}

size_t ComboBox::AddString( const TString &strText)
{
    return _pListBox->AddString(strText);
}

int ComboBox::SetCurrSel( int select )
{
    return _pListBox->SetCurrSel(select);
}

void ComboBox::DeleteString( size_t index )
{
    return _pListBox->DeleteString(index);
}

 int ComboBox::GetCurrSel( ) const
 {
     return _pListBox->GetCurrSel();
 }

Button* ComboBox::GetDropDownButtton() const
{
    return _pDropDwonButton;
}

ListBox* ComboBox::GetListBox() const
{
    return _pListBox;
}

TextLabel* ComboBox::GetTextLabel() const
{
    return _pSelectItem;
}

void ComboBox::OnDropDown(BaseEvent *baseEvent)
{
    if(_pListBox->IsVisible())
    {
        _pListBox->Hide();
    }
    else
    {
         _pListBox->Show();
    }

    WindowEvent event(EVENT_DROPDOWN);
    DispatchEvent(&event);
}

void ComboBox::OnSelectOk(BaseEvent *baseEvent)
{
    _pListBox->Hide();

    int currSel = _pListBox->GetCurrSel();
    TString strText = _pListBox->GetString(currSel);
    _pSelectItem->SetText(strText);

    WindowEvent event(EVENT_SELECTOK);
    DispatchEvent(&event);
}
