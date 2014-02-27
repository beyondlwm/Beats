#include "stdafx.h"
#include "Matrix4.h"

CMatrix4D::CMatrix4D(const CVector4& c1, const CVector4& c2, const CVector4& c3, const CVector4& c4)
{
    n[0][0] = c1.x;
    n[0][1] = c1.y;
    n[0][2] = c1.z;
    n[0][3] = c1.w;
    n[1][0] = c2.x;
    n[1][1] = c2.y;
    n[1][2] = c2.z;
    n[1][3] = c2.w;
    n[2][0] = c3.x;
    n[2][1] = c3.y;
    n[2][2] = c3.z;
    n[2][3] = c3.w;
    n[3][0] = c4.x;
    n[3][1] = c4.y;
    n[3][2] = c4.z;
    n[3][3] = c4.w;
}

CMatrix4D::CMatrix4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33)
{
    n[0][0] = n00;
    n[1][0] = n01;
    n[2][0] = n02;
    n[3][0] = n03;
    n[0][1] = n10;
    n[1][1] = n11;
    n[2][1] = n12;
    n[3][1] = n13;
    n[0][2] = n20;
    n[1][2] = n21;
    n[2][2] = n22;
    n[3][2] = n23;
    n[0][3] = n30;
    n[1][3] = n31;
    n[2][3] = n32;
    n[3][3] = n33;
}

CMatrix4D& CMatrix4D::Set(const CVector4& c1, const CVector4& c2, const CVector4& c3, const CVector4& c4)
{
    n[0][0] = c1.x;
    n[0][1] = c1.y;
    n[0][2] = c1.z;
    n[0][3] = c1.w;
    n[1][0] = c2.x;
    n[1][1] = c2.y;
    n[1][2] = c2.z;
    n[1][3] = c2.w;
    n[2][0] = c3.x;
    n[2][1] = c3.y;
    n[2][2] = c3.z;
    n[2][3] = c3.w;
    n[3][0] = c4.x;
    n[3][1] = c4.y;
    n[3][2] = c4.z;
    n[3][3] = c4.w;

    return (*this);
}

CMatrix4D& CMatrix4D::Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33)
{
    n[0][0] = n00;
    n[1][0] = n01;
    n[2][0] = n02;
    n[3][0] = n03;
    n[0][1] = n10;
    n[1][1] = n11;
    n[2][1] = n12;
    n[3][1] = n13;
    n[0][2] = n20;
    n[1][2] = n21;
    n[2][2] = n22;
    n[3][2] = n23;
    n[0][3] = n30;
    n[1][3] = n31;
    n[2][3] = n32;
    n[3][3] = n33;

    return (*this);
}

CMatrix4D& CMatrix4D::operator =(const CMatrix3D& m)
{
    n[0][0] = m.n[0][0];
    n[0][1] = m.n[0][1];
    n[0][2] = m.n[0][2];
    n[1][0] = m.n[1][0];
    n[1][1] = m.n[1][1];
    n[1][2] = m.n[1][2];
    n[2][0] = m.n[2][0];
    n[2][1] = m.n[2][1];
    n[2][2] = m.n[2][2];

    n[0][3] = n[1][3] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;
    n[3][3] = 1.0F;

    return (*this);
}

CMatrix4D& CMatrix4D::operator *=(const CMatrix4D& m)
{
    float x = n[0][0];
    float y = n[1][0];
    float z = n[2][0];
    float w = n[3][0];
    n[0][0] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2] + w * m.n[0][3];
    n[1][0] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2] + w * m.n[1][3];
    n[2][0] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2] + w * m.n[2][3];
    n[3][0] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + w * m.n[3][3];

    x = n[0][1];
    y = n[1][1];
    z = n[2][1];
    w = n[3][1];
    n[0][1] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2] + w * m.n[0][3];
    n[1][1] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2] + w * m.n[1][3];
    n[2][1] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2] + w * m.n[2][3];
    n[3][1] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + w * m.n[3][3];

    x = n[0][2];
    y = n[1][2];
    z = n[2][2];
    w = n[3][2];
    n[0][2] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2] + w * m.n[0][3];
    n[1][2] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2] + w * m.n[1][3];
    n[2][2] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2] + w * m.n[2][3];
    n[3][2] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + w * m.n[3][3];

    x = n[0][3];
    y = n[1][3];
    z = n[2][3];
    w = n[3][3];
    n[0][3] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2] + w * m.n[0][3];
    n[1][3] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2] + w * m.n[1][3];
    n[2][3] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2] + w * m.n[2][3];
    n[3][3] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + w * m.n[3][3];

    return (*this);
}

