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
    void SwitchFile(size_t uFileId);
    void LoadFile(size_t uFileId, std::vector<CComponentBase*>& loadComponents);
    void CloseFile(size_t uFileId);
    CSerializer* GetFileSerializer() const;
    virtual void ResolveDependency() override;

private:
    void LoadDirectoryFiles(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>& loadComponents);
private:
    size_t m_uCurLoadFileId;
    size_t m_uCurWorkingFileId;
    CSerializer* m_pSerializer;
    std::set<size_t> m_loadedFiles;
};

#endif