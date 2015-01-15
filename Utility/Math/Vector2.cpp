#include "stdafx.h"
#include "Vector2.h"

CVector2::CVector2( float value )
: x(value)
, y(value)
{

}

CVector2::CVector2( float xx, float yy )
: x(xx)
, y(yy)
{

}

CVector2::~CVector2()
{

}

float& CVector2::operator [] ( const uint32_t i )
{
    BEATS_ASSERT( i < 2 , _T("Index out of bound!"));

    return *(&x+i);
}

CVector2& CVector2::operator = ( const CVector2& rkVector )
{
    x = rkVector.x;
    y = rkVector.y;

    return *this;
}

CVector2& CVector2::operator = ( const float value )
{
    x = value;
    y = value;
    return *this;
}

bool CVector2::operator == ( const CVector2& rkVector ) const
{
    return ( BEATS_FLOAT_EQUAL(x, rkVector.x) && BEATS_FLOAT_EQUAL(y, rkVector.y));
}

bool CVector2::operator != ( const CVector2& rkVector ) const
{
    return ( x != rkVector.x || y != rkVector.y );
}

CVector2 CVector2::operator + ( const CVector2& rkVector ) const
{
    return CVector2(
        x + rkVector.x,
        y + rkVector.y);
}

CVector2& CVector2::operator+=( const CVector2& rkVector )
{
    x += rkVector.x;
    y += rkVector.y;

    return (*this);
}

CVector2 CVector2::operator - ( const CVector2& rkVector ) const
{
    return CVector2(
        x - rkVector.x,
        y - rkVector.y);
}

CVector2& CVector2::operator-=( const CVector2& rkVector )
{
    x -= rkVector.x;
    y -= rkVector.y;

    return (*this);
}

CVector2 CVector2::operator * ( const float fScalar ) const
{
    return CVector2(
        x * fScalar,
        y * fScalar);
}

CVector2 CVector2::operator * ( const CVector2& rhs) const
{
    return CVector2(
        x * rhs.x,
        y * rhs.y);
}

CVector2 CVector2::operator / ( const float fScalar ) const
{
    BEATS_ASSERT( fScalar != 0.0, _T("Can't divide zero!" ));
    float fInv = 1.0f / fScalar;
    return CVector2( x * fInv, y * fInv);
}

CVector2 CVector2::operator / ( const CVector2& rhs) const
{
    BEATS_ASSERT( rhs.x != 0.0, _T("Can't divide zero!" ));
    BEATS_ASSERT( rhs.y != 0.0, _T("Can't divide zero!" ));

    return CVector2( x / rhs.x, y / rhs.y);
}

const CVector2& CVector2::operator + () const
{
    return *this;
}

CVector2 CVector2::operator - () const
{
    return CVector2(-x, -y);
}

float CVector2::Length() const
{
    return sqrt( x * x + y * y );
}

bool CVector2::LongerThan( const CVector2& other )
{
    return ( x * x + y * y ) > ( other.x * other.x + other.y * other.y );
}

float CVector2::Distance( const CVector2& rhs ) const
{
    return (*this - rhs).Length();
}

float CVector2::Dot( const CVector2& vec ) const
{
    return x * vec.x + y * vec.y ;
}

CVector2& CVector2::Normalize()
{
    BEATS_ASSERT(!IsZero(), _T("Can't Normalize zero vector!" ));
    float fInvLength = 1.0f / Length();
    x *= fInvLength;
    y *= fInvLength;
    return *this;
}

bool CVector2::IsZero()
{
    return BEATS_FLOAT_EQUAL(x, 0.f) && BEATS_FLOAT_EQUAL(y, 0.f);
}
