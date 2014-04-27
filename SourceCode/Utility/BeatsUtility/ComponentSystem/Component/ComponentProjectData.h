#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECTDATA_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECTDATA_H__INCLUDE

class CComponentProjectDirectory
{
public:
    CComponentProjectDirectory(CComponentProjectDirectory* pParent, const TString& strName);
    ~CComponentProjectDirectory();

    bool AddFile(const TString& strFileName, std::map<size_t, std::vector<size_t>>& conflictMap);
    CComponentProjectDirectory* AddDirectory(const TString& strDirectoryName);
    bool DeleteFile(size_t uFileId);
    bool DeleteDirectory(CComponentProjectDirectory* pDirectory);
    bool DeleteAll(bool bUpdateProject);

    const TString& GetName() const;
    const std::vector<CComponentProjectDirectory*>& GetChildren() const;
    const std::set<size_t>& GetFileList() const;
    CComponentProjectDirectory* GetParent() const;

    bool IsDirectory();

private:
    CComponentProjectDirectory* m_pParent;
    TString m_strName;
    std::set<size_t>* m_pFilesSet;
    std::vector<CComponentProjectDirectory*>* m_pChildrenVec;
};


#endif

