#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTREFERENCE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTREFERENCE_H__INCLUDE

#include "ComponentProxy.h"

class CComponentReference : public CComponentProxy
{
    typedef CComponentProxy super;
public:
    CComponentReference(CComponentProxy* pComponentHost);
    virtual ~CComponentReference();

    CComponentProxy* GetHostProxy();
    virtual size_t GetGuid() const override;
    virtual size_t GetParentGuid() const override;
    virtual const TCHAR* GetClassStr() const override;

private:
    CComponentProxy* m_pHostProxy;
};

#endif