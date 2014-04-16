#ifndef BEATS_COMPONENTS_DEPENDENCYDESCRIPTIONLINE_H__INCLUDE
#define BEATS_COMPONENTS_DEPENDENCYDESCRIPTIONLINE_H__INCLUDE

#include "expdef.h"
class CDependencyDescription;
class CComponentEditorProxy;
struct SVertex;

class COMPONENTS_DLL_DECL CDependencyDescriptionLine
{
public:
    CDependencyDescriptionLine(CDependencyDescription* pOwner, size_t uIndex, CComponentEditorProxy* pTo);
    ~CDependencyDescriptionLine();

    void UpdateRect(float cellSize, bool bInverseY = false);
    const SVertex* GetRectArray();
    const SVertex* GetArrowRectArray();
    CDependencyDescription* GetOwnerDependency();
    CComponentEditorProxy* GetConnectedComponent();
    void SetConnectComponent(CComponentEditorProxy* pComponentTo);
    bool IsSelected();
    void SetSelected(bool bFlag);
    size_t GetIndex();
    void SetIndex(size_t uIndex);
    bool HitTest(float x, float y);

private:
    bool m_bIsSelected;
    size_t m_uIndex;
    CDependencyDescription* m_pOwner;
    CComponentEditorProxy* m_pConnectedComponent;
    SVertex* m_pRect; // 0 = left top, 1 = left down, 2 = right top, 3 = right down.
    SVertex* m_pArrowRect;
};
#endif