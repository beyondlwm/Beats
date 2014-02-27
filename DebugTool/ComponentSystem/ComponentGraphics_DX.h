#ifndef BEATS_COMPONENTS_COMPONENTGRAPHICS_DX_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENTGRAPHICS_DX_H__INCLUDE

#include <d3d9.h>
#include <d3dx9core.h>
#include "../../Components/Component/ComponentGraphic.h"

class CComponentGraphic_DX : public CComponentGraphic
{
    typedef CComponentGraphic super;
public:
    CComponentGraphic_DX(IDirect3DDevice9* pRenderDevice, LPD3DXFONT m_pFont);
    virtual ~CComponentGraphic_DX();

    static void RecreateTextures(IDirect3DDevice9* pRenderDevice, const TCHAR* pPath);
    static void ReleaseTextures();
    virtual CComponentGraphic* Clone();

private:
    virtual void DrawHead(float cellSize);
    virtual void DrawDependencies(float cellSize);
    virtual void DrawDependencyLine(float cellSize, const CDependencyDescription* pDependency);
    virtual void DrawSelectedRect(float cellSize);

private:
    static LPDIRECT3DTEXTURE9* m_pTextures;
    IDirect3DDevice9* m_pRenderDevice;
    LPD3DXFONT m_pFont;
};


#endif