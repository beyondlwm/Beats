#ifndef BEATS_UTILITY_MATH_MATRIX4_H__INCLUDE
#define BEATS_UTILITY_MATH_MATRIX4_H__INCLUDE

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix3.h"

class CMatrix4D
{
    friend class CTransform4D;

protected:

    float	n[4][4];

public:

    CMatrix4D() {}

    CMatrix4D(const CVector4& c1, const CVector4& c2, const CVector4& c3, const CVector4& c4);
    CMatrix4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33);

    CMatrix4D& Set(const CVector4& c1, const CVector4& c2, const CVector4& c3, const CVector4& c4);
    CMatrix4D& Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33);

    float& operator ()(long i, long j)
    {
        return (n[j][i]);
    }

    const float& operator ()(long i, long j) const
    {
        return (n[j][i]);
    }

    CVector4& operator [](long j)
    {
        return (*reinterpret_cast<CVector4 *>(&n[j][0]));
    }

    const CVector4& operator [](long j) const
    {
        return (*reinterpret_cast<const CVector4 *>(&n[j][0]));
    }

    CVector4 GetRow(long i) const
    {
        return (CVector4(n[0][i], n[1][i], n[2][i], n[3][i]));
    }

    CMatrix4D& operator =(const CMatrix3D& m);
    CMatrix4D& operator *=(const CMatrix4D& m);
    CMatrix4D& operator *=(const CMatrix3D& m);

    CMatrix4D operator *(const CMatrix4D& m) const;
    CMatrix4D operator *(const CMatrix3D& m) const;
    CVector4 operator *(const CVector4& v) const;
    CVector4 operator *(const CVector3& v) const;
    CVector4 operator *(const CVector2& v) const;

    bool operator ==(const CMatrix4D& m) const;
    bool operator !=(const CMatrix4D& m) const;

    CMatrix4D& SetIdentity(void);
    float Determinant() const;
    CMatrix4D Inverse() const;
    CMatrix4D Transpose() const;

    friend  CVector4 operator *(const CVector4& v, const CMatrix4D& m);
    friend  CVector4 operator *(const CVector3& v, const CMatrix4D& m);
    friend  CVector4 operator *(const CVector2& v, const CMatrix4D& m);
};


class CTransform4D : public CMatrix4D
{
public:

    CTransform4D() {}

    CTransform4D(const CMatrix3D& m);
    CTransform4D(const CMatrix3D& m, const CVector3& p);
    CTransform4D(const CVector3& c1, const CVector3& c2, const CVector3& c3, const CVector3& c4);
    CTransform4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23);

    CTransform4D& Set(const CMatrix3D& m, const CVector3& p);
    CTransform4D& Set(const CVector3& c1, const CVector3& c2, const CVector3& c3, const CVector3& c4);
    CTransform4D& Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23);

    CVector3& operator [](long j)
    {
        return (*reinterpret_cast<CVector3 *>(&n[j][0]));
    }

    const CVector3& operator [](long j) const
    {
        return (*reinterpret_cast<const CVector3 *>(&n[j][0]));
    }

    const CVector3& GetTranslation(void) const
    {
        return (*reinterpret_cast<const CVector3 *>(&n[3][0]));
    }

    CTransform4D& SetTranslation(const CVector3& p)
    {
        n[3][0] = p.x;
        n[3][1] = p.y;
        n[3][2] = p.z;
        return (*this);
    }

    CTransform4D& operator *=(const CTransform4D& m);
    CTransform4D& operator *=(const CMatrix3D& m);

    CTransform4D operator *(const CTransform4D& m) const;
    CMatrix4D operator *(const CMatrix4D& m) const;
    CTransform4D operator *(const CMatrix3D& m) const;
    CVector4 operator *(const CVector4& v) const;
    CVector3 operator *(const CVector3& v) const;
    CVector2 operator *(const CVector2& v) const;

    CMatrix3D GetMatrix3D(void) const;
    CTransform4D& SetMatrix3D(const CMatrix3D& m);
    CTransform4D& SetMatrix3D(const CTransform4D& m);
    CTransform4D& SetMatrix3D(const CVector3& c1, const CVector3& c2, const CVector3& c3);
    CTransform4D& SetMatrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);

    CTransform4D& SetRotationAboutX(float angle);
    CTransform4D& SetRotationAboutY(float angle);
    CTransform4D& SetRotationAboutZ(float angle);
    CTransform4D& SetRotationAboutAxis(float angle, const CVector3& axis);

    void GetEulerAngles(float *x, float *y, float *z) const;
    CTransform4D& SetEulerAngles(float x, float y, float z);

    CTransform4D& SetScale(float t);
    CTransform4D& SetScale(float r, float s, float t);

    float Determinant() const;
    CTransform4D Inverse() const;

    friend  CVector3 operator *(const CVector3& v, const CTransform4D& m);
    friend  CVector3 operator *(const CVector3& v, const CTransform4D& m);
    friend  CVector2 operator *(const CVector2& v, const CTransform4D& m);
    friend  CVector2 operator *(const CVector2& v, const CTransform4D& m);
};


#endif