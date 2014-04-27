#ifndef RENDER_SHADERUNIFORMDATA_H__INCLUDE
#define RENDER_SHADERUNIFORMDATA_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

class CShaderUniformData : public CComponentBase
{
    DECLARE_REFLECT_GUID(CShaderUniformData, 0xD807AB1C, CComponentBase)
public:
    CShaderUniformData();
    CShaderUniformData( CSerializer& serializer );
    ~CShaderUniformData();

    void SetValue( float data );

    bool operator==( const CShaderUniformData& other ) const;
private:
    float m_Value;
};

#endif