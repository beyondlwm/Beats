#ifndef FCENGINEEDITOR_COMPONENTGRAPHICS_GL_H__INCLUDE
#define FCENGINEEDITOR_COMPONENTGRAPHICS_GL_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"

namespace FCGUI
{
    class FontFace;
}
class CTexture;

class CComponentGraphic_GL : public CComponentGraphic
{
    typedef CComponentGraphic super;
public:
    CComponentGraphic_GL();
    virtual ~CComponentGraphic_GL();

    void CreateTextures();
    virtual void GetDependencyPosition(size_t uDependencyIndex, int* pOutX, int* pOutY);
    virtual EComponentAeraRectType HitTestForAreaType( int x, int y, void** pReturnData);
    virtual CComponentGraphic* Clone();

private:
    virtual void DrawHead(float cellSize);
    virtual void DrawDependencies(float cellSize);
    virtual void DrawDependencyLine(float cellSize, const CDependencyDescription* pDependency);
    virtual void DrawSelectedRect(float cellSize);

private:
    FCGUI::FontFace* m_pFont;
    SharePtr<CTexture> m_pTextures[eCT_Count];
};

#endif