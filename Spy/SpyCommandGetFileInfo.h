#ifndef BEATS_SPY_COMMAND_SPYCOMMANDGETFILEINFO_H__INCLUDE
#define BEATS_SPY_COMMAND_SPYCOMMANDGETFILEINFO_H__INCLUDE

#include "SpyCommandBase.h"

enum EFileInfoType
{
    eFIT_File,
    eFIT_Directory,
    eFIT_Disk,
    eFIT_Error,

    eFIT_Count,
    eFIT_Force32Bit = 0xFFFFFFFF
};
struct SDirectory;
struct SDiskInfo;

class CSpyCommandGetFileInfo : public CSpyCommandBase
{
    typedef CSpyCommandBase super;
public:
    CSpyCommandGetFileInfo();
    virtual ~CSpyCommandGetFileInfo();

    void SetFileName(const TCHAR* pszFileName);

    virtual bool ExecuteImpl(SharePtr<SSocketContext>&  pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

private:
    const TCHAR* GetDiskTypeString(char diskType);

private:
    TString m_fileName;
    TString m_errorInfo;
    EFileInfoType m_type;
    SDirectory* m_pDirectory;
    SDiskInfo* m_pDiskInfo;
    WIN32_FILE_ATTRIBUTE_DATA m_fileData;
};
#endif