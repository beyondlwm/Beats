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
    bool CalcSwitchFile(uint32_t uFileId, std::vector<uint32_t>& loadFiles, std::vector<uint32_t>& unloadFiles);
    void LoadFile(uint32_t uFileId, std::vector<CComponentBase*>& loadComponents);
    void CloseFile(uint32_t uFileId);
    CSerializer* GetFileSerializer() const;
    const std::set<uint32_t>& GetLoadedFiles() const;
    void SetCurLoadFileId(uint32_t uFileId);
    uint32_t GetCurLoadFileId() const;
    bool IsInClonePhase() const;
    void SetClonePhaseFlag(bool bInClonePhase);

    virtual void ResolveDependency() override;

private:
    void LoadDirectoryFiles(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>& loadComponents);
private:
    bool m_bInClonePhase;
    uint32_t m_uCurLoadFileId;
    uint32_t m_uCurWorkingFileId;
    CSerializer* m_pSerializer;
    std::set<uint32_t> m_loadedFiles;
};

#endif