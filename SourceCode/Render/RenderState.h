#ifndef RENDER_RENDERSTATE_H__INCLUDE
#define RENDER_RENDERSTATE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

class CStencilRenderStateParam;
class CShadeModeRenderStateParam;
class CCullModeRenderStateParam;
class CClockWiseRenderStateParam;
class CBlendRenderStateParam;
class CBlendEquationRenderStateParam;
class CUintRenderStateParam;
class CPolygonModeRenderStateParam;
class CIntRenderStateParam;
class CFunctionRenderStateParam;
class CFloatRenderStateParam;
class CBoolRenderStateParam;

class CRenderState : public CComponentBase
{
    typedef CComponentBase super;
    DECLARE_REFLECT_GUID(CRenderState, 0x71B04E1B, CComponentBase)

public:
    CRenderState();
    CRenderState(CSerializer& serializer);
    virtual ~CRenderState();

    void SetBoolState(GLenum state, bool bEnable);
    bool GetBoolState(GLenum state) const;

    void SetDepthMask(bool bWriteable);
    bool GetDepthMask() const;

    void SetEdgeFlag(bool bEdgeFlag);
    bool GetEdgetFlag() const;

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

    void SetFrontFace(GLenum frontFace);
    GLenum GetFrontFace() const;

    void SetCullFace(GLenum cullFace);
    GLenum GetCullFace() const;

    void SetDepthNear(float fDepthNear);
    void SetDepthFar(float fDepthFar);
    float GetDepthNear() const;
    float GetDepthFar() const;
    void SetDepthFunc(GLenum func);
    GLenum GetDepthFunc() const;

    void SetStencilFunc(GLenum func);
    GLenum GetStencilFunc() const;
    void SetStencilReference(GLint nRef);
    GLint GetStencilReference() const;
    void SetStencilValueMask(GLint nValueMask);
    GLint GetStencilValueMask() const;
    void SetClearStencil(GLint nClearValue);
    GLint GetClearStencil() const;

    void SetShadeModel(GLenum shadeModel);
    GLenum GetShadeModel() const;

    void Restore();

private:
    size_t m_uCurrShaderProgram;
    size_t m_uCurrActiveTexture;
    CStencilRenderStateParam* m_pStencilRenderState;
    CShadeModeRenderStateParam* m_pShadeModeRenderState;
    CCullModeRenderStateParam* m_pCullModeRenderState;
    CClockWiseRenderStateParam* m_pClockWiseRenderState;
    CBlendRenderStateParam* m_pBlendRenderState;
    CBlendEquationRenderStateParam* m_pBlendEquationRenderState;
    std::vector<CUintRenderStateParam*> m_uintRenderState;
    std::vector<CPolygonModeRenderStateParam*> m_polygonModeRenderState;
    std::vector<CIntRenderStateParam*> m_intRenderState;
    std::vector<CFunctionRenderStateParam*> m_funcRenderState;
    std::vector<CFloatRenderStateParam*> m_floatRenderState;
    std::vector<CBoolRenderStateParam*> m_boolRenderState;
    std::map<GLenum, CBoolRenderStateParam*> m_boolRenderStateMap;
};


#endif