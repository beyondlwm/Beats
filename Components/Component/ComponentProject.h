#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECT_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECT_H__INCLUDE

#include "../expdef.h"

class TiXmlElement;
class CComponentProjectDirectory;
struct SFileDataLayout
{
    SFileDataLayout()
        : m_uStartPos(0)
        , m_uDataSize(0)
    {

    }
    SFileDataLayout(uint32_t uStartPos, uint32_t uDataSize)
        : m_uStartPos(uStartPos)
        , m_uDataSize(uDataSize)
    {

    }
    uint32_t m_uStartPos;
    uint32_t m_uDataSize;
};

class COMPONENTS_DLL_DECL CComponentProject
{
public:
    CComponentProject();
    ~CComponentProject();

    CComponentProjectDirectory* LoadProject(const TCHAR* pszProjectFile, std::map<uint32_t, std::vector<uint32_t> >& conflictIdMap);
    bool CloseProject();
    void SaveProject();

    // Register file only when: 1. Load Project. 2.Add File. 3. Reload the file when close file(No matter save or not).
    // Unregister file only when: 1. Delete file. 2. Reset the id when close file.
    uint32_t RegisterFile(CComponentProjectDirectory* pDirectory, const TString& strFileName, std::map<uint32_t, std::vector<uint32_t> >& failedId, uint32_t uSpecifyFileId = 0xFFFFFFFF);
    bool AnalyseFile(const TString& strFileName, std::map<uint32_t, std::vector<uint32_t> >& outResult);
    bool UnregisterFile(uint32_t uFileID);
    void ReloadFile(uint32_t uFileID);

    // Register/Unregister component only when we need to change some info in static record.
    // Register component only when: 1. Register file. 2. Resolve conflict.
    // Unregister component only when: Resolve conflict.
    void RegisterComponent(uint32_t uFileID, uint32_t componentGuid, uint32_t componentId);
    void UnregisterComponent(uint32_t componentId);

    CComponentProjectDirectory* GetRootDirectory() const;
    void SetRootDirectory(CComponentProjectDirectory* pDirectory);

    const TString& GetProjectFilePath() const;
    const TString& GetProjectFileName() const;

    const TString& GetComponentFileName(uint32_t id) const;
    uint32_t GetComponentFileId(const TString& strFileName) const;

    uint32_t QueryFileId(uint32_t uComponentId, bool bOnlyInProjectFile);
    void ResolveIdForFile(uint32_t uFileId, uint32_t idToResolve, bool bKeepId);

    void RegisterPropertyMaintainInfo(uint32_t uComponentGuid, const TString& strOriginPropertyName, const TString& strReplacePropertyName);
    bool GetReplacePropertyName(uint32_t uComponentGuid, const TString& strOriginPropertyName, TString& strResult);

    const std::vector<TString>* GetFileList() const;

    void RegisterFileLayoutInfo(uint32_t uFileId, uint32_t uStartPos, uint32_t uDataLength);
    bool QueryFileLayoutInfo(uint32_t uFileId, uint32_t& uStartPos, uint32_t& uDataLength) const;

    void SetStartFile(uint32_t uFileId);
    uint32_t GetStartFile() const;
    CComponentProjectDirectory* FindProjectDirectory(const TString& strPath, bool bAbsoluteOrLogicPath) const;
    CComponentProjectDirectory* FindProjectDirectoryById(uint32_t uFileId);

    uint32_t QueryComponentGuid(uint32_t uId);

    std::map<uint32_t, std::vector<uint32_t> >* GetFileToComponentMap() const;
    std::map<uint32_t, std::set<uint32_t>>* GetIdToReferenceMap() const;
    std::map<uint32_t, uint32_t>* GetReferenceMap();
    std::map<uint32_t, CComponentProjectDirectory*>* GetFileToDirectoryMap() const;
    std::map<uint32_t, uint32_t>* GetComponentToFileMap() const;
    std::map<uint32_t, std::vector<uint32_t> >* GetTypeToComponentMap() const;

private:
    void LoadXMLProject(TiXmlElement* pNode, CComponentProjectDirectory* pProjectDirectory, TString& strStartFilePath, std::map<uint32_t, std::vector<uint32_t> >& conflictIdMap);
    void SaveProjectFile( TiXmlElement* pParentNode, const CComponentProjectDirectory* p);

private:
    CComponentProjectDirectory* m_pProjectDirectory;
    uint32_t m_uStartFileId;
    std::vector<TString>* m_pComponentFiles;
    std::map<uint32_t, uint32_t>* m_pComponentToTypeMap;
    std::map<uint32_t, uint32_t>* m_pComponentToFileMap;
    std::map<uint32_t, std::vector<uint32_t> >* m_pFileToComponentMap;
    std::map<uint32_t, CComponentProjectDirectory*>* m_pFileToDirectoryMap;
    std::map<uint32_t, std::vector<uint32_t> >* m_pTypeToComponentMap;
    // Store property replace info, uint32_t is the guid of component, map is the old property name and new property name.
    std::map<uint32_t, std::map<TString, TString> >* m_pPropertyMaintainMap;
    // This member only available in game mode, to save the info about file data layout in the export file.
    std::map<uint32_t, SFileDataLayout>* m_pFileDataLayout;
    // Key is the proxy id of the real component, and the value is the reference ids, this data collect from XML, it is static info.
    std::map<uint32_t, std::set<uint32_t>>* m_pIdToReferenceMap;
    // Key is the reference id, and the value is the proxy of real component.
    std::map<uint32_t, uint32_t>* m_pReferenceToIdMap;

    TString m_strProjectFilePath;
    TString m_strProjectFileName;
};

#endif