CMatrix4D& CMatrix4D::operator *=(const CMatrix3D& m)
{
    float x = n[0][0];
    float y = n[1][0];
    float z = n[2][0];
    n[0][0] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][0] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][0] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

    x = n[0][1];
    y = n[1][1];
    z = n[2][1];
    n[0][1] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][1] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][1] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

    x = n[0][2];
    y = n[1][2];
    z = n[2][2];
    n[0][2] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][2] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][2] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

    x = n[0][3];
    y = n[1][3];
    z = n[2][3];
    n[0][3] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][3] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][3] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

    return (*this);
}

CMatrix4D CMatrix4D::operator *(const CMatrix4D& m) const
{
    return (CMatrix4D(n[0][0] * m.n[0][0] + n[1][0] * m.n[0][1] + n[2][0] * m.n[0][2] + n[3][0] * m.n[0][3],
        n[0][0] * m.n[1][0] + n[1][0] * m.n[1][1] + n[2][0] * m.n[1][2] + n[3][0] * m.n[1][3],
        n[0][0] * m.n[2][0] + n[1][0] * m.n[2][1] + n[2][0] * m.n[2][2] + n[3][0] * m.n[2][3],
        n[0][0] * m.n[3][0] + n[1][0] * m.n[3][1] + n[2][0] * m.n[3][2] + n[3][0] * m.n[3][3],
        n[0][1] * m.n[0][0] + n[1][1] * m.n[0][1] + n[2][1] * m.n[0][2] + n[3][1] * m.n[0][3],
        n[0][1] * m.n[1][0] + n[1][1] * m.n[1][1] + n[2][1] * m.n[1][2] + n[3][1] * m.n[1][3],
        n[0][1] * m.n[2][0] + n[1][1] * m.n[2][1] + n[2][1] * m.n[2][2] + n[3][1] * m.n[2][3],
        n[0][1] * m.n[3][0] + n[1][1] * m.n[3][1] + n[2][1] * m.n[3][2] + n[3][1] * m.n[3][3],
        n[0][2] * m.n[0][0] + n[1][2] * m.n[0][1] + n[2][2] * m.n[0][2] + n[3][2] * m.n[0][3],
        n[0][2] * m.n[1][0] + n[1][2] * m.n[1][1] + n[2][2] * m.n[1][2] + n[3][2] * m.n[1][3],
        n[0][2] * m.n[2][0] + n[1][2] * m.n[2][1] + n[2][2] * m.n[2][2] + n[3][2] * m.n[2][3],
        n[0][2] * m.n[3][0] + n[1][2] * m.n[3][1] + n[2][2] * m.n[3][2] + n[3][2] * m.n[3][3],
        n[0][3] * m.n[0][0] + n[1][3] * m.n[0][1] + n[2][3] * m.n[0][2] + n[3][3] * m.n[0][3],
        n[0][3] * m.n[1][0] + n[1][3] * m.n[1][1] + n[2][3] * m.n[1][2] + n[3][3] * m.n[1][3],
        n[0][3] * m.n[2][0] + n[1][3] * m.n[2][1] + n[2][3] * m.n[2][2] + n[3][3] * m.n[2][3],
        n[0][3] * m.n[3][0] + n[1][3] * m.n[3][1] + n[2][3] * m.n[3][2] + n[3][3] * m.n[3][3]));
}

CMatrix4D CMatrix4D::operator *(const CMatrix3D& m) const
{
    return (CMatrix4D(n[0][0] * m.n[0][0] + n[1][0] * m.n[0][1] + n[2][0] * m.n[0][2],
        n[0][0] * m.n[1][0] + n[1][0] * m.n[1][1] + n[2][0] * m.n[1][2],
        n[0][0] * m.n[2][0] + n[1][0] * m.n[2][1] + n[2][0] * m.n[2][2],
        n[3][0],
        n[0][1] * m.n[0][0] + n[1][1] * m.n[0][1] + n[2][1] * m.n[0][2],
        n[0][1] * m.n[1][0] + n[1][1] * m.n[1][1] + n[2][1] * m.n[1][2],
        n[0][1] * m.n[2][0] + n[1][1] * m.n[2][1] + n[2][1] * m.n[2][2],
        n[3][1],
        n[0][2] * m.n[0][0] + n[1][2] * m.n[0][1] + n[2][2] * m.n[0][2],
        n[0][2] * m.n[1][0] + n[1][2] * m.n[1][1] + n[2][2] * m.n[1][2],
        n[0][2] * m.n[2][0] + n[1][2] * m.n[2][1] + n[2][2] * m.n[2][2],
        n[3][2],
        n[0][3] * m.n[0][0] + n[1][3] * m.n[0][1] + n[2][3] * m.n[0][2],
        n[0][3] * m.n[1][0] + n[1][3] * m.n[1][1] + n[2][3] * m.n[1][2],
        n[0][3] * m.n[2][0] + n[1][3] * m.n[2][1] + n[2][3] * m.n[2][2],
        n[3][3]));
}

