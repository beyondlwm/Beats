#ifndef BEATS_SHADER_BASE_H__INCLUDE
#define BEATS_SHADER_BASE_H__INCLUDE

class CShaderBase
{
public:
    CShaderBase();
    virtual ~CShaderBase();

    size_t GetIndex();
    void SetIndex(size_t uIndex);

    const TString& GetShaderFileName();
    void SetShaderFileName(const TString& szFileName);

    const TString& GetMainFuncName();
    void SetMainFuncName(const TString& szMainFuncName);

private:
    size_t m_uIndex;
    TString m_szShaderFileName;
    TString m_szMainFuncName;
};


#endif