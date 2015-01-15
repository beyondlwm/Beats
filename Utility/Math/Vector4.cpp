#include "stdafx.h"
#include "Vector4.h"

CVector4::CVector4( float value )
: x(value)
, y(value)
, z(value)
, w(value)
{

}

CVector4::CVector4( float x, float y, float z, float w )
: x(x)
, y(y)
, z(z)
, w(w)
{

}

CVector4::~CVector4()
{

}

float& CVector4::operator [] ( const uint32_t i )
{
    BEATS_ASSERT( i < 4 , _T("Index out of bound!"));

    return *(&x+i);
}

CVector4& CVector4::operator = ( const CVector4& rkVector )
{
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;
    w = rkVector.w;

    return *this;
}

CVector4& CVector4::operator = ( const float value )
{
    x = value;
    y = value;
    z = value;
    w = value;
    return *this;
}

bool CVector4::operator == ( const CVector4& rkVector ) const
{
    return ( BEATS_FLOAT_EQUAL(x, rkVector.x) && BEATS_FLOAT_EQUAL(y, rkVector.y) && BEATS_FLOAT_EQUAL(z, rkVector.z) && BEATS_FLOAT_EQUAL(w, rkVector.w));
}

bool CVector4::operator != ( const CVector4& rkVector ) const
{
    return ( x != rkVector.x || y != rkVector.y || z != rkVector.z || w != rkVector.w );
}

CVector4 CVector4::operator + ( const CVector4& rkVector ) const
{
    return CVector4(
        x + rkVector.x,
        y + rkVector.y,
        z + rkVector.z,
        w + rkVector.w);
}

CVector4& CVector4::operator+=( const CVector4& rkVector )
{
    x += rkVector.x;
    y += rkVector.y;
    z += rkVector.z;
    w += rkVector.w;

    return (*this);
}

CVector4 CVector4::operator - ( const CVector4& rkVector ) const
{
    return CVector4(
        x - rkVector.x,
        y - rkVector.y,
        z - rkVector.z,
        w - rkVector.w);
}

CVector4& CVector4::operator-=( const CVector4& rkVector )
{
    x -= rkVector.x;
    y -= rkVector.y;
    z -= rkVector.z;
    w -= rkVector.w;

    return (*this);
}

CVector4 CVector4::operator * ( const float fScalar ) const
{
    return CVector4(
        x * fScalar,
        y * fScalar,
        z * fScalar,
        w * fScalar);
}

CVector4 CVector4::operator * ( const CVector4& rhs) const
{
    return CVector4(
        x * rhs.x,
        y * rhs.y,
        z * rhs.z,
        w * rhs.w);
}

CVector4 CVector4::operator / ( const float fScalar ) const
{
    BEATS_ASSERT( fScalar != 0.0, _T("Can't divide zero!" ));
    float fInv = 1.0f / fScalar;
    return CVector4( x * fInv, y * fInv, z * fInv, w * fInv);
}

CVector4 CVector4::operator / ( const CVector4& rhs) const
{
    BEATS_ASSERT( rhs.x != 0.0, _T("Can't divide zero!" ));
    BEATS_ASSERT( rhs.y != 0.0, _T("Can't divide zero!" ));
    BEATS_ASSERT( rhs.z != 0.0, _T("Can't divide zero!" ));
    BEATS_ASSERT( rhs.w != 0.0, _T("Can't divide zero!" ));

    return CVector4( x / rhs.x, y / rhs.y,  z / rhs.z, w / rhs.w);
}

const CVector4& CVector4::operator + () const
{
    return *this;
}

CVector4 CVector4::operator - () const
{
    return CVector4(-x, -y, -z, -w);
}

float CVector4::Length() const
{
    return sqrt( x * x + y * y + z * z + w * w );
}

bool CVector4::LongerThan( const CVector4& other )
{
    return ( x * x + y * y + z * z + w * w ) > ( other.x * other.x + other.y * other.y + other.z * other.z + other.w * other.w );
}

float CVector4::Distance( const CVector4& rhs ) const
{
    return (*this - rhs).Length();
}

float CVector4::Dot( const CVector4& vec ) const
{
    return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
}

CVector4& CVector4::Normalize()
{
    BEATS_ASSERT(!IsZero(), _T("Can't Normalize zero vector!" ));
    float fInvLength = 1.0f / Length();
    x *= fInvLength;
    y *= fInvLength;
    z *= fInvLength;
    w *= fInvLength;
    return *this;
}

bool CVector4::IsZero()
{
    return BEATS_FLOAT_EQUAL(x, 0.f) && BEATS_FLOAT_EQUAL(y, 0.f) && BEATS_FLOAT_EQUAL(z, 0.f) && BEATS_FLOAT_EQUAL(w, 0.f);
}



