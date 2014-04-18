#ifndef RENDER_COMMONTYPES_H__INCLUDE
#define RENDER_COMMONTYPES_H__INCLUDE

class CTex
{
public:
    float u, v;

    CTex(float u = 0.f, float v = 0.f):u(u),v(v){}
};

class CColor
{
public:
    unsigned char r, g, b, a;
    CColor()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }

    CColor(float fr, float fg, float fb, float fa)
    {
        BEATS_ASSERT(fr <= 1.0f && fr >= 0.0f);
        BEATS_ASSERT(fg <= 1.0f && fg >= 0.0f);
        BEATS_ASSERT(fb <= 1.0f && fb >= 0.0f);
        BEATS_ASSERT(fa <= 1.0f && fa >= 0.0f);
        r = (char)(fr * 0xFF);
        g = (char)(fg * 0xFF);
        b = (char)(fb * 0xFF);
        a = (char)(fa * 0xFF);
    }

    CColor(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa)
        : r(rr)
        , g(gg)
        , b(bb)
        , a(aa)
    {
    }

    CColor(size_t colorUint)
    {
        r = colorUint >> 24;
        g = (colorUint >> 16) & 0x000000FF;
        b = (colorUint >> 8) & 0x000000FF;
        a = colorUint & 0x000000FF;
    }

    bool operator==( const CColor& other ) const
    {
        return ( r == other.r && g == other.g && b == other.b && a == other.a );
    }

    operator size_t () const
    {
        size_t uRet = (r << 24) + (g << 16) + (b << 8) + a;
        return uRet;
    }
};

class CIVector4
{
public:
    int x, y, z, w;

    CIVector4(int x = 0, int y = 0, int z = 0, int w = 0):x(x),y(y),z(z),w(w){}
};

class CVertexPT
{
public:
    kmVec3 position;
    CTex tex;

    CVertexPT()
    {
        kmVec3Fill(&position, 0.f, 0.f, 0.f);
    }
};

class CVertexPTC
{
public:
    kmVec3 position;
    CColor color;
    CTex tex;
    CVertexPTC()
    {
        kmVec3Fill(&position, 0.f, 0.f, 0.f);
    }
};

class CVertexPTB
{
public:
    kmVec3 position;
    CTex tex;
    CIVector4 bones;
    kmVec4 weights;

    CVertexPTB()
    {
        kmVec3Fill(&position, 0.f, 0.f, 0.f);
        kmVec4Fill(&weights, 0.f, 0.f, 0.f, 0.f);
    }
};

class CVertexPC
{
public:
    kmVec3 position;
    CColor color;

    CVertexPC()
    {
        kmVec3Fill(&position, 0.f, 0.f, 0.f);
    }
};

class CVertexPTN
{
public:
    kmVec3 position;
    kmVec3 normal;
    CTex tex;
    CVertexPTN()
    {
        kmVec3Fill(&position, 0.f, 0.f, 0.f);
        kmVec3Fill(&normal, 0.f, 0.f, 0.f);
    }
};

class CVertexPTNC
{
public:
    kmVec3 position;
    kmVec3 normal;
    CTex tex;
    CColor color;
    CVertexPTNC()
    {
        kmVec3Fill(&position, 0.f, 0.f, 0.f);
        kmVec3Fill(&normal, 0.f, 0.f, 0.f);
    }
};

class CQuadPT
{
public:
    CVertexPT tl,    //top left
            bl,    //bottom left
            tr,    //top right
            br;    //bottom right
};

class CQuadPTC
{
public:
    CVertexPTC tl,
               bl,
               tr,
               br;
};

class CQuadP
{
public:
    kmVec3  tl,    //topleft
            bl,    //bottomleft
            tr,    //topright
            br;    //bottomright

    CQuadP()
    {
        kmVec3Fill(&tl, 0.f, 0.f, 0.f);
        kmVec3Fill(&tr, 0.f, 0.f, 0.f);
        kmVec3Fill(&bl, 0.f, 0.f, 0.f);
        kmVec3Fill(&br, 0.f, 0.f, 0.f);
    }
};

class CQuadT
{
public:
    CTex    tl,    //topleft
            bl,    //bottomleft
            tr,    //topright
            br;    //bottomright
};

#endif