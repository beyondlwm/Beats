#ifndef BEATS_GAME_UI_UIPAGEBASE_H__INCLUDE
#define BEATS_GAME_UI_UIPAGEBASE_H__INCLUDE

#include "UIPublicDef.h"
#include "CEGUI.h"

namespace CEGUI
{
    class Window;
}

class CUIPageBase
{
public:
    CUIPageBase(EUIPageName pageName);
    virtual ~CUIPageBase();

public:
    void Open();
    void Close();
    bool IsOpen();

    void SetAllComponentsVisible(bool visible);

    CEGUI::Window* GetRoot() const;
    void SetRoot(CEGUI::Window* pRoot);
    void Activate(bool active);

    virtual void InitUIEvent();
    virtual void Init();
    virtual void Init(const TCHAR* layoutFileName, int compCount, const TCHAR* compNames[]);
    virtual void Update() = 0;

protected:
    void InitComponents(int count, const TCHAR* names[]);

private:
    virtual bool OpenImpl() = 0;
    virtual bool CloseImpl() = 0;
protected:
    bool m_isOpen;
    EUIPageName m_name;
    CUIPageBase* m_pParent;
    CEGUI::Window* m_pRootWindow;
    std::vector<CEGUI::Window*> m_pComponents;
};

#endif