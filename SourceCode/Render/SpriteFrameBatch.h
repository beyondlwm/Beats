#ifndef RENDER_SPRITE_FRAME_BATCH_H__INCLUDE
#define RENDER_SPRITE_FRAME_BATCH_H__INCLUDE
#include "RenderObject.h"
#include "CommonTypes.h"

class CTexture;
class CTextureFrag;
class CSpriteFrameBatch : public CRenderObject
{
public:
    CSpriteFrameBatch();
    ~CSpriteFrameBatch();

    virtual void PreRender();

    virtual void Render();

    virtual void PostRender();

    void AddQuad(const CQuadP &quad, CTextureFrag *frag, const kmMat4 &transform);

    void AddQuad(const CQuadPT &quad, SharePtr<CTexture> texture);

    void Clear();
    
private:
    void setupVAO();
    void updateVBO();

private:
    static const size_t MAX_QUADS = 1024;
    GLuint m_texture;
    CQuadPT m_quads[MAX_QUADS];
    size_t m_count;
    GLuint m_VAO, m_VBO[2];
};

#endif