#ifndef RENDER_RENDERSTATE_H__INCLUDE
#define RENDER_RENDERSTATE_H__INCLUDE

#include "RenderStateParam/RenderStateParamBase.h"
#include "RenderStateParam/BlendEquationRenderStateParam.h"
#include "RenderStateParam/BlendRenderStateParam.h"
#include "RenderStateParam/BoolRenderStateParam.h"
#include "RenderStateParam/ClockWiseRenderStateParam.h"
#include "RenderStateParam/CullModeRenderStateParam.h"
#include "RenderStateParam/FloatRenderStateParam.h"
#include "RenderStateParam/FunctionRenderStateParam.h"
#include "RenderStateParam/IntRenderStateParam.h"
#include "RenderStateParam/PolygonModeRenderStateParam.h"
#include "RenderStateParam/UintRenderStateParam.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

class CRenderState : public CComponentBase
{
    typedef CComponentBase super;
    typedef std::vector< CRenderStateParamBase* > TParamStateVector;
    typedef TParamStateVector::iterator TParamStateVectorIter;
    typedef TParamStateVector::const_iterator TParamStateVectorIterConst;
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
    void SetStencilOp( GLenum fail, GLenum zFail, GLenum zPass );
    void GetStencilOp( GLenum& fail, GLenum& zFail, GLenum& zPass ) const;

    void SetShadeModel(GLenum shadeModel);
    GLenum GetShadeModel() const;

    void SetPolygonMode( GLenum face, GLenum mode );
    void GetPolygonMode( GLenum& face, GLenum& mode ) const;

    void SetColor( CUintRenderStateParam::EUintStateParam type, GLclampf r, GLclampf g, GLclampf b, GLclampf a );
    void GetColor( CUintRenderStateParam::EUintStateParam type , GLclampf& r, GLclampf& g, GLclampf& b, GLclampf& a );

    void Restore();

    bool operator==( const CRenderState& other ) const;

private:

    CRenderStateParamBase* GetRenderStateParamBasePtr( ERenderState state ) const;

    CRenderStateParamBase* GetFloatRenderStateParamPtr( CFloatRenderStateParam::EFloatStateParam type ) const;

    CRenderStateParamBase* GetFuncRenderStateParamPtr( CFunctionRenderStateParam::EFunctionStateParam type ) const;

    CRenderStateParamBase* GetIntRenderStateParamPtr( CIntRenderStateParam::EIntStateParam type ) const;

    CRenderStateParamBase* GetBoolRenderStateParamPtr( CBoolRenderStateParam::EBoolStateParam type ) const;

    CRenderStateParamBase* GetUnitRenderStateParamPtr( CUintRenderStateParam::EUintStateParam type ) const;

private:
    bool m_bDepthMark;
    bool m_bEdgeFlag;
    size_t m_uCurrShaderProgram;
    size_t m_uCurrActiveTexture;
    TParamStateVector m_pRenderStateParams;
    TParamStateVector m_boolRenderStateVector;
    TParamStateVector m_funcRenderStateVector;
    TParamStateVector m_floatRenderStateVector;
    TParamStateVector m_intRenderStateVector;
    TParamStateVector m_unitRenderStateVector;
};

#endif