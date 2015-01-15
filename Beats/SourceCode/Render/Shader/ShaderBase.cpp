#include "stdafx.h"
#include "ShaderBase.h"

CShaderBase::CShaderBase()
: m_uIndex(0xFFFFFFFF)
{
}

CShaderBase::~CShaderBase()
{

}

uint32_t CShaderBase::GetIndex()
{
    return m_uIndex;
}

void CShaderBase::SetIndex(uint32_t uIndex)
{
    m_uIndex = uIndex;
}

const TString& CShaderBase::GetShaderFileName()
{
    return m_szShaderFileName;
}

void CShaderBase::SetShaderFileName(const TString& szFileName)
{
    m_szShaderFileName = szFileName;
}

const TString& CShaderBase::GetMainFuncName()
{
    return m_szMainFuncName;
}

void CShaderBase::SetMainFuncName(const TString& szMainFuncName)
{
    m_szMainFuncName = szMainFuncName;
}