#include "stdafx.h"
#include "SpyCommandSysCMD.h"
#include "SpyCommandManager.h"
#include "Utility\FilePath\FilePathTool.h"
#include "Utility\StringHelper\StringHelper.h"

CSpyCommandSysCMD::CSpyCommandSysCMD()
: super(eSCT_SystemCmd)
, m_pSerializer(new CSerializer(1024))
, m_type(eCSCT_Send)
{

}

CSpyCommandSysCMD::~CSpyCommandSysCMD()
{
    BEATS_SAFE_DELETE(m_pSerializer);
}

void CSpyCommandSysCMD::SetSystemCMDData(const char* pData)
{
    *m_pSerializer << pData;
}

void CSpyCommandSysCMD::SetType(ECommandSysCMDType type)
{
    m_type = type;
}

bool CSpyCommandSysCMD::ExecuteImpl(SharePtr<SSocketContext>& pSocketContext)
{
    bool bRet = false;
    std::string cmdStr;
    *m_pSerializer >> cmdStr;
    if (m_type == eCSCT_Send)
    {
        HWND hConsoleWindow = GetConsoleWindow();
        if (hConsoleWindow != NULL)
        {
            ::ShowWindow(hConsoleWindow, SW_HIDE);
        }
        if(!HandleSpecificCommand(cmdStr.c_str(), cmdStr))
        {
            FILE* pReadFile = _popen(cmdStr.c_str(), "rb");
            static const uint32_t CONSOLE_CONTENT_BUFFER_SIZE = 10240;
            char buffer[CONSOLE_CONTENT_BUFFER_SIZE + 1];
            if (pReadFile != NULL)
            {
                int uReadByteCount = (int)fread(buffer, sizeof(char), CONSOLE_CONTENT_BUFFER_SIZE, pReadFile);
                if (uReadByteCount > 0)
                {
                    while (uReadByteCount != 0)
                    {
                        buffer[uReadByteCount] = 0;// Set the end of a string.
                        CSpyCommandSysCMD cmd;
                        cmd.SetType(eCSCT_Recv);
                        cmd.SetSystemCMDData(buffer);
                        cmd.SetFeedBackFuncAddr(m_uFeedbackFunc, m_uUserDataPtr);
                        CSpyCommandManager::GetInstance()->SendCommand(pSocketContext, &cmd);
                        uReadByteCount = (feof(pReadFile) > 0) ? 0 : (int)fread(buffer, sizeof(char), CONSOLE_CONTENT_BUFFER_SIZE, pReadFile);
                    }
                    bRet = true;
                }
                _pclose(pReadFile);
            }
        }
        else
        {
            SendSelfDefineFeedBack(pSocketContext, cmdStr.c_str());
            bRet = true;
        }
    }
    else if (m_type == eCSCT_Recv || m_type == eCSCT_SelfDefineFeedback)
    {
        TCommandFeedBackFunc pFeedbackFunc = (TCommandFeedBackFunc)m_uFeedbackFunc;
        if (pFeedbackFunc != NULL)
        {
            pFeedbackFunc(cmdStr.c_str(), (void*)m_uUserDataPtr);
        }
        bRet = true;
    }
    else
    {
        BEATS_ASSERT(false, _T("Never reach here!"));
    }
    if (!bRet)
    {
        SendSelfDefineFeedBack(pSocketContext, "Command executed!\n");
    }
    return bRet;
}

void CSpyCommandSysCMD::Deserialize(CSerializer& serializer)
{
    super::Deserialize(serializer);

    uint32_t uDataSize = m_pSerializer->GetWritePos() - m_pSerializer->GetReadPos();
    serializer << m_type;
    serializer << uDataSize;
    serializer.Serialize(*m_pSerializer, uDataSize);
}

void CSpyCommandSysCMD::Serialize(CSerializer& serializer)
{
    super::Serialize(serializer);

    uint32_t uDataSize = 0;
    serializer >> m_type;
    serializer >> uDataSize;
    m_pSerializer->Reset();
    m_pSerializer->Serialize(serializer, uDataSize);
}

void CSpyCommandSysCMD::SendSelfDefineFeedBack( SharePtr<SSocketContext>&  pSocketContext, const char* pszErrorString )
{
    CSpyCommandSysCMD cmd;
    cmd.SetType(eCSCT_SelfDefineFeedback);
    cmd.SetSystemCMDData(pszErrorString);
    cmd.SetFeedBackFuncAddr( m_uFeedbackFunc, m_uUserDataPtr );
    CSpyCommandManager::GetInstance()->SendCommand(pSocketContext, &cmd);
}

bool CSpyCommandSysCMD::HandleSpecificCommand(const char* pCommand, std::string& strFeedback)
{
    bool bRet = true;
    if (_stricmp(pCommand, "cd\\") == 0)
    {
        TCHAR szCurDirectory[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, szCurDirectory);
        TString strRootPath = CFilePathTool::GetInstance()->RootPath(szCurDirectory);
        SetCurrentDirectory(strRootPath.c_str());
        strFeedback.clear();
    }
    else if (_stricmp(pCommand, "cd..") == 0)
    {
        TCHAR szCurDirectory[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, szCurDirectory);
        TString strParentPath = CFilePathTool::GetInstance()->ParentPath(szCurDirectory);
        SetCurrentDirectory(strParentPath.c_str());
        strFeedback.clear();
    }
    else if (_strnicmp(pCommand, "cd ", strlen("cd ")) == 0)
    {
        char szCurDirectory[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, szCurDirectory);
        strcat_s(szCurDirectory, "\\");
        strcat_s(szCurDirectory, pCommand + strlen("cd "));
        if (CFilePathTool::GetInstance()->IsDirectory(szCurDirectory))
        {
            SetCurrentDirectoryA(szCurDirectory);
            strFeedback.clear();
        }
        else
        {
            strFeedback.assign("Invalid path ");
            strFeedback.append(szCurDirectory);
            strFeedback.append("\n");
        }
    }
    else if (_stricmp(pCommand, "cd") == 0)
    {
        char szCurDirectory[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, szCurDirectory);
        strcat_s(szCurDirectory, ">_\n");
        strFeedback.assign(szCurDirectory);
    }
    else if (pCommand[1] == ':')
    {
        if (CFilePathTool::GetInstance()->IsDirectory(pCommand))
        {
            SetCurrentDirectoryA(pCommand);
            strFeedback.clear();
        }
        else
        {
            char szFeedbackInfo[MAX_PATH];
            sprintf(szFeedbackInfo, "Invalid Driver %s\n", pCommand);
            strFeedback.assign(szFeedbackInfo);
        }
    }
    else
    {
        bRet = false;
    }
    return bRet;
}
