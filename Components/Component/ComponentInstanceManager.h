#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGER_H__INCLUDE

#include "../expdef.h"

#include "ComponentManagerBase.h"

class CComponentBase;
class CComponentProxy;
class CIdManager;
class CPropertyDescriptionBase;
class TiXmlElement;
class CSerializer;
class CDependencyDescription;
class CComponentBase;
struct SDependencyResolver;
class CComponentProject;
class CComponentGraphic;

class COMPONENTS_DLL_DECL CComponentInstanceManager : public CComponentManagerBase
{
    typedef CComponentManagerBase super;
    BEATS_DECLARE_SINGLETON(CComponentInstanceManager);
public:
    size_t GetVersion();
    void Import(CSerializer& serializer);
    virtual void ResolveDependency() override;
};

#endif