CVector4 CMatrix4D::operator *(const CVector4& v) const
{
    return (CVector4(n[0][0] * v.x + n[1][0] * v.y + n[2][0] * v.z + n[3][0] * v.w,
        n[0][1] * v.x + n[1][1] * v.y + n[2][1] * v.z + n[3][1] * v.w,
        n[0][2] * v.x + n[1][2] * v.y + n[2][2] * v.z + n[3][2] * v.w,
        n[0][3] * v.x + n[1][3] * v.y + n[2][3] * v.z + n[3][3] * v.w));
}

CVector4 CMatrix4D::operator *(const CVector3& v) const
{
    return (CVector4(n[0][0] * v.x + n[1][0] * v.y + n[2][0] * v.z,
        n[0][1] * v.x + n[1][1] * v.y + n[2][1] * v.z,
        n[0][2] * v.x + n[1][2] * v.y + n[2][2] * v.z,
        n[0][3] * v.x + n[1][3] * v.y + n[2][3] * v.z));
}

CVector4 CMatrix4D::operator *(const CVector2& v) const
{
    return (CVector4(n[0][0] * v.x + n[1][0] * v.y,
        n[0][1] * v.x + n[1][1] * v.y,
        n[0][2] * v.x + n[1][2] * v.y,
        n[0][3] * v.x + n[1][3] * v.y));
}

bool CMatrix4D::operator ==(const CMatrix4D& m) const
{
    return ((n[0][0] == m.n[0][0]) && (n[0][1] == m.n[0][1]) && (n[0][2] == m.n[0][2]) && (n[1][0] == m.n[1][0]) && (n[1][1] == m.n[1][1]) && (n[1][2] == m.n[1][2]) && (n[2][0] == m.n[2][0]) && (n[2][1] == m.n[2][1]) && (n[2][2] == m.n[2][2]) && (n[3][0] == m.n[3][0]) && (n[3][1] == m.n[3][1]) && (n[3][2] == m.n[3][2]));
}

bool CMatrix4D::operator !=(const CMatrix4D& m) const
{
    return ((n[0][0] != m.n[0][0]) || (n[0][1] != m.n[0][1]) || (n[0][2] != m.n[0][2]) || (n[1][0] != m.n[1][0]) || (n[1][1] != m.n[1][1]) || (n[1][2] != m.n[1][2]) || (n[2][0] != m.n[2][0]) || (n[2][1] != m.n[2][1]) || (n[2][2] != m.n[2][2]) || (n[3][0] != m.n[3][0]) || (n[3][1] != m.n[3][1]) || (n[3][2] != m.n[3][2]));
}

CMatrix4D& CMatrix4D::SetIdentity(void)
{
    n[0][0] = n[1][1] = n[2][2] = n[3][3] = 1.0F;
    n[1][0] = n[2][0] = n[3][0] = n[0][1] = n[2][1] = n[3][1] = n[0][2] = n[1][2] = n[3][2] = n[0][3] = n[1][3] = n[2][3] = 0.0F;
    return (*this);
}

float CMatrix4D::Determinant() const
{
    float n00 = (*this)(0,0);
    float n01 = (*this)(0,1);
    float n02 = (*this)(0,2);
    float n03 = (*this)(0,3);

    float n10 = (*this)(1,0);
    float n11 = (*this)(1,1);
    float n12 = (*this)(1,2);
    float n13 = (*this)(1,3);

    float n20 = (*this)(2,0);
    float n21 = (*this)(2,1);
    float n22 = (*this)(2,2);
    float n23 = (*this)(2,3);

    float n30 = (*this)(3,0);
    float n31 = (*this)(3,1);
    float n32 = (*this)(3,2);
    float n33 = (*this)(3,3);

    return (n00 * (n11 * (n22 * n33 - n23 * n32) + n12 * (n23 * n31 - n21 * n33) + n13 * (n21 * n32 - n22 * n31)) +
        n01 * (n10 * (n23 * n32 - n22 * n33) + n12 * (n20 * n33 - n23 * n30) + n13 * (n22 * n30 - n20 * n32)) +
        n02 * (n10 * (n21 * n33 - n23 * n31) + n11 * (n23 * n30 - n20 * n33) + n13 * (n20 * n31 - n21 * n30)) +
        n03 * (n10 * (n22 * n31 - n21 * n32) + n11 * (n20 * n32 - n22 * n30) + n12 * (n21 * n30 - n20 * n31)));
}

