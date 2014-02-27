#include "stdafx.h"
#include "Matrix3.h"

CMatrix3D::CMatrix3D(const CVector3& c1, const CVector3& c2, const CVector3& c3)
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
}

CMatrix3D::CMatrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22)
{
    n[0][0] = n00;
    n[0][1] = n10;
    n[0][2] = n20;
    n[1][0] = n01;
    n[1][1] = n11;
    n[1][2] = n21;
    n[2][0] = n02;
    n[2][1] = n12;
    n[2][2] = n22;
}

CMatrix3D& CMatrix3D::Set(const CVector3& c1, const CVector3& c2, const CVector3& c3)
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

CMatrix3D& CMatrix3D::Set(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22)
{
    n[0][0] = n00;
    n[0][1] = n10;
    n[0][2] = n20;
    n[1][0] = n01;
    n[1][1] = n11;
    n[1][2] = n21;
    n[2][0] = n02;
    n[2][1] = n12;
    n[2][2] = n22;

    return (*this);
}

CMatrix3D& CMatrix3D::operator *=(const CMatrix3D& m)
{
    float t = n[0][0] * m.n[0][0] + n[1][0] * m.n[0][1] + n[2][0] * m.n[0][2];
    float u = n[0][0] * m.n[1][0] + n[1][0] * m.n[1][1] + n[2][0] * m.n[1][2];
    n[2][0] = n[0][0] * m.n[2][0] + n[1][0] * m.n[2][1] + n[2][0] * m.n[2][2];
    n[0][0] = t;
    n[1][0] = u;

    t = n[0][1] * m.n[0][0] + n[1][1] * m.n[0][1] + n[2][1] * m.n[0][2];
    u = n[0][1] * m.n[1][0] + n[1][1] * m.n[1][1] + n[2][1] * m.n[1][2];
    n[2][1] = n[0][1] * m.n[2][0] + n[1][1] * m.n[2][1] + n[2][1] * m.n[2][2];
    n[0][1] = t;
    n[1][1] = u;

    t = n[0][2] * m.n[0][0] + n[1][2] * m.n[0][1] + n[2][2] * m.n[0][2];
    u = n[0][2] * m.n[1][0] + n[1][2] * m.n[1][1] + n[2][2] * m.n[1][2];
    n[2][2] = n[0][2] * m.n[2][0] + n[1][2] * m.n[2][1] + n[2][2] * m.n[2][2];
    n[0][2] = t;
    n[1][2] = u;

    return (*this);
}

CMatrix3D& CMatrix3D::operator *=(float t)
{
    n[0][0] *= t;
    n[0][1] *= t;
    n[0][2] *= t;
    n[1][0] *= t;
    n[1][1] *= t;
    n[1][2] *= t;
    n[2][0] *= t;
    n[2][1] *= t;
    n[2][2] *= t;

    return (*this);
}

CMatrix3D& CMatrix3D::operator /=(float t)
{
    float f = 1.0F / t;
    n[0][0] *= f;
    n[0][1] *= f;
    n[0][2] *= f;
    n[1][0] *= f;
    n[1][1] *= f;
    n[1][2] *= f;
    n[2][0] *= f;
    n[2][1] *= f;
    n[2][2] *= f;

    return (*this);
}

CMatrix3D CMatrix3D::operator *(const CMatrix3D& m) const
{
    return (CMatrix3D(n[0][0] * m.n[0][0] + n[1][0] * m.n[0][1] + n[2][0] * m.n[0][2],
        n[0][0] * m.n[1][0] + n[1][0] * m.n[1][1] + n[2][0] * m.n[1][2],
        n[0][0] * m.n[2][0] + n[1][0] * m.n[2][1] + n[2][0] * m.n[2][2],
        n[0][1] * m.n[0][0] + n[1][1] * m.n[0][1] + n[2][1] * m.n[0][2],
        n[0][1] * m.n[1][0] + n[1][1] * m.n[1][1] + n[2][1] * m.n[1][2],
        n[0][1] * m.n[2][0] + n[1][1] * m.n[2][1] + n[2][1] * m.n[2][2],
        n[0][2] * m.n[0][0] + n[1][2] * m.n[0][1] + n[2][2] * m.n[0][2],
        n[0][2] * m.n[1][0] + n[1][2] * m.n[1][1] + n[2][2] * m.n[1][2],
        n[0][2] * m.n[2][0] + n[1][2] * m.n[2][1] + n[2][2] * m.n[2][2]));
}

CMatrix3D CMatrix3D::operator *(float t) const
{
    return (CMatrix3D(n[0][0] * t, n[1][0] * t, n[2][0] * t, n[0][1] * t, n[1][1] * t, n[2][1] * t, n[0][2] * t, n[1][2] * t, n[2][2] * t));
}

CMatrix3D CMatrix3D::operator /(float t) const
{
    float f = 1.0F / t;
    return (CMatrix3D(n[0][0] * f, n[1][0] * f, n[2][0] * f, n[0][1] * f, n[1][1] * f, n[2][1] * f, n[0][2] * f, n[1][2] * f, n[2][2] * f));
}

CVector3 CMatrix3D::operator *(const CVector3& v) const
{
    return (CVector3(n[0][0] * v.x + n[1][0] * v.y + n[2][0] * v.z, n[0][1] * v.x + n[1][1] * v.y + n[2][1] * v.z, n[0][2] * v.x + n[1][2] * v.y + n[2][2] * v.z));
}

