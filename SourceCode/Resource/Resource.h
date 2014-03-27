#ifndef RESOURCE_RESOURCE_H__INCLUDE
#define RESOURCE_RESOURCE_H__INCLUDE

class CResource
{
public:
    CResource();
    virtual ~CResource();

    const TString& GetFilePath() const;
    void SetFilePath(const TString& str);
    
    bool IsLoaded() const;
    void SetLoadedFlag(bool bFlag);

    virtual EResourceType GetType() = 0;
    virtual bool Load() = 0;
    virtual bool Unload() = 0;
     
protected:
    bool m_bIsLoaded;
    TString m_strPath;
};

#endif