#ifndef RENDER_RENDERSTATE_H__INCLUDE
#define RENDER_RENDERSTATE_H__INCLUDE

class CRenderState
{
public:
    CRenderState();
    ~CRenderState();

    void SetBoolState(GLenum state, bool bEnable);
    bool GetBoolState(GLenum state) const;

    void SetDepthMask(bool bWriteable);
    bool GetDepthMask() const;

    void SetActiveTexture(GLenum activeTexture);
    GLenum GetActiveTexture() const;

    void SetBlendFuncSrcFactor(GLenum src);
    void SetBlendFuncTargetFactor(GLenum target);
    GLenum GetBlendSrcFactor() const;
    GLenum GetBlendTargetFactor() const;
    void SetBlendEquation(GLenum func);
    GLenum GetBlendEquation();

    void SetCurrentShaderProgram(GLuint program);
    GLuint GetCurrentShaderProgram()const;

    void Restore();

private:
    bool m_bDepthMask;
    GLuint m_uCurrShaderProgram;
    GLenum m_uCurrActiveTexture;
    GLenum m_uBlendSourceFactor;
    GLenum m_uBlendTargetFactor;
    GLenum m_uBlendEquation;
    std::map<GLenum, bool> m_boolStateMap;
};


#endif