bool CMatrix3D::operator ==(const CMatrix3D& m) const
{
    return ((n[0][0] == m.n[0][0]) && (n[0][1] == m.n[0][1]) && (n[0][2] == m.n[0][2]) && (n[1][0] == m.n[1][0]) && (n[1][1] == m.n[1][1]) && (n[1][2] == m.n[1][2]) && (n[2][0] == m.n[2][0]) && (n[2][1] == m.n[2][1]) && (n[2][2] == m.n[2][2]));
}

bool CMatrix3D::operator !=(const CMatrix3D& m) const
{
    return ((n[0][0] != m.n[0][0]) || (n[0][1] != m.n[0][1]) || (n[0][2] != m.n[0][2]) || (n[1][0] != m.n[1][0]) || (n[1][1] != m.n[1][1]) || (n[1][2] != m.n[1][2]) || (n[2][0] != m.n[2][0]) || (n[2][1] != m.n[2][1]) || (n[2][2] != m.n[2][2]));
}

CMatrix3D& CMatrix3D::SetIdentity(void)
{
    n[0][0] = n[1][1] = n[2][2] = 1.0F;
    n[0][1] = n[0][2] = n[1][0] = n[1][2] = n[2][0] = n[2][1] = 0.0F;

    return (*this);
}

CMatrix3D& CMatrix3D::SetRotationAboutX(float angle)
{
    float s = sin(angle);
    n[0][0] = 1.0F;
    n[0][1] = n[0][2] = n[1][0] = n[2][0] = 0.0F;
    n[1][1] = n[2][2] = cos(angle);
    n[2][1] = -s;
    n[1][2] = s;

    return (*this);
}

CMatrix3D& CMatrix3D::SetRotationAboutY(float angle)
{
    float s = sin(angle);
    n[1][1] = 1.0F;
    n[1][0] = n[1][2] = n[0][1] = n[2][1] = 0.0F;
    n[0][0] = n[2][2] = cos(angle);
    n[2][0] = s;
    n[0][2] = -s;

    return (*this);
}

CMatrix3D& CMatrix3D::SetRotationAboutZ(float angle)
{
    float s = sin(angle);
    n[2][2] = 1.0F;
    n[2][0] = n[2][1] = n[0][2] = n[1][2] = 0.0F;
    n[0][0] = n[1][1] = cos(angle);
    n[1][0] = -s;
    n[0][1] = s;

    return (*this);
}

CMatrix3D& CMatrix3D::SetRotationAboutAxis(float angle, const CVector3& axis)
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

    return (*this);
}

void CMatrix3D::GetEulerAngles(float *x, float *y, float *z) const
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

CMatrix3D& CMatrix3D::SetEulerAngles(float x, float y, float z)
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

CMatrix3D& CMatrix3D::SetScale(float t)
{
    n[0][0] = n[1][1] = n[2][2] = t;
    n[0][1] = n[0][2] = n[1][0] = n[1][2] = n[2][0] = n[2][1] = 0.0F;

    return (*this);
}

CMatrix3D& CMatrix3D::SetScale(float r, float s, float t)
{
    n[0][0] = r;
    n[1][1] = s;
    n[2][2] = t;
    n[0][1] = n[0][2] = n[1][0] = n[1][2] = n[2][0] = n[2][1] = 0.0F;

    return (*this);
}

CMatrix3D& CMatrix3D::Orthonormalize(void)
{
    CVector3& x = (*this)[0];
    CVector3& y = (*this)[1];
    CVector3& z = (*this)[2];

    x.Normalize();
    (y = y - x * (x * y)).Normalize();
    (z = z - x * (x * z) - y * (y * z)).Normalize();

    return (*this);
}

float CMatrix3D::Determinant(const CMatrix3D& m)
{
    return (m(0,0) * (m(1,1) * m(2,2) - m(2,1) * m(2,1)) - m(0,1) * (m(1,0) * m(2,2) - m(1,2) * m(2,0)) + m(0,2) * (m(1,0) * m(2,1) - m(1,1) * m(2,0)));
}

CMatrix3D CMatrix3D::Inverse(const CMatrix3D& m)
{
    float n00 = m(0,0);
    float n01 = m(0,1);
    float n02 = m(0,2);
    float n10 = m(1,0);
    float n11 = m(1,1);
    float n12 = m(1,2);
    float n20 = m(2,0);
    float n21 = m(2,1);
    float n22 = m(2,2);

    float p00 = n11 * n22 - n12 * n21;
    float p10 = n12 * n20 - n10 * n22;
    float p20 = n10 * n21 - n11 * n20;

    float t = 1.0F / (n00 * p00 + n01 * p10 + n02 * p20);

    return (CMatrix3D(p00 * t, (n02 * n21 - n01 * n22) * t, (n01 * n12 - n02 * n11) * t,
        p10 * t, (n00 * n22 - n02 * n20) * t, (n02 * n10 - n00 * n12) * t,
        p20 * t, (n01 * n20 - n00 * n21) * t, (n00 * n11 - n01 * n10) * t));
}

CMatrix3D CMatrix3D::Transpose(const CMatrix3D& m)
{
    return (CMatrix3D(m(0,0), m(1,0), m(2,0), m(0,1), m(1,1), m(2,1), m(0,2), m(1,2), m(2,2)));
}
