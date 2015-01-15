#ifndef BEATS_UTILITY_MATH_VECTOR2_H__INCLUDE
#define BEATS_UTILITY_MATH_VECTOR2_H__INCLUDE

class CVector2
{
public:
    CVector2(float value = 0.0f);
    CVector2(float xx, float yy);

    ~CVector2();

    float Length () const;
    float Distance(const CVector2& rhs) const;
    float Dot(const CVector2& vec) const;
    template<typename T>
    void ConvertTo(T& outResult) const
    {
        outResult.x = x;
        outResult.y = y;
    }

    CVector2& Normalize();
    bool IsZero();
    bool LongerThan(const CVector2& other);

    float& operator [] ( const uint32_t i );
    CVector2& operator = ( const CVector2& rkVector );
    CVector2& operator = ( const float value );
    bool operator == ( const CVector2& rkVector ) const;
    bool operator != ( const CVector2& rkVector ) const;
    CVector2 operator + ( const CVector2& rkVector ) const;
    CVector2& operator += ( const CVector2& rkVector );
    CVector2 operator - ( const CVector2& rkVector ) const;
    CVector2& operator -= ( const CVector2& rkVector );
    const CVector2& operator + () const;
    CVector2 operator - () const;
    CVector2 operator * ( const float fScalar ) const;
    CVector2 operator * ( const CVector2& rhs) const;
    CVector2 operator / ( const float fScalar ) const;
    CVector2 operator / ( const CVector2& rhs) const;
public:
    float x;
    float y;
};


#endif