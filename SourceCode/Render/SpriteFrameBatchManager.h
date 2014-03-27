#ifndef RENDER_SPRITE_FRAME_BATCH_MANAGER_H__INCLUDE
#define RENDER_SPRITE_FRAME_BATCH_MANAGER_H__INCLUDE

class CSpriteFrame;
class CSpriteFrameBatch;
class CShaderProgram;

class CSpriteFrameBatchManager
{
    BEATS_DECLARE_SINGLETON(CSpriteFrameBatchManager);
public:
    void AddSpriteFrame(CSpriteFrame *frame, const kmMat4 &transform);
    void Render();
    void Clear();

private:
    std::map<GLuint, CSpriteFrameBatch *> m_batches;
    SharePtr<CShaderProgram> m_program;
};

#endif