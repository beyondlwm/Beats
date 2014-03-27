#ifndef RENDER_MATERIAL_H__INCLUDE
#define RENDER_MATERIAL_H__INCLUDE

#include "resource/Resource.h"
#include "render/Texture.h"
#include "Utility/BeatsUtility/Serializer.h"

class CRenderState;

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

struct SShaderUniform
{
    SShaderUniform(size_t count, EShaderUniformType type)
        : m_uParamCount(count)
        , m_type(type)
    {
    }
    size_t m_uParamCount;
    EShaderUniformType m_type;
    CSerializer m_data;
#ifdef _DEBUG
    TString m_strName;
#endif
};

class CMaterial : public CResource
{
public:
    CMaterial();
    virtual ~CMaterial();

    virtual EResourceType GetType() override {return eRT_Material;}
    virtual bool Load() override;
    virtual bool Unload() override;

    void Use();
private:
    void SendUniform();

private:
    CRenderState* m_pRenderState;
    std::vector<SharePtr<CTexture>> m_textures;
    std::map<size_t, SShaderUniform> m_uniforms;
};

#endif