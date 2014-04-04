#ifndef RENDER_SPRITE_FRAME_H__INCLUDE
#define RENDER_SPRITE_FRAME_H__INCLUDE
#include "Geometry.h"
#include "CommonTypes.h"

class CTexture;
class CTextureFrag;

class CSpriteFrame
{
public:
    CSpriteFrame(CTextureFrag *textureFrag);
    CSpriteFrame(const TString &textureFragName);
    CSpriteFrame(const TString &textureFragName, const kmVec2 &size, const kmVec2 &origin);
    ~CSpriteFrame();

    void SetQuad(const CQuadP &quad);
    void SetOriginSize(const kmVec2 &origin, const kmVec2 &size);

    const CQuadP &QuadP() const;
    const CQuadT &QuadT() const;
    SharePtr<CTexture> Texture() const;

private:
    CQuadP m_quad;
    CTextureFrag *m_textureFrag;
};

#endif