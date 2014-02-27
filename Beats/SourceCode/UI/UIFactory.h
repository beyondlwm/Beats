#ifndef BEATS_UI_UIFACTORY_H__INCLUDE
#define BEATS_UI_UIFACTORY_H__INCLUDE

#include "UIPublicDef.h"

class CUIPageBase;

class CUIFactory
{
    BEATS_DECLARE_SINGLETON(CUIFactory);

public:
    void Init();
    CUIPageBase* CreateUI(EUIPageName name);
};

#endif