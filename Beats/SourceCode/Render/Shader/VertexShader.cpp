#include "stdafx.h"
#include "VertexShader.h"

CVertexShader::CVertexShader(IDirect3DVertexShader9* pShader)
: m_pVertexShader(pShader)
{

}

CVertexShader::~CVertexShader()
{
    if (m_pVertexShader != NULL)
    {
        m_pVertexShader->Release();
        m_pVertexShader = NULL;
    }
}

IDirect3DVertexShader9* CVertexShader::Get()
{
    return m_pVertexShader;
}

void CVertexShader::Set(IDirect3DVertexShader9* pVertexShader)
{
    m_pVertexShader = pVertexShader;
}
