#ifndef RENDER_SKIN_H__INCLUDE
#define RENDER_SKIN_H__INCLUDE

#include "Resource/Resource.h"
#ifdef SW_SKEL_ANIM
#include "AnimationController.h"
#endif
#include "CommonTypes.h"
class CSkeleton;

class CSkin : public CResource
{
public:
    CSkin();
    virtual ~CSkin();

    bool Load();
    bool Unload();
    virtual EResourceType GetType();

    GLuint GetVAO() const;
#ifdef SW_SKEL_ANIM
    void CalcSkelAnim(const CAnimationController::BoneMatrixMap &matrices);
#endif

protected:
    void buildVAO();
    void buildVBOVertex(const GLvoid *data, GLsizeiptr size);
    void buildVBOIndex(const GLvoid *data, GLsizeiptr size);

private:
    CSkeleton* m_pSkeleton;
    GLuint m_uVAO;
    GLuint m_uVBO[2];    //0:vertex 1:index
    CVertexPTB *m_vertices;
    size_t m_uVertexCount;
};

#endif