CMatrix4D CMatrix4D::Inverse() const
{
    float n00 = (*this)(0,0);
    float n01 = (*this)(0,1);
    float n02 = (*this)(0,2);
    float n03 = (*this)(0,3);

    float n10 = (*this)(1,0);
    float n11 = (*this)(1,1);
    float n12 = (*this)(1,2);
    float n13 = (*this)(1,3);

    float n20 = (*this)(2,0);
    float n21 = (*this)(2,1);
    float n22 = (*this)(2,2);
    float n23 = (*this)(2,3);

    float n30 = (*this)(3,0);
    float n31 = (*this)(3,1);
    float n32 = (*this)(3,2);
    float n33 = (*this)(3,3);

    float p00 = n11 * (n22 * n33 - n23 * n32) + n12 * (n23 * n31 - n21 * n33) + n13 * (n21 * n32 - n22 * n31);
    float p10 = n10 * (n23 * n32 - n22 * n33) + n12 * (n20 * n33 - n23 * n30) + n13 * (n22 * n30 - n20 * n32);
    float p20 = n10 * (n21 * n33 - n23 * n31) + n11 * (n23 * n30 - n20 * n33) + n13 * (n20 * n31 - n21 * n30);
    float p30 = n10 * (n22 * n31 - n21 * n32) + n11 * (n20 * n32 - n22 * n30) + n12 * (n21 * n30 - n20 * n31);

    float t = 1.0F / (n00 * p00 + n01 * p10 + n02 * p20 + n03 * p30);

    return (CMatrix4D(p00 * t,
        (n01 * (n23 * n32 - n22 * n33) + n02 * (n21 * n33 - n23 * n31) + n03 * (n22 * n31 - n21 * n32)) * t,
        (n01 * (n12 * n33 - n13 * n32) + n02 * (n13 * n31 - n11 * n33) + n03 * (n11 * n32 - n12 * n31)) * t,
        (n01 * (n13 * n22 - n12 * n23) + n02 * (n11 * n23 - n13 * n21) + n03 * (n12 * n21 - n11 * n22)) * t,
        p10 * t,
        (n00 * (n22 * n33 - n23 * n32) + n02 * (n23 * n30 - n20 * n33) + n03 * (n20 * n32 - n22 * n30)) * t,
        (n00 * (n13 * n32 - n12 * n33) + n02 * (n10 * n33 - n13 * n30) + n03 * (n12 * n30 - n10 * n32)) * t,
        (n00 * (n12 * n23 - n13 * n22) + n02 * (n13 * n20 - n10 * n23) + n03 * (n10 * n22 - n12 * n20)) * t,
        p20 * t,
        (n00 * (n23 * n31 - n21 * n33) + n01 * (n20 * n33 - n23 * n30) + n03 * (n21 * n30 - n20 * n31)) * t,
        (n00 * (n11 * n33 - n13 * n31) + n01 * (n13 * n30 - n10 * n33) + n03 * (n10 * n31 - n11 * n30)) * t,
        (n00 * (n13 * n21 - n11 * n23) + n01 * (n10 * n23 - n13 * n20) + n03 * (n11 * n20 - n10 * n21)) * t,
        p30 * t,
        (n00 * (n21 * n32 - n22 * n31) + n01 * (n22 * n30 - n20 * n32) + n02 * (n20 * n31 - n21 * n30)) * t,
        (n00 * (n12 * n31 - n11 * n32) + n01 * (n10 * n32 - n12 * n30) + n02 * (n11 * n30 - n10 * n31)) * t,
        (n00 * (n11 * n22 - n12 * n21) + n01 * (n12 * n20 - n10 * n22) + n02 * (n10 * n21 - n11 * n20)) * t));
}

CMatrix4D CMatrix4D::Transpose() const
{
    return (CMatrix4D((*this)(0,0), (*this)(1,0), (*this)(2,0), (*this)(3,0), (*this)(0,1), (*this)(1,1), (*this)(2,1), (*this)(3,1), (*this)(0,2), (*this)(1,2), (*this)(2,2), (*this)(3,2), (*this)(0,3), (*this)(1,3), (*this)(2,3), (*this)(3,3)));
}


CTransform4D::CTransform4D(const CMatrix3D& m)
{
    n[0][0] = m.n[0][0];
    n[0][1] = m.n[0][1];
    n[0][2] = m.n[0][2];
    n[1][0] = m.n[1][0];
    n[1][1] = m.n[1][1];
    n[1][2] = m.n[1][2];
    n[2][0] = m.n[2][0];
    n[2][1] = m.n[2][1];
    n[2][2] = m.n[2][2];

    n[0][3] = n[1][3] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;
    n[3][3] = 1.0F;
}

CTransform4D::CTransform4D(const CMatrix3D& m, const CVector3& p)
{
    n[0][0] = m.n[0][0];
    n[0][1] = m.n[0][1];
    n[0][2] = m.n[0][2];
    n[1][0] = m.n[1][0];
    n[1][1] = m.n[1][1];
    n[1][2] = m.n[1][2];
    n[2][0] = m.n[2][0];
    n[2][1] = m.n[2][1];
    n[2][2] = m.n[2][2];
    n[3][0] = p.x;
    n[3][1] = p.y;
    n[3][2] = p.z;

    n[0][3] = n[1][3] = n[2][3] = 0.0F;
    n[3][3] = 1.0F;
}

