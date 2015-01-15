#ifndef BEATS_UTILITY_MATH_VECTOR4_H__INCLUDE
#define BEATS_UTILITY_MATH_VECTOR4_H__INCLUDE

class CVector4
{
public:
    CVector4(float value = 0.0f);
    CVector4(float x, float y, float z, float w);

    ~CVector4();

    float Length () const;
    float Distance(const CVector4& rhs) const;
    float Dot(const CVector4& vec) const;
    template<typename T>
    void ConvertTo(T& outResult) const
    {
        outResult.x = x;
        outResult.y = y;
        outResult.z = z;
        outResult.w = w;
    }

    CVector4& Normalize();
    bool IsZero();
    bool LongerThan(const CVector4& other);

    float& operator [] ( const uint32_t i );
    CVector4& operator = ( const CVector4& rkVector );
    CVector4& operator = ( const float value );
    bool operator == ( const CVector4& rkVector ) const;
    bool operator != ( const CVector4& rkVector ) const;
    CVector4 operator + ( const CVector4& rkVector ) const;
    CVector4& operator += ( const CVector4& rkVector );
    CVector4 operator - ( const CVector4& rkVector ) const;
    CVector4& operator -= ( const CVector4& rkVector );
    const CVector4& operator + () const;
    CVector4 operator - () const;
    CVector4 operator * ( const float fScalar ) const;
    CVector4 operator * ( const CVector4& rhs) const;
    CVector4 operator / ( const float fScalar ) const;
    CVector4 operator / ( const CVector4& rhs) const;
public:
    float x;
    float y;
    float z;
    float w;
};


#endif