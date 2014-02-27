#ifndef BEATS_UTILITY_MATH_MATRIX3_H__INCLUDE
#define BEATS_UTILITY_MATH_MATRIX3_H__INCLUDE

#include "Vector3.h"

class CMatrix3D
{
    friend class CMatrix4D;
    friend class CTransform4D;

private:

    float	n[3][3];

public:

    CMatrix3D() {}

    CMatrix3D(const CVector3& c1, const CVector3& c2, const CVector3& c3);
    CMatrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);

    CMatrix3D& Set(const CVector3& c1, const CVector3& c2, const CVector3& c3);
    CMatrix3D& Set(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);

    float& operator ()(long i, long j)
    {
        return (n[j][i]);
    }

    const float& operator ()(long i, long j) const
    {
        return (n[j][i]);
    }

    CVector3& operator [](long j)
    {
        return (*reinterpret_cast<CVector3 *>(&n[j][0]));
    }

    const CVector3& operator [](long j) const
    {
        return (*reinterpret_cast<const CVector3 *>(&n[j][0]));
    }

    CVector3 GetRow(long i) const
    {
        return (CVector3(n[0][i], n[1][i], n[2][i]));
    }

    CMatrix3D& operator *=(const CMatrix3D& m);
    CMatrix3D& operator *=(float t);
    CMatrix3D& operator /=(float t);

    CMatrix3D operator *(const CMatrix3D& m) const;
    CMatrix3D operator *(float t) const;
    CMatrix3D operator /(float t) const;

    CVector3 operator *(const CVector3& v) const;

    bool operator ==(const CMatrix3D& m) const;
    bool operator !=(const CMatrix3D& m) const;

    CMatrix3D& SetIdentity(void);

    CMatrix3D& SetRotationAboutX(float angle);
    CMatrix3D& SetRotationAboutY(float angle);
    CMatrix3D& SetRotationAboutZ(float angle);
    CMatrix3D& SetRotationAboutAxis(float angle, const CVector3& axis);

    void GetEulerAngles(float *x, float *y, float *z) const;
    CMatrix3D& SetEulerAngles(float x, float y, float z);

    CMatrix3D& SetScale(float t);
    CMatrix3D& SetScale(float r, float s, float t);

    CMatrix3D& Orthonormalize(void);
    float Determinant(const CMatrix3D& m);
    CMatrix3D Inverse(const CMatrix3D& m);
    CMatrix3D Transpose(const CMatrix3D& m);

    friend  CVector3 operator *(const CVector3& v, const CMatrix3D& m);
};

#endif