#ifndef FCENGINEEDITOR_COMPONENTGRAPHICS_GL_H__INCLUDE
#define FCENGINEEDITOR_COMPONENTGRAPHICS_GL_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"

class CTexture;

class CComponentGraphic_GL : public CComponentGraphic
{
    typedef CComponentGraphic super;
public:
    CComponentGraphic_GL();
    virtual ~CComponentGraphic_GL();

    static void RecreateTextures();
    static void ReleaseTextures();
    virtual CComponentGraphic* Clone();

private:
    virtual void DrawHead(float cellSize);
    virtual void DrawDependencies(float cellSize);
    virtual void DrawDependencyLine(float cellSize, const CDependencyDescription* pDependency);
    virtual void DrawSelectedRect(float cellSize);

private:
    static CTexture* m_pTextures;
};

#endif