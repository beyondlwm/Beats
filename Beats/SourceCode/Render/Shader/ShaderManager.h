#ifndef BEATS_SHADER_MANAGER_H__INCLUDE
#define BEATS_SHADER_MANAGER_H__INCLUDE
 
class CVertexShader;
class CPixelShader;

class CShaderManager
{
    BEATS_DECLARE_SINGLETON(CShaderManager);
public:

    void Initialize(IDirect3DDevice9* pDevice);
    bool CreateVertexShaderFromFile(const char* pszFilePath, 
                                    const char* pszFuncName,
                                    const char* pszProfile,
                                    const D3DXMACRO* pDefine,
                                    CVertexShader** ppShader,
                                    size_t* pRegisterId = NULL);

    bool CreatePixelShaderFromFile(const char* pszFilePath, 
                                    const char* pszFuncName,
                                    const char* pszProfile,
                                    const D3DXMACRO* pDefine,
                                    CPixelShader** ppPixelShader,
                                    size_t* pRegisterId = NULL);

    bool SetVertexShader(CVertexShader* pVertexShader);
    bool SetPixelShader(CPixelShader* pPixelShader);
    CVertexShader* GetVertexShader() const;
    CPixelShader* GetPixelShader() const;

    CVertexShader* GetVertexShaderByID(UINT id) const;
    CPixelShader* GetPixelShaderByID(UINT id) const;

private:
    bool CompileShaderFromFile(const char* pszFilePath, 
                                const char* pszFuncName,
                                const char* pszProfile,
                                const D3DXMACRO* pDefine,
                                LPD3DXBUFFER* ppShader,
                                LPD3DXBUFFER* ppErrorMsgs,
                                LPD3DXCONSTANTTABLE * ppConstantTable);

    bool CreateVertexShader(CONST DWORD * pFunction, IDirect3DVertexShader9** ppShader);
    bool CreatePixelShader(CONST DWORD * pFunction, IDirect3DPixelShader9** ppShader);

private:
    CVertexShader* m_pCurrentVertexShader;
    CPixelShader* m_pCurrentPixelShader;
    IDirect3DDevice9* m_pRenderDevice;
    std::vector<CVertexShader*> m_vertexShaderPool;
    std::vector<CPixelShader*> m_pixelShaderPool;
};
#endif