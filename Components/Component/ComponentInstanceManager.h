#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGER_H__INCLUDE

#include "../expdef.h"

#include "ComponentManagerBase.h"
class CSerializer;
class CComponentProjectDirectory;
class COMPONENTS_DLL_DECL CComponentInstanceManager : public CComponentManagerBase
{
    typedef CComponentManagerBase super;
    BEATS_DECLARE_SINGLETON(CComponentInstanceManager);
public:
    size_t GetVersion();
    CSerializer* Import(const TCHAR* pszFilePath);
    void LoadDirectory(CComponentProjectDirectory* pDirectory);
    CSerializer* GetFileSerializer() const;
    virtual void ResolveDependency() override;

private:
    void LoadDirectoryFiles(CComponentProjectDirectory* pDirectory);
private:
    CSerializer* m_pSerializer;
};

#endif