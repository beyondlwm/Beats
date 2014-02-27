#ifndef BEATS_VERTEX_SHADER_H__INCLUDE
#define BEATS_VERTEX_SHADER_H__INCLUDE

#include "ShaderBase.h"

class CVertexShader : public CShaderBase
{
public:
    CVertexShader(IDirect3DVertexShader9* pShader);
    virtual ~CVertexShader();

    IDirect3DVertexShader9* Get();
    void Set(IDirect3DVertexShader9* pVertexShader);

private:
    IDirect3DVertexShader9* m_pVertexShader;
};

#endif