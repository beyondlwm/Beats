#ifndef BEATS_RENDER_RENDERUNIT_H__INCLUDE
#define BEATS_RENDER_RENDERUNIT_H__INCLUDE

#include "RenderPublicDef.h"
class CBaseObject;

class CRenderUnit
{
public:
    CRenderUnit(CBaseObject* pOwner);
    ~CRenderUnit();

    void PreRender();
    void Render(float timeDeltaMs);
    bool IsRecycle();
    bool IsActive();
    void Reset();
    void SetActive(bool active);
    void SetColor(unsigned long color);
    unsigned long GetColor();
private:
    bool m_active;
    bool m_recycleFlag;
    unsigned long m_color;
    CBaseObject* m_pOwner;
    IDirect3DDevice9* m_pRenderDevice;
    LPDIRECT3DTEXTURE9 m_pTexture;
};


#endif