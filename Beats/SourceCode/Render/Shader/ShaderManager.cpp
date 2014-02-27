#include "stdafx.h"
#include "ShaderManager.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "../../Utility/StringHelper/StringHelper.h"

CShaderManager* CShaderManager::m_pInstance = NULL;

CShaderManager::CShaderManager()
: m_pRenderDevice(NULL)
, m_pCurrentVertexShader(NULL)
, m_pCurrentPixelShader(NULL)
{

}

CShaderManager::~CShaderManager()
{
    for (size_t i = 0; i < m_vertexShaderPool.size(); ++i)
    {
        BEATS_SAFE_DELETE(m_vertexShaderPool[i]);
    }
    m_vertexShaderPool.clear();
    for (size_t i = 0; i < m_pixelShaderPool.size(); ++i)
    {
        BEATS_SAFE_DELETE(m_pixelShaderPool[i]);
    }
    m_pixelShaderPool.clear();
}

void CShaderManager::Initialize( IDirect3DDevice9* pDevice )
{
    m_pRenderDevice = pDevice;
    //CPixelShader* pPixelShader = NULL;
    //size_t uId = 0;
    //bool bTemp = CreatePixelShaderFromFile("E:\\Work\\Project\\EraFable\\XSHADERs\\xblast.cc", "ps_main_blast_blur", "ps_3_0", NULL, &pPixelShader, &uId);
    //BEATS_ASSERT(bTemp == true);
}
bool CShaderManager::CreateVertexShaderFromFile(const char* pszFilePath, 
                                                const char* pszFuncName,
                                                const char* pszProfile,
                                                const D3DXMACRO* pDefine,
                                                CVertexShader** ppVertexShader,
                                                size_t* pRegisterId/* = NULL*/)
{
    bool bRet = false;
    LPD3DXBUFFER pErrorMsgs = NULL;
    LPD3DXCONSTANTTABLE pConstantTable = NULL;
    LPD3DXBUFFER pShader = NULL;
    if (CompileShaderFromFile(pszFilePath, pszFuncName, pszProfile, pDefine, &pShader, &pErrorMsgs, &pConstantTable))
    {
        IDirect3DVertexShader9* pVertexShader = NULL;
        if (CreateVertexShader((DWORD*)(pShader->GetBufferPointer()), &pVertexShader))
        {
            CVertexShader* pBeatsVertexShader = new CVertexShader(pVertexShader);
            TCHAR szTCharBuffer[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(pszFuncName, szTCharBuffer, MAX_PATH);
            pBeatsVertexShader->SetMainFuncName(szTCharBuffer);
            CStringHelper::GetInstance()->ConvertToTCHAR(pszFilePath, szTCharBuffer, MAX_PATH);
            pBeatsVertexShader->SetShaderFileName(szTCharBuffer);

            *ppVertexShader = pBeatsVertexShader;

            if (pRegisterId != NULL)
            {
                m_vertexShaderPool.push_back(pBeatsVertexShader);
                *pRegisterId = m_vertexShaderPool.size() - 1;
                pBeatsVertexShader->SetIndex(*pRegisterId);
            }
            bRet = true;
        }
        BEATS_SAFE_RELEASE(pErrorMsgs);
        BEATS_SAFE_RELEASE(pConstantTable);
        BEATS_SAFE_RELEASE(pShader);
    }
    return bRet;
}
bool CShaderManager::CreatePixelShaderFromFile(const char* pszFilePath, 
                                               const char* pszFuncName,
                                               const char* pszProfile,
                                               const D3DXMACRO* pDefine,
                                               CPixelShader** ppPixelShader,
                                               size_t* pRegisterId/* = NULL*/)
{
    bool bRet = false;
    LPD3DXBUFFER pErrorMsgs = NULL;
    LPD3DXCONSTANTTABLE pConstantTable = NULL;
    LPD3DXBUFFER pShader = NULL;
    if (CompileShaderFromFile(pszFilePath, pszFuncName, pszProfile, pDefine, &pShader, &pErrorMsgs, &pConstantTable))
    {
        IDirect3DPixelShader9* pPixelShader = NULL;
        if (CreatePixelShader((DWORD*)(pShader->GetBufferPointer()), &pPixelShader))
        {
            CPixelShader * pBeatsPixelShader = new CPixelShader(pPixelShader);
            TCHAR szTCharBuffer[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(pszFuncName, szTCharBuffer, MAX_PATH);
            pBeatsPixelShader->SetMainFuncName(szTCharBuffer);
            CStringHelper::GetInstance()->ConvertToTCHAR(pszFilePath, szTCharBuffer, MAX_PATH);
            pBeatsPixelShader->SetShaderFileName(szTCharBuffer);

            *ppPixelShader = pBeatsPixelShader;

            if (pRegisterId != NULL)
            {
                m_pixelShaderPool.push_back(pBeatsPixelShader);
                *pRegisterId = m_pixelShaderPool.size() - 1;
                pBeatsPixelShader->SetIndex(*pRegisterId);
            }
            bRet = true;
        }
        BEATS_SAFE_RELEASE(pErrorMsgs);
        BEATS_SAFE_RELEASE(pConstantTable);
        BEATS_SAFE_RELEASE(pShader);
    }
    return bRet;
}

bool CShaderManager::CompileShaderFromFile(const char* pszFilePath, 
                                           const char* pszFuncName,
                                           const char* pszProfile,
                                           const D3DXMACRO* pDefine,
                                           LPD3DXBUFFER* ppShader,
                                           LPD3DXBUFFER* ppErrorMsgs,
                                           LPD3DXCONSTANTTABLE* ppConstantTable)
{
    bool bRet = false;
    FILE* pFile = fopen(pszFilePath, "rt");
    BEATS_ASSERT(pFile != NULL, _T("The file %s doesn't exists for compiling shader!"), pszFilePath);
    if (pFile != NULL)
    {
        fseek(pFile, 0, SEEK_END);
        size_t uFileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);
        char* pFileDataString = new char[uFileSize];
        fread(pFileDataString, 1, uFileSize, pFile);
        fclose(pFile);

        HRESULT hr = D3DXCompileShader((LPCSTR)pFileDataString,
            uFileSize, 
            pDefine, 
            NULL, 
            pszFuncName, 
            pszProfile, 
            D3DXSHADER_OPTIMIZATION_LEVEL3, 
            ppShader, 
            ppErrorMsgs, 
            ppConstantTable);
        bRet = hr == D3D_OK;
#ifdef _DEBUG
        if (!bRet)
        {
            TCHAR szInfomation[10240];
            CStringHelper::GetInstance()->ConvertToTCHAR((char*)(*ppErrorMsgs)->GetBufferPointer(), szInfomation, 10240);
            BEATS_ASSERT(bRet, _T("Compiling shader failed!\nerror: %d\n%s\n%s\n%s"), 
                hr,
                szInfomation, 
                DXGetErrorString(hr),
                DXGetErrorDescription(hr));
        }
#endif
        BEATS_SAFE_DELETE_ARRAY(pFileDataString);
    }
    return bRet;
}

bool CShaderManager::CreateVertexShader(CONST DWORD * pFunction, IDirect3DVertexShader9** ppShader)
{
    HRESULT hr = m_pRenderDevice->CreateVertexShader(pFunction, ppShader);
    bool bRet = hr == D3D_OK;
    BEATS_ASSERT(bRet, _T("CreateVertexShader failed!\nerror %d\n%s\n%s"), hr, DXGetErrorString(hr), DXGetErrorDescription(hr));
    return bRet;
}
bool CShaderManager::CreatePixelShader(CONST DWORD * pFunction, IDirect3DPixelShader9** ppShader)
{
    HRESULT hr = m_pRenderDevice->CreatePixelShader(pFunction, ppShader);
    bool bRet = hr == D3D_OK;
    BEATS_ASSERT(bRet, _T("CreatePixelShader failed!\nerror %d\n%s\n%s"), hr, DXGetErrorString(hr), DXGetErrorDescription(hr));
    return bRet;
}

bool CShaderManager::SetVertexShader(CVertexShader* pVertexShader)
{
    HRESULT hr = m_pRenderDevice->SetVertexShader(pVertexShader->Get());
    bool bRet = hr == D3D_OK;
    if (bRet)
    {
        BEATS_ASSERT(pVertexShader == NULL || pVertexShader == m_vertexShaderPool[pVertexShader->GetIndex()],
                    _T("Vertex Shader %s has got an un-matched id!"), pVertexShader->GetShaderFileName().c_str());
        m_pCurrentVertexShader = pVertexShader;
    }
    BEATS_ASSERT(bRet, _T("SetVertexShader failed!\nError %d\n%s\n%s"), hr, DXGetErrorString(hr), DXGetErrorDescription(hr));
    
    return bRet;
}

bool CShaderManager::SetPixelShader(CPixelShader* pPixelShader)
{
    HRESULT hr = m_pRenderDevice->SetPixelShader(pPixelShader->Get());
    bool bRet = hr == D3D_OK;
    if (bRet)
    {
        BEATS_ASSERT(pPixelShader == NULL || pPixelShader == m_pixelShaderPool[pPixelShader->GetIndex()],
            _T("Pixel Shader %s has got an un-matched id!"), pPixelShader->GetShaderFileName().c_str());
        m_pCurrentPixelShader = pPixelShader;
    }
    BEATS_ASSERT(bRet, _T("SetPixelShader failed!\nError %d\n%s\n%s"), hr, DXGetErrorString(hr), DXGetErrorDescription(hr));

    return bRet;
}

CVertexShader* CShaderManager::GetVertexShader() const
{
    return m_pCurrentVertexShader;
}

CPixelShader* CShaderManager::GetPixelShader() const
{
    return m_pCurrentPixelShader;
}

CVertexShader* CShaderManager::GetVertexShaderByID(UINT id) const
{
    BEATS_ASSERT(id < m_vertexShaderPool.size(), _T("Invalid id of vertex shader %d!"), id);
    return m_vertexShaderPool[id];
}

CPixelShader* CShaderManager::GetPixelShaderByID(UINT id) const
{
    BEATS_ASSERT(id < m_pixelShaderPool.size(), _T("Invalid id of pixel shader %d!"), id);
    return m_pixelShaderPool[id];

}
