#include "stdafx.h"
#include "Vector3.h"

CVector3::CVector3( float value )
: x(value)
, y(value)
, z(value)
{

}

CVector3::CVector3( float xx, float yy, float zz )
: x(xx)
, y(yy)
, z(zz)
{

}

CVector3::~CVector3()
{

}

float& CVector3::operator [] ( const size_t i )
{
    BEATS_ASSERT( i < 3 , _T("Index out of bound!"));

    return *(&x+i);
}

CVector3& CVector3::operator = ( const CVector3& rkVector )
{
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;

    return *this;
}

CVector3& CVector3::operator = ( const float value )
{
    x = value;
    y = value;
    z = value;
    return *this;
}

bool CVector3::operator == ( const CVector3& rkVector ) const
{
    return ( BEATS_FLOAT_EQUAL(x, rkVector.x) && BEATS_FLOAT_EQUAL(y, rkVector.y) && BEATS_FLOAT_EQUAL(z, rkVector.z) );
}

bool CVector3::operator != ( const CVector3& rkVector ) const
{
    return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
}

CVector3 CVector3::operator + ( const CVector3& rkVector ) const
{
    return CVector3(
        x + rkVector.x,
        y + rkVector.y,
        z + rkVector.z);
}

CVector3& CVector3::operator += ( const CVector3& rkVector )
{
    x += rkVector.x;
    y += rkVector.y;
    z += rkVector.z;

    return (*this);
}

CVector3 CVector3::operator - ( const CVector3& rkVector ) const
{
    return CVector3(
        x - rkVector.x,
        y - rkVector.y,
        z - rkVector.z);
}

CVector3& CVector3::operator-=( const CVector3& rkVector )
{
    x -= rkVector.x;
    y -= rkVector.y;
    z -= rkVector.z;

    return (*this);
}

CVector3 CVector3::operator * ( const float fScalar ) const
{
    return CVector3(
        x * fScalar,
        y * fScalar,
        z * fScalar);
}

CVector3 CVector3::operator * ( const CVector3& rhs) const
{
    return CVector3(
        x * rhs.x,
        y * rhs.y,
        z * rhs.z);
}

CVector3 CVector3::operator / ( const float fScalar ) const
{
    BEATS_ASSERT( fScalar != 0.0, _T("Can't divide zero!" ));
    float fInv = 1.0f / fScalar;
    return CVector3( x * fInv, y * fInv, z * fInv);
}

CVector3 CVector3::operator / ( const CVector3& rhs) const
{
    BEATS_ASSERT( rhs.x != 0.0, _T("Can't divide zero!" ));
    BEATS_ASSERT( rhs.y != 0.0, _T("Can't divide zero!" ));
    BEATS_ASSERT( rhs.z != 0.0, _T("Can't divide zero!" ));

    return CVector3( x / rhs.x, y / rhs.y,  z / rhs.z);
}

const CVector3& CVector3::operator + () const
{
    return *this;
}

CVector3 CVector3::operator - () const
{
    return CVector3(-x, -y, -z);
}

float CVector3::Length() const
{
    return sqrt( x * x + y * y + z * z );
}

bool CVector3::LongerThan( const CVector3& other )
{
    return ( x * x + y * y + z * z ) > ( other.x * other.x + other.y * other.y + other.z * other.z );
}

float CVector3::Distance( const CVector3& rhs ) const
{
    return (*this - rhs).Length();
}

float CVector3::Dot( const CVector3& vec ) const
{
    return x * vec.x + y * vec.y + z * vec.z;
}

CVector3& CVector3::Normalize()
{
    BEATS_ASSERT(!IsZero(), _T("Can't Normalize zero vector!" ));
    float fInvLength = 1.0f / Length();
    x *= fInvLength;
    y *= fInvLength;
    z *= fInvLength;
    return *this;
}

bool CVector3::IsZero()
{
    return BEATS_FLOAT_EQUAL(x, 0.f) && BEATS_FLOAT_EQUAL(y, 0.f) && BEATS_FLOAT_EQUAL(z, 0.f);
}

CVector3 CVector3::Cross( const CVector3& rkVector ) const
{
    CVector3 result(
        y * rkVector.z - z * rkVector.y,
        z * rkVector.x - x * rkVector.z,
        x * rkVector.y - y * rkVector.x);
    result.Normalize();
    return result;
}


