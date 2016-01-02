#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCEMANAGER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCEMANAGER_H__INCLUDE

#include "../expdef.h"

#include "ComponentManagerBase.h"
class CSerializer;
class CComponentProjectDirectory;
class COMPONENTS_DLL_DECL CComponentInstanceManager : public CComponentManagerBase
{
    typedef CComponentManagerBase super;
    BEATS_DECLARE_SINGLETON(CComponentInstanceManager);
public:
    uint32_t GetVersion();
    CSerializer* Import(const TCHAR* pszFilePath);
    void SwitchFile(uint32_t uFileId);
    virtual void LoadFile(uint32_t uFileId, std::vector<CComponentBase*>* pLoadComponents) override;
    virtual void UnloadFile(uint32_t uFileId, std::vector<CComponentBase*>* pUnloadComponents) override;
    virtual void CloseFile(uint32_t uFileId) override;
    CSerializer* GetFileSerializer() const;
    virtual void ResolveDependency() override;
    bool IsInClonePhase() const;
    void SetClonePhaseFlag(bool bInClonePhase);
    bool IsInLoadingPhase() const;
    void SetLoadPhaseFlag(bool bInLoadPhase);
    std::mutex& GetFileSerializerMutex();

private:
    void LoadDirectoryFiles(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>& loadComponents);
private:
    bool m_bInClonePhase = false;
    bool m_bInLoadingPhase = false;
    CSerializer* m_pSerializer;
    std::mutex m_fileSerializerMutex;
};

#endif