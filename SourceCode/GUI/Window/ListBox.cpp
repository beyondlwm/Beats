#include "stdafx.h"
#include "ListBox.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "GUI/WindowManager.h"
#include "Render/TextureFragManager.h"
#include "GUI/Renderer/CheckBoxRenderer.h"
#include "GUI/Font/FontManager.h"
using namespace FCGUI;

ListBox::ListBox(const TString &name)
    : Window(name)
{
    Init();
}

bool ListBox::OnMouseEvent( MouseEvent *event )
{
    bool bRet = false;
    if( IsEnabled() )
    {
        if(event->Button() == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(event->Type() == EVENT_MOUSE_PRESSED)
            {
                kmVec3 pos;
                kmVec3Fill( &pos, event->X(),  event->Y(), 0.f );
                Window::WorldToLocal( pos );

                if(IsContainPos(pos))
                {
                    int currSel = (int) (pos.y / _itemHeight );
                    if( currSel != _currSel && currSel >= 0 &&  (size_t)currSel < _strList.size() )
                    {
                        _currSel = currSel;
                        WindowEvent event(EVENT_SELCHANGE);
                        DispatchEvent(&event);

                        ShowSelBackGround();
                    }
                }
            }
        }
        bRet = true;
    }
    return bRet;
}

size_t ListBox::GetCount( ) const
{
    return _strList.size();
}

kmScalar ListBox::SetItemHeight(kmScalar  itemHeight)
{
    kmScalar oldItemHeight = _itemHeight;
    _itemHeight = itemHeight;
    return oldItemHeight;
}

kmScalar ListBox::GetItemHeight( ) const
{
    return _itemHeight;
}

int ListBox::GetCurrSel( ) const
{
    return _currSel;
}

int ListBox::SetCurrSel( int select )
{
    int oldCurrSel = _currSel;
    _currSel = select;

    ShowSelBackGround();
    return oldCurrSel;
}

size_t ListBox::AddString( const TString &strText)
{
    _strList.push_back(strText);
    CaclItemHeight();
    return _strList.size();
}

void ListBox::DeleteString( size_t index )
{
    if( index < _strList.size() )
    {
        CaclItemHeight();
        _strList.erase(_strList.begin() + index);
    }
}

void ListBox::InsertString( int index, const TString &strText )
{
    _strList.insert( _strList.begin() + index, strText);
    CaclItemHeight();
}

const ListBox::StringList& ListBox::GetStringList() const
{
    return _strList;
}

kmScalar ListBox::GetPadding() const
{
    return _padding;
}

Window*  ListBox::GetSubSelWin()
{
    return _pSubSel;
}

TString ListBox::GetString(int index) const
{
    TString result = _T("");
    if( index >=0 && (size_t)index < _strList.size())
    {
        result = _strList[index];
    }
    return result;
}

void ListBox::Init()
{
    _itemHeight = 0.f;
    _currSel = -1;
    _padding =  3.f;
     _pSubSel = WindowManager::GetInstance()->Create<Window>(_T("SubSel"),this);
     _pFontFace  = FontManager::GetInstance()->GetFace(_T("STFANGSO_12"));
}

bool ListBox::IsContainPos(const kmVec3 &pos)
{
    kmVec2 size = RealSize();
    return (pos.x >=0 && pos.x <= size.x 
        && pos.y >=0 && pos.y <= size.y );
}

void ListBox::ShowSelBackGround()
{
    int count = _strList.size();
    float percent = (float)_currSel / (float)count;
    GetSubSelWin()->SetArea(0.f,  0.f,  0.f,  percent, 0.f, _itemHeight, 1.f, percent);
}

void ListBox::CaclItemHeight()
{
    int count = _strList.size();
    kmVec2 size = RealSize();
    _itemHeight = size.y / (float)count;
}

