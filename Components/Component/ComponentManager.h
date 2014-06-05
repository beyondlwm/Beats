#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGER_H__INCLUDE

#include "../expdef.h"

#include "ComponentManagerBase.h"

class CComponentBase;
class CComponentEditorProxy;
class CIdManager;
class CPropertyDescriptionBase;
class TiXmlElement;
class CSerializer;
class CDependencyDescription;
class CComponentBase;
struct SDependencyResolver;
class CComponentProject;
class CComponentGraphic;

class COMPONENTS_DLL_DECL CComponentManager : public CComponentManagerBase
{
    BEATS_DECLARE_SINGLETON(CComponentManager);
public:
    void SerializeTemplateData(CSerializer& serializer);
    size_t GetVersion();
    void Import(CSerializer& serializer);
    virtual void ResolveDependency() override;

};

#endif