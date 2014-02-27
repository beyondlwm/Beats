#ifndef BEATS_UI_UIMANAGER_H__INCLUDE
#define BEATS_UI_UIMANAGER_H__INCLUDE

#include "UIPublicDef.h"
#include <set>

class CUIPageBase;
class CUISystemBase;

namespace CEGUI
{
    class Direct3D9Renderer;
    class GeometryBuffer;
}

class CUIManager
{
    BEATS_DECLARE_SINGLETON(CUIManager);

public:

    void Init();
    void Update(float deltaTime);

    void RegisterPage(EUIPageName name, CUIPageBase* page);

    CUIPageBase* GetUIPage(EUIPageName name);

    void SetPageActive(EUIPageName name, bool active);

    void ShowWinCursor(bool visible);



private:
    bool m_winCursorVisible;
    CUISystemBase* m_pSystem;
    std::map<EUIPageName, CUIPageBase*> m_pagePool;
    std::set<EUIPageName> m_activePage;
};

#endif