CTransform4D::CTransform4D(const CVector3& c1, const CVector3& c2, const CVector3& c3, const CVector3& c4)
{
    n[0][0] = c1.x;
    n[0][1] = c1.y;
    n[0][2] = c1.z;
    n[1][0] = c2.x;
    n[1][1] = c2.y;
    n[1][2] = c2.z;
    n[2][0] = c3.x;
    n[2][1] = c3.y;
    n[2][2] = c3.z;
    n[3][0] = c4.x;
    n[3][1] = c4.y;
    n[3][2] = c4.z;

    n[0][3] = n[1][3] = n[2][3] = 0.0F;
    n[3][3] = 1.0F;
}

CTransform4D::CTransform4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23)
{
    n[0][0] = n00;
    n[1][0] = n01;
    n[2][0] = n02;
    n[3][0] = n03;
    n[0][1] = n10;
    n[1][1] = n11;
    n[2][1] = n12;
    n[3][1] = n13;
    n[0][2] = n20;
    n[1][2] = n21;
    n[2][2] = n22;
    n[3][2] = n23;

    n[0][3] = n[1][3] = n[2][3] = 0.0F;
    n[3][3] = 1.0F;
}

CTransform4D& CTransform4D::Set(const CMatrix3D& m, const CVector3& p)
{
    n[0][0] = m.n[0][0];
    n[0][1] = m.n[0][1];
    n[0][2] = m.n[0][2];
    n[1][0] = m.n[1][0];
    n[1][1] = m.n[1][1];
    n[1][2] = m.n[1][2];
    n[2][0] = m.n[2][0];
    n[2][1] = m.n[2][1];
    n[2][2] = m.n[2][2];
    n[3][0] = p.x;
    n[3][1] = p.y;
    n[3][2] = p.z;

    n[0][3] = n[1][3] = n[2][3] = 0.0F;
    n[3][3] = 1.0F;

    return (*this);
}

CTransform4D& CTransform4D::Set(const CVector3& c1, const CVector3& c2, const CVector3& c3, const CVector3& c4)
{
    n[0][0] = c1.x;
    n[0][1] = c1.y;
    n[0][2] = c1.z;
    n[1][0] = c2.x;
    n[1][1] = c2.y;
    n[1][2] = c2.z;
    n[2][0] = c3.x;
    n[2][1] = c3.y;
    n[2][2] = c3.z;
    n[3][0] = c4.x;
    n[3][1] = c4.y;
    n[3][2] = c4.z;

    n[0][3] = n[1][3] = n[2][3] = 0.0F;
    n[3][3] = 1.0F;

    return (*this);
}

CTransform4D& CTransform4D::Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23)
{
    n[0][0] = n00;
    n[1][0] = n01;
    n[2][0] = n02;
    n[3][0] = n03;
    n[0][1] = n10;
    n[1][1] = n11;
    n[2][1] = n12;
    n[3][1] = n13;
    n[0][2] = n20;
    n[1][2] = n21;
    n[2][2] = n22;
    n[3][2] = n23;

    n[0][3] = n[1][3] = n[2][3] = 0.0F;
    n[3][3] = 1.0F;

    return (*this);
}

CTransform4D& CTransform4D::operator *=(const CTransform4D& m)
{
    float x = n[0][0];
    float y = n[1][0];
    float z = n[2][0];
    n[0][0] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][0] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][0] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];
    n[3][0] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + n[3][0];

    x = n[0][1];
    y = n[1][1];
    z = n[2][1];
    n[0][1] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][1] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][1] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];
    n[3][1] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + n[3][1];

    x = n[0][2];
    y = n[1][2];
    z = n[2][2];
    n[0][2] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][2] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][2] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];
    n[3][2] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + n[3][2];

    return (*this);
}

CTransform4D& CTransform4D::operator *=(const CMatrix3D& m)
{
    float x = n[0][0];
    float y = n[1][0];
    float z = n[2][0];
    n[0][0] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][0] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][0] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

    x = n[0][1];
    y = n[1][1];
    z = n[2][1];
    n[0][1] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][1] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][1] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

    x = n[0][2];
    y = n[1][2];
    z = n[2][2];
    n[0][2] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
    n[1][2] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
    n[2][2] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

    return (*this);
}

