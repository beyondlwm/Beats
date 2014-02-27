#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTGRAPHIC_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTGRAPHIC_H__INCLUDE

#include "../expdef.h"
#include "../../Utility/Math/Vector2.h"
#include "../../Utility/Math/Vector3.h"

static const int HEADER_HEIGHT = 2;
static const int DEPENDENCY_HEIGHT = 1;
static const int DEPENDENCY_WIDTH = 1;
static const int MIN_WIDTH = 6;
static const int CONNECTION_WIDTH = 1;
static const float DEPENDENCY_LINE_WIDTH = 2.5f;
static const float ARROW_SIZE = 15.0f;

struct SVertex
{
    CVector3 m_position;
    DWORD m_color;
    CVector2 m_uv;
    static const long VertexFormat/* = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1*/;

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
    CVector3 m_position;
    DWORD    m_color;
    static const long VertexFormat /*= D3DFVF_XYZ | D3DFVF_DIFFUSE*/;

    SVertex2() 
        : m_position(0,0,0)
        , m_color(0)
    {}

    SVertex2(float x, float y, float z, unsigned long color) 
        : m_position(x,y,z)
        , m_color(color)
    {}
};

enum EComponentAeraRectType
{
    eCART_Invalid,
    eCART_Body,
    eCART_Connection,
    eCART_Dependency,
    eCART_DependencyLine,

    eCART_Count,
    eCART_Force32Bit = 0xFFFFFFFF
};

enum EComponentTexture
{
    eCT_RectBG,
    eCT_ConnectRect,
    eCT_SelectedRectBG,
    eCT_NormalLine,
    eCT_SelectedLine,
    eCT_NormalArrow,
    eCT_SelectedArrow,
    eCT_ConnectedDependency,
    eCT_ConnectedDependencyList,
    eCT_WeakDependency,
    eCT_WeakDependencyList,
    eCT_StrongDependency,
    eCT_StrongDependencyList,

    eCT_Count,
    eCT_Force32Bit = 0xFFFFFFFF
};
class CComponentEditorProxy;
class CDependencyDescription;

class COMPONENTS_DLL_DECL CComponentGraphic
{
public:
    CComponentGraphic();
    ~CComponentGraphic();

    void OnRender(float gridSize, bool bSelected);
    void SetPosition(int x, int y);
    void GetPosition(int* pOutX, int* pOutY);
    void GetDependencyPosition(size_t uDependencyIndex, int* pOutX, int* pOutY);
    size_t GetDependencyWidth();
    size_t GetDependencyHeight();
    size_t GetHeaderHeight();
    size_t GetConnectWidth();
    float GetDependencyLineWidth();
    float GetDependencyLineArrowSize();

    size_t GetWidth();
    size_t GetHeight();
    EComponentAeraRectType HitTestForAreaType(int x, int y, void** pReturnData);
    void SetOwner(CComponentEditorProxy* pOwner);
    CComponentEditorProxy* GetOwner() const;
    void CaculateSize();

    virtual CComponentGraphic* Clone() = 0;

protected:
    virtual void DrawHead(float cellSize) = 0;
    virtual void DrawDependencies(float cellSize) = 0;
    virtual void DrawDependencyLine(float cellSize, const CDependencyDescription* pDependency) = 0;
    virtual void DrawSelectedRect(float cellSize) = 0;

protected:
    CComponentEditorProxy* m_pOwner;
    size_t m_width;
    size_t m_height;
    int m_gridPosX;
    int m_gridPosY;
    int m_gridPosZ;
};

#endif
