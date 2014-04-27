#include "stdafx.h"
#include "KeyFrame.h"

using namespace FCGUI;

KeyFrame::KeyFrame(kmScalar position)
    : _position(position)
{

}

kmScalar KeyFrame::Position() const
{
    return _position;
}

void KeyFrame::SetValueCount(size_t size)
{
    _values.resize(size);
}

size_t KeyFrame::ValueCount() const
{
    return _values.size();
}

void KeyFrame::SetValue(size_t index, kmScalar value)
{
    BEATS_ASSERT(index < _values.size());
    _values[index] = value;
}

void KeyFrame::DeleteValue(size_t index)
{
    _values.erase(_values.begin() + index);
}

kmScalar KeyFrame::Value(size_t index) const
{
    BEATS_ASSERT(index < _values.size());
    return _values[index];
}

const KeyFrame::ValueList &KeyFrame::Values() const
{
    return _values;
}