CTransform4D CTransform4D::operator *(const CTransform4D& m) const
{
    return (CTransform4D(n[0][0] * m.n[0][0] + n[1][0] * m.n[0][1] + n[2][0] * m.n[0][2],
        n[0][0] * m.n[1][0] + n[1][0] * m.n[1][1] + n[2][0] * m.n[1][2],
        n[0][0] * m.n[2][0] + n[1][0] * m.n[2][1] + n[2][0] * m.n[2][2],
        n[0][0] * m.n[3][0] + n[1][0] * m.n[3][1] + n[2][0] * m.n[3][2] + n[3][0],
        n[0][1] * m.n[0][0] + n[1][1] * m.n[0][1] + n[2][1] * m.n[0][2],
        n[0][1] * m.n[1][0] + n[1][1] * m.n[1][1] + n[2][1] * m.n[1][2],
        n[0][1] * m.n[2][0] + n[1][1] * m.n[2][1] + n[2][1] * m.n[2][2],
        n[0][1] * m.n[3][0] + n[1][1] * m.n[3][1] + n[2][1] * m.n[3][2] + n[3][1],
        n[0][2] * m.n[0][0] + n[1][2] * m.n[0][1] + n[2][2] * m.n[0][2],
        n[0][2] * m.n[1][0] + n[1][2] * m.n[1][1] + n[2][2] * m.n[1][2],
        n[0][2] * m.n[2][0] + n[1][2] * m.n[2][1] + n[2][2] * m.n[2][2],
        n[0][2] * m.n[3][0] + n[1][2] * m.n[3][1] + n[2][2] * m.n[3][2] + n[3][2]));
}

CMatrix4D CTransform4D::operator *(const CMatrix4D& m) const
{
    return (CMatrix4D(n[0][0] * m.n[0][0] + n[1][0] * m.n[0][1] + n[2][0] * m.n[0][2] + n[3][0] * m.n[0][3],
        n[0][0] * m.n[1][0] + n[1][0] * m.n[1][1] + n[2][0] * m.n[1][2] + n[3][0] * m.n[1][3],
        n[0][0] * m.n[2][0] + n[1][0] * m.n[2][1] + n[2][0] * m.n[2][2] + n[3][0] * m.n[2][3],
        n[0][0] * m.n[3][0] + n[1][0] * m.n[3][1] + n[2][0] * m.n[3][2] + n[3][0] * m.n[3][3],
        n[0][1] * m.n[0][0] + n[1][1] * m.n[0][1] + n[2][1] * m.n[0][2] + n[3][1] * m.n[0][3],
        n[0][1] * m.n[1][0] + n[1][1] * m.n[1][1] + n[2][1] * m.n[1][2] + n[3][1] * m.n[1][3],
        n[0][1] * m.n[2][0] + n[1][1] * m.n[2][1] + n[2][1] * m.n[2][2] + n[3][1] * m.n[2][3],
        n[0][1] * m.n[3][0] + n[1][1] * m.n[3][1] + n[2][1] * m.n[3][2] + n[3][1] * m.n[3][3],
        n[0][2] * m.n[0][0] + n[1][2] * m.n[0][1] + n[2][2] * m.n[0][2] + n[3][2] * m.n[0][3],
        n[0][2] * m.n[1][0] + n[1][2] * m.n[1][1] + n[2][2] * m.n[1][2] + n[3][2] * m.n[1][3],
        n[0][2] * m.n[2][0] + n[1][2] * m.n[2][1] + n[2][2] * m.n[2][2] + n[3][2] * m.n[2][3],
        n[0][2] * m.n[3][0] + n[1][2] * m.n[3][1] + n[2][2] * m.n[3][2] + n[3][2] * m.n[3][3],
        m.n[0][3], m.n[1][3], m.n[2][3], m.n[3][3]));
}

CTransform4D CTransform4D::operator *(const CMatrix3D& m) const
{
    return (CTransform4D(n[0][0] * m.n[0][0] + n[1][0] * m.n[0][1] + n[2][0] * m.n[0][2],
        n[0][0] * m.n[1][0] + n[1][0] * m.n[1][1] + n[2][0] * m.n[1][2],
        n[0][0] * m.n[2][0] + n[1][0] * m.n[2][1] + n[2][0] * m.n[2][2],
        n[3][0],
        n[0][1] * m.n[0][0] + n[1][1] * m.n[0][1] + n[2][1] * m.n[0][2],
        n[0][1] * m.n[1][0] + n[1][1] * m.n[1][1] + n[2][1] * m.n[1][2],
        n[0][1] * m.n[2][0] + n[1][1] * m.n[2][1] + n[2][1] * m.n[2][2],
        n[3][1],
        n[0][2] * m.n[0][0] + n[1][2] * m.n[0][1] + n[2][2] * m.n[0][2],
        n[0][2] * m.n[1][0] + n[1][2] * m.n[1][1] + n[2][2] * m.n[1][2],
        n[0][2] * m.n[2][0] + n[1][2] * m.n[2][1] + n[2][2] * m.n[2][2],
        n[3][2]));
}

