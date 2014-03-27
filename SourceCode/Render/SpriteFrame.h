#ifndef RENDER_SPRITE_FRAME_H__INCLUDE
#define RENDER_SPRITE_FRAME_H__INCLUDE
#include "Geometry.h"
#include "CommonTypes.h"

class CTexture;
class CTextureFrag;

class CSpriteFrame
{
public:
    CSpriteFrame(SharePtr<CTexture> texture, const CRect &rect);
    CSpriteFrame(SharePtr<CTexture> texture, const CRect &rect, const CSize &size, const CPoint &origin);
    ~CSpriteFrame();

    void SetQuad(const CQuadP &quad);
    void SetOriginSize(const CPoint &origin, const CSize &size);

    const CQuadP &QuadP() const;
    const CQuadT &QuadT() const;
    SharePtr<CTexture> Texture() const;

private:
    CQuadP m_quad;
    CTextureFrag *m_textureFrag;
};

#endif