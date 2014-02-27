#ifndef BEATS_UTILITY_MATH_VECTOR3_H__INCLUDE
#define BEATS_UTILITY_MATH_VECTOR3_H__INCLUDE

class CVector3
{
public:
    CVector3(float value = 0.0f);
    CVector3(float xx, float yy, float zz);

    ~CVector3();

    float Length () const;
    float Distance(const CVector3& rhs) const;
    float Dot(const CVector3& vec) const;
    CVector3 Cross( const CVector3& rkVector ) const;
    template<typename T>
    void ConvertTo(T& outResult) const
    {
        outResult.x = x;
        outResult.y = y;
        outResult.z = z;
    }

    CVector3& Normalize();
    bool IsZero();
    bool LongerThan(const CVector3& other);

    float& operator [] ( const size_t i );
    CVector3& operator = ( const CVector3& rkVector );
    CVector3& operator = ( const float value );
    bool operator == ( const CVector3& rkVector ) const;
    bool operator != ( const CVector3& rkVector ) const;
    CVector3 operator + ( const CVector3& rkVector ) const;
    CVector3& operator += ( const CVector3& rkVector );
    CVector3 operator - ( const CVector3& rkVector ) const;
    CVector3& operator -= ( const CVector3& rkVector );
    const CVector3& operator + () const;
    CVector3 operator - () const;
    CVector3 operator * ( const float fScalar ) const;
    CVector3 operator * ( const CVector3& rhs) const;
    CVector3 operator / ( const float fScalar ) const;
    CVector3 operator / ( const CVector3& rhs) const;
public:
    float x;
    float y;
    float z;
};


#endif