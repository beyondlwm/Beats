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
    void LoadFile(uint32_t uFileId, std::vector<CComponentBase*>& loadComponents);
    void CloseFile(uint32_t uFileId);
    CSerializer* GetFileSerializer() const;
    void SetCurLoadFileId(uint32_t uFileId);
    uint32_t GetCurLoadFileId() const;
    bool IsInClonePhase() const;
    void SetClonePhaseFlag(bool bInClonePhase);

    virtual void ResolveDependency() override;

private:
    void LoadDirectoryFiles(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>& loadComponents);
private:
    bool m_bInClonePhase;
    CSerializer* m_pSerializer;
};

#endif