#ifndef RENDER_SHADERUNIFORM_H__INCLUDE
#define RENDER_SHADERUNIFORM_H__INCLUDE

#include "ShaderUniformData.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

enum EShaderUniformType
{
    eSUT_1i,
    eSUT_2i,
    eSUT_3i,
    eSUT_4i,

    eSUT_1f,
    eSUT_2f,
    eSUT_3f,
    eSUT_4f,

    eSUT_Matrix2f,
    eSUT_Matrix3f,
    eSUT_Matrix4f,

    eSUT_Count,
    eSUT_Force32Bit = 0xFFFFFF
};

class CShaderUniform : public CComponentBase
{
    typedef std::vector< CShaderUniformData* > TUniformData;
    DECLARE_REFLECT_GUID(CShaderUniform, 0xD707AB1C, CComponentBase)
public:
    CShaderUniform();
    CShaderUniform( CSerializer& serializer );
    ~CShaderUniform();

    void SetValue( const TString& name, EShaderUniformType type, const TUniformData& data );

    void SendUniform( );

    bool operator==( const CShaderUniform& other ) const;

private:
    bool CheckType( );
private:
    EShaderUniformType m_type;
    TUniformData m_data;
    TString m_strName;
};
#endif