#ifndef RENDER_SPRITE_FRAME_BATCH_MANAGER_H__INCLUDE
#define RENDER_SPRITE_FRAME_BATCH_MANAGER_H__INCLUDE

class CTexture;
class CTextureFrag;
class CSpriteFrameBatch;
class CShaderProgram;
class CCamera;

class CSpriteFrameBatchManager
{
    BEATS_DECLARE_SINGLETON(CSpriteFrameBatchManager);
public:
    void AddQuad(const CQuadP &quad, CTextureFrag *frag, const kmMat4 &transform);
    void AddQuad(const CQuadPT &quad, SharePtr<CTexture> texture);
    void Render();
    void Clear();
    CCamera* GetCamera() const;

private:
    std::map<GLuint, CSpriteFrameBatch *> m_batches;
    CShaderProgram* m_pProgram;
    CCamera *m_pCamera;
};

#endif