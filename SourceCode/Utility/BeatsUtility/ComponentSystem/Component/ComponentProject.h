#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECT_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECT_H__INCLUDE

class TiXmlElement;
class CComponentProjectDirectory;

class CComponentProject
{
public:
    CComponentProject();
    ~CComponentProject();

    CComponentProjectDirectory* LoadProject(const TCHAR* pszProjectFile, std::map<size_t, std::vector<size_t>>& conflictIdMap);
    bool CloseProject();
    void SaveProject();

    // Register file only when: 1. Load Project. 2.Add File. 3. Reload the file when close file(No matter save or not).
    // Unregister file only when: 1. Delete file. 2. Reset the id when close file.
    size_t RegisterFile(const TString& strFileName, std::map<size_t, std::vector<size_t>>& failedId, size_t uSpecifyFileId = 0xFFFFFFFF);
    bool AnalyseFile(const TString& strFileName, std::map<size_t, std::vector<size_t>>& outResult);
    bool UnregisterFile(size_t uFileID);
    void ReloadFile(size_t uFileID);

    // Register/Unregister component only when we need to change some info in static record.
    // Register component only when: 1. Register file. 2. Resolve conflict.
    // Unregister component only when: Resolve conflict.
    void RegisterComponent(size_t uFileID, size_t componentGuid, size_t componentId);
    void UnregisterComponent(size_t componentId);

    CComponentProjectDirectory* GetRootDirectory() const;

    const TString& GetProjectFilePath() const;
    const TString& GetProjectFileName() const;

    TString GetComponentFileName(size_t id) const;
    size_t GetComponentFileId(const TString& strFileName) const;

    size_t QueryFileId(size_t uComponentId, bool bOnlyInProjectFile);
    void ResolveIdForFile(size_t uFileId, size_t idToResolve, bool bKeepId);

    void RegisterPropertyMaintainInfo(size_t uComponentGuid, const TString& strOriginPropertyName, const TString& strReplacePropertyName);
    bool GetReplacePropertyName(size_t uComponentGuid, const TString& strOriginPropertyName, TString& strResult);

private:
    void LoadXMLProject(TiXmlElement* pNode, CComponentProjectDirectory* pProjectDirectory, std::map<size_t, std::vector<size_t>>& conflictIdMap);
    void SaveProjectFile( TiXmlElement* pParentNode, const CComponentProjectDirectory* p);

private:
    CComponentProjectDirectory* m_pProjectData;
    std::vector<TString>* m_pComponentFiles;
    std::map<size_t, size_t>* m_pComponentToTypeMap;
    std::map<size_t, size_t>* m_pComponentToFileMap;
    std::map<size_t, std::vector<size_t>>* m_pFileToComponentMap;
    std::map<size_t, std::vector<size_t>>* m_pTypeToComponentMap;
    std::map<size_t, std::map<TString, TString>>* m_pPropertyMaintainMap;

    TString m_strProjectFilePath;
    TString m_strProjectFileName;
};

#endif