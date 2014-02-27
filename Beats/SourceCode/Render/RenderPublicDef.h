#ifndef BEATS_RENDER_RENDERPUBLICDEF_H__INCLUDE
#define BEATS_RENDER_RENDERPUBLICDEF_H__INCLUDE

struct SVertex
{
    D3DXVECTOR3 m_position;
    D3DCOLOR    m_color;
    D3DXVECTOR2 m_uv;
    static const long VertexFormat = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    SVertex() 
        : m_position(0,0,0)
        , m_color(0)
        , m_uv(0,0)
    {}

    SVertex(float x, float y, float z, unsigned long color, float u, float v) 
        : m_position(x,y,z)
        , m_color(color)
        , m_uv(u,v)
    {}
};

struct SVertex2
{
    D3DXVECTOR3 m_position;
    D3DCOLOR    m_color;
    static const long VertexFormat = D3DFVF_XYZ | D3DFVF_DIFFUSE;

    SVertex2() 
        : m_position(0,0,0)
        , m_color(0)
    {}

    SVertex2(float x, float y, float z, unsigned long color) 
        : m_position(x,y,z)
        , m_color(color)
    {}
};

enum ERenderCacheType
{
    eRCT_Quad,
    eRCT_Line,

    eRCT_Count,
    eRCT_ForceTo32Bit
};
#endif