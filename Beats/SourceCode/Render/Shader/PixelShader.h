#ifndef BEATS_PIXEL_SHADER_H__INCLUDE
#define BEATS_PIXEL_SHADER_H__INCLUDE

#include "ShaderBase.h"

class CPixelShader : public CShaderBase
{
public:
    CPixelShader(IDirect3DPixelShader9* pPixelShader);
    virtual ~CPixelShader();

    IDirect3DPixelShader9* Get();
    void Set(IDirect3DPixelShader9* pPixelShader);

private:
    IDirect3DPixelShader9* m_pPixelShader;
};

#endif