#ifndef BEATS_SPY_COMMAND_FILETRANSFER_SPYCOMMANDUPLOADFILE_H__INCLUDE
#define BEATS_SPY_COMMAND_FILETRANSFER_SPYCOMMANDUPLOADFILE_H__INCLUDE

#include "SpyCommandBase.h"

class CFileFilter;

class CSpyCommandUploadFile : public CSpyCommandBase
{
    typedef CSpyCommandBase super;
public:
    CSpyCommandUploadFile();
    virtual ~CSpyCommandUploadFile();

    void SetUploadFile(const std::vector<TString>& uploadFileNames);
    void SetStorePath(const TCHAR* pszStorePath);
    void SetFileFilter(CFileFilter* pFileFilter);

    virtual bool ExecuteImpl(SharePtr<SSocketContext>&  pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

private:
    CFileFilter* m_pFileFilter;
    std::vector<TString> m_uploadFileNames;
    TString m_storePath;
};

#endif