
#include "stdafx.h"
#include "ShaderUniformData.h"

CShaderUniformData::CShaderUniformData()
{
    m_Value = 0.0f;
}

CShaderUniformData::CShaderUniformData( CSerializer& serializer )
{
    m_Value = 0.0f;
    DECLARE_PROPERTY(serializer, m_Value, true, 0xFFFFFFFF, _T("Êý¾Ý"), NULL, NULL, NULL);
}

CShaderUniformData::~CShaderUniformData()
{

}

void CShaderUniformData::SetValue( float data )
{
    m_Value = data;
}

bool CShaderUniformData::operator==( const CShaderUniformData& other ) const
{
    return m_Value == other.m_Value;
}