CVector4 CTransform4D::operator *(const CVector4& v) const
{
    return (CVector4(n[0][0] * v.x + n[1][0] * v.y + n[2][0] * v.z + n[3][0] * v.w,
        n[0][1] * v.x + n[1][1] * v.y + n[2][1] * v.z + n[3][1] * v.w,
        n[0][2] * v.x + n[1][2] * v.y + n[2][2] * v.z + n[3][2] * v.w,
        v.w));
}

CVector3 CTransform4D::operator *(const CVector3& v) const
{
    return (CVector3(n[0][0] * v.x + n[1][0] * v.y + n[2][0] * v.z,
        n[0][1] * v.x + n[1][1] * v.y + n[2][1] * v.z,
        n[0][2] * v.x + n[1][2] * v.y + n[2][2] * v.z));
}

CVector2 CTransform4D::operator *(const CVector2& v) const
{
    return (CVector2(n[0][0] * v.x + n[1][0] * v.y,
        n[0][1] * v.x + n[1][1] * v.y));
}

CMatrix3D CTransform4D::GetMatrix3D(void) const
{
    return (CMatrix3D(n[0][0], n[1][0], n[2][0], n[0][1], n[1][1], n[2][1], n[0][2], n[1][2], n[2][2]));
}

CTransform4D& CTransform4D::SetMatrix3D(const CMatrix3D& m)
{
    n[0][0] = m.n[0][0];
    n[0][1] = m.n[0][1];
    n[0][2] = m.n[0][2];
    n[1][0] = m.n[1][0];
    n[1][1] = m.n[1][1];
    n[1][2] = m.n[1][2];
    n[2][0] = m.n[2][0];
    n[2][1] = m.n[2][1];
    n[2][2] = m.n[2][2];

    return (*this);
}

CTransform4D& CTransform4D::SetMatrix3D(const CTransform4D& m)
{
    n[0][0] = m.n[0][0];
    n[0][1] = m.n[0][1];
    n[0][2] = m.n[0][2];
    n[1][0] = m.n[1][0];
    n[1][1] = m.n[1][1];
    n[1][2] = m.n[1][2];
    n[2][0] = m.n[2][0];
    n[2][1] = m.n[2][1];
    n[2][2] = m.n[2][2];

    return (*this);
}

CTransform4D& CTransform4D::SetMatrix3D(const CVector3& c1, const CVector3& c2, const CVector3& c3)
{
    n[0][0] = c1.x;
    n[0][1] = c1.y;
    n[0][2] = c1.z;
    n[1][0] = c2.x;
    n[1][1] = c2.y;
    n[1][2] = c2.z;
    n[2][0] = c3.x;
    n[2][1] = c3.y;
    n[2][2] = c3.z;

    return (*this);
}

CTransform4D& CTransform4D::SetMatrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22)
{
    n[0][0] = n00;
    n[1][0] = n01;
    n[2][0] = n02;
    n[0][1] = n10;
    n[1][1] = n11;
    n[2][1] = n12;
    n[0][2] = n20;
    n[1][2] = n21;
    n[2][2] = n22;

    return (*this);
}

CTransform4D& CTransform4D::SetRotationAboutX(float angle)
{
    float s = sin(angle);
    n[0][0] = n[3][3] = 1.0F;
    n[0][1] = n[0][2] = n[0][3] = n[1][0] = n[1][3] = n[2][0] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;
    n[1][1] = n[2][2] = cos(angle);
    n[2][1] = -s;
    n[1][2] = s;

    return (*this);
}

CTransform4D& CTransform4D::SetRotationAboutY(float angle)
{
    float s = sin(angle);
    n[1][1] = n[3][3] = 1.0F;
    n[0][1] = n[0][3] = n[1][0] = n[1][2] = n[1][3] = n[2][1] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;
    n[0][0] = n[2][2] = cos(angle);
    n[2][0] = s;
    n[0][2] = -s;

    return (*this);
}

CTransform4D& CTransform4D::SetRotationAboutZ(float angle)
{
    float s = sin(angle);
    n[2][2] = n[3][3] = 1.0F;
    n[0][2] = n[0][3] = n[1][2] = n[1][3] = n[2][0] = n[2][1] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;
    n[0][0] = n[1][1] = cos(angle);
    n[1][0] = -s;
    n[0][1] = s;

    return (*this);
}

CTransform4D& CTransform4D::SetRotationAboutAxis(float angle, const CVector3& axis)
{
    float s = sin(angle);
    float c = cos(angle);
    float k = 1.0F - c;

    n[0][0] = c + k * axis.x * axis.x;
    n[1][0] = k * axis.x * axis.y - s * axis.z;
    n[2][0] = k * axis.x * axis.z + s * axis.y;
    n[0][1] = k * axis.x * axis.y + s * axis.z;
    n[1][1] = c + k * axis.y * axis.y;
    n[2][1] = k * axis.y * axis.z - s * axis.x;
    n[0][2] = k * axis.x * axis.z - s * axis.y;
    n[1][2] = k * axis.y * axis.z + s * axis.x;
    n[2][2] = c + k * axis.z * axis.z;

    n[3][3] = 1.0F;
    n[0][3] = n[1][3] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;

    return (*this);
}

void CTransform4D::GetEulerAngles(float *x, float *y, float *z) const
{
    float sy = n[2][0];
    if (sy < 1.0F)
    {
        if (sy > -1.0F)
        {
            *x = -atan2(n[2][1], n[2][2]);
            *y = asin(sy);
            *z = -atan2(n[1][0], n[0][0]);
        }
        else
        {
            *x = 0.0F;
            *y = -MATH_PI_HALF;
            *z = atan2(n[0][1], n[1][1]);
        }
    }
    else
    {
        *x = 0.0F;
        *y = MATH_PI_HALF;
        *z = atan2(n[0][1], n[1][1]);
    }
}

CTransform4D& CTransform4D::SetEulerAngles(float x, float y, float z)
{
    float cx = cos(x);
    float cy = cos(y);
    float cz = cos(z);
    float sx = sin(x);
    float sy = sin(y);
    float sz = sin(z);

    n[0][0] = cy * cz;
    n[0][1] = cx * sz + sx * sy * cz;
    n[0][2] = sx * sz - cx * sy * cz;
    n[1][0] = -cy * sz;
    n[1][1] = cx * cz - sx * sy * sz;
    n[1][2] = sx * cz + cx * sy * sz;
    n[2][0] = sy;
    n[2][1] = -sx * cy;
    n[2][2] = cx * cy;

    return (*this);
}

CTransform4D& CTransform4D::SetScale(float t)
{
    n[0][0] = n[1][1] = n[2][2] = t;
    n[3][3] = 1.0F;
    n[1][0] = n[2][0] = n[3][0] = n[0][1] = n[2][1] = n[3][1] = n[0][2] = n[1][2] = n[3][2] = n[0][3] = n[1][3] = n[2][3] = 0.0F;
    return (*this);
}

CTransform4D& CTransform4D::SetScale(float r, float s, float t)
{
    n[0][0] = r;
    n[1][1] = s;
    n[2][2] = t;
    n[3][3] = 1.0F;
    n[1][0] = n[2][0] = n[3][0] = n[0][1] = n[2][1] = n[3][1] = n[0][2] = n[1][2] = n[3][2] = n[0][3] = n[1][3] = n[2][3] = 0.0F;
    return (*this);
}


float CTransform4D::Determinant() const
{
    float n00 = (*this)(0,0);
    float n01 = (*this)(0,1);
    float n02 = (*this)(0,2);

    float n10 = (*this)(1,0);
    float n11 = (*this)(1,1);
    float n12 = (*this)(1,2);

    float n20 = (*this)(2,0);
    float n21 = (*this)(2,1);
    float n22 = (*this)(2,2);

    return (n00 * (n11 * n22 - n12 * n21) + n01 * (n12 * n20 - n10 * n22) + n02 * (n10 * n21 - n11 * n20));
}

CTransform4D CTransform4D::Inverse() const
{
    float n00 = (*this)(0,0);
    float n01 = (*this)(0,1);
    float n02 = (*this)(0,2);
    float n03 = (*this)(0,3);

    float n10 = (*this)(1,0);
    float n11 = (*this)(1,1);
    float n12 = (*this)(1,2);
    float n13 = (*this)(1,3);

    float n20 = (*this)(2,0);
    float n21 = (*this)(2,1);
    float n22 = (*this)(2,2);
    float n23 = (*this)(2,3);

    float p00 = n11 * n22 - n12 * n21;
    float p10 = n12 * n20 - n10 * n22;
    float p20 = n10 * n21 - n11 * n20;

    float t = 1.0F / (n00 * p00 + n01 * p10 + n02 * p20);

    return (CTransform4D(p00 * t,
        (n02 * n21 - n01 * n22) * t,
        (n01 * n12 - n02 * n11) * t,
        (n01 * (n13 * n22 - n12 * n23) + n02 * (n11 * n23 - n13 * n21) + n03 * (n12 * n21 - n11 * n22)) * t,
        p10 * t,
        (n00 * n22 - n02 * n20) * t,
        (n02 * n10 - n00 * n12) * t,
        (n00 * (n12 * n23 - n13 * n22) + n02 * (n13 * n20 - n10 * n23) + n03 * (n10 * n22 - n12 * n20)) * t,
        p20 * t,
        (n01 * n20 - n00 * n21) * t,
        (n00 * n11 - n01 * n10) * t,
        (n00 * (n13 * n21 - n11 * n23) + n01 * (n10 * n23 - n13 * n20) + n03 * (n11 * n20 - n10 * n21)) * t));
}
