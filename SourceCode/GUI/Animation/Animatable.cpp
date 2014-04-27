#include "stdafx.h"
#include "Animatable.h"

using namespace FCGUI;

Animatable::Animatable()
{

}

Animatable::~Animatable()
{
    for(auto prop : _props)
    {
        BEATS_SAFE_DELETE(prop.second);
    }
}

void Animatable::AddProp( int ID, const TString &name, 
                         const AnimProp::Setter &setter, const AnimProp::Getter &getter )
{
    BEATS_ASSERT(_props.find(ID) == _props.end());
    AnimProp *prop = new AnimProp(ID, name, setter, getter);
    _props[ID] = prop;
}

const AnimProp *Animatable::GetProp( int ID ) const
{
    auto itr = _props.find(ID);
    return itr != _props.end() ? itr->second : nullptr;
}
