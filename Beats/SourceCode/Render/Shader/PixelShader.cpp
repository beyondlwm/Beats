#include "stdafx.h"
#include "PixelShader.h"

CPixelShader::CPixelShader(IDirect3DPixelShader9* pPixelShader)
: m_pPixelShader(pPixelShader)
{

}

CPixelShader::~CPixelShader()
{
    if (m_pPixelShader != NULL)
    {
        m_pPixelShader->Release();
        m_pPixelShader = NULL;
    }
}

IDirect3DPixelShader9* CPixelShader::Get()
{
    return m_pPixelShader;
}

void CPixelShader::Set(IDirect3DPixelShader9* pPixelShader)
{
    m_pPixelShader = pPixelShader;
}
