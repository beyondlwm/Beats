#include "stdafx.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Resource/ResourcePathManager.h"
#include "Render/Model.h"
#include "Render/Skeleton.h"
#include "Render/RenderState.h"
#include "Render/Material.h"

#include "Render/RenderStateParam/RenderStateParamBase.h"
#include "Render/RenderStateParam/BoolRenderStateParam.h"
#include "Render/RenderStateParam/UintRenderStateParam.h"
#include "Render/RenderStateParam/IntRenderStateParam.h"
#include "Render/RenderStateParam/FloatRenderStateParam.h"
#include "Render/RenderStateParam/FunctionRenderStateParam.h"
#include "Render/RenderStateParam/BlendRenderStateParam.h"
#include "Render/RenderStateParam/BlendEquationRenderStateParam.h"
#include "Render/RenderStateParam/CullModeRenderStateParam.h"
#include "Render/RenderStateParam/ClockWiseRenderStateParam.h"
#include "Render/RenderStateParam/PolygonModeRenderStateParam.h"
#include "Render/RenderStateParam/ShadeModeRenderStateParam.h"
#include "Render/RenderStateParam/StencilRenderStateParam.h"

#include "GUI/Window/Window.h"

#include "ParticlesSystem/ParticleSystem.h"
#include "ParticlesSystem/Emitter.h"
#include "ParticlesSystem/ParticleProperty.h"
#include "ParticlesSystem/BoxParticleEmitterEntity.h"
#include "ParticlesSystem/BallParticleEmitterEntity.h"
#include "ParticlesSystem/PointParticleEmitterEntity.h"
#include "ParticlesSystem/ConeParticleEmitterEntity.h"
#include "ParticlesSystem/ColorParticleAnimation.h"
#include "ParticlesSystem/RotateParticleAnimation.h"
#include "ParticlesSystem/ScaleParticleAnimation.h"
#include "ParticlesSystem/ParticleEmitterEntity.h"

void ForceReferenceSymbolForComponent()
{
    // When you are writing in a lib project, this cpp may not be referenced by the exe project.
    // so we add this empty function and make it extern in componentpublic.h, every cpp in the exe project,
    // which contain the componentpublic.h file will reference this symbol.
}

START_REGISTER_COMPONENT
REGISTER_COMPONENT(CModel, _T("Ä£ÐÍ"), _T("Render\\CModel"))
REGISTER_COMPONENT(CSkeleton, _T("¹Ç÷À"), _T("Render\\CSkeleton"))
REGISTER_COMPONENT(CMaterial, _T("²ÄÖÊ"), _T("Render\\CMaterial"))
REGISTER_COMPONENT(CRenderState, _T("äÖÈ¾×´Ì¬"), _T("Render\\CRenderState"))

REGISTER_COMPONENT(CBoolRenderStateParam, _T("CBoolRenderStateParam"), _T("Render\\CBoolRenderStateParam"))
REGISTER_COMPONENT(CUintRenderStateParam, _T("CUintRenderStateParam"), _T("Render\\CUintRenderStateParam"))
REGISTER_COMPONENT(CIntRenderStateParam, _T("CIntRenderStateParam"), _T("Render\\CIntRenderStateParam"))
REGISTER_COMPONENT(CFloatRenderStateParam, _T("CFloatRenderStateParam"), _T("Render\\CFloatRenderStateParam"))
REGISTER_COMPONENT(CFunctionRenderStateParam, _T("CFunctionRenderStateParam"), _T("Render\\CFunctionRenderStateParam"))
REGISTER_COMPONENT(CBlendRenderStateParam, _T("CBlendRenderStateParam"), _T("Render\\CBlendRenderStateParam"))
REGISTER_COMPONENT(CBlendEquationRenderStateParam, _T("CBlendEquationParam"), _T("Render\\CBlendEquationParam"))
REGISTER_COMPONENT(CCullModeRenderStateParam, _T("CCullModeParam"), _T("Render\\CCullModeParam"))
REGISTER_COMPONENT(CClockWiseRenderStateParam, _T("CClockWiseParam"), _T("Render\\CClockWiseParam"))
REGISTER_COMPONENT(CPolygonModeRenderStateParam, _T("CPolygonModeParam"), _T("Render\\CPolygonModeParam"))
REGISTER_COMPONENT(CShadeModeRenderStateParam, _T("CShadeModeParam"), _T("Render\\CShadeModeParam"))
REGISTER_COMPONENT(CStencilRenderStateParam, _T("CStencilParam"), _T("Render\\CStencilParam"))

REGISTER_COMPONENT(FCGUI::Window, _T("Window"), _T("GUI\\Window"))

REGISTER_COMPONENT(FCEngine::ParticleSystem, _T("ParticleSystem"), _T("ParticleSystem\\ParticleSystem"))
REGISTER_COMPONENT(FCEngine::ParticleEmitter, _T("ParticleEmitter"), _T("ParticleSystem") )
REGISTER_COMPONENT(FCEngine::PointParticleEmitterEntity, _T("PointEmitter"), _T("ParticleSystem") )
REGISTER_COMPONENT(FCEngine::BoxParticleEmitterEntity, _T("BoxEmitter"), _T("ParticleSystem") )
REGISTER_COMPONENT(FCEngine::BallParticleEmitterEntity, _T("BallEmitter"), _T("ParticleSystem") )
REGISTER_COMPONENT(FCEngine::ConeParticleEmitterEntity, _T("ConeEmitter"), _T("ParticleSystem") )
REGISTER_COMPONENT(FCEngine::ColorParticleAnimation, _T("ColorAnimation"), _T("ParticleSystem") )
REGISTER_COMPONENT(FCEngine::ScaleParticleAnimation, _T("ScaleAnimation"), _T("ParticleSystem") )
REGISTER_COMPONENT(FCEngine::RotateParticleAnimation, _T("RotateAnimation"), _T("ParticleSystem") )
REGISTER_COMPONENT(FCEngine::ParticleProperty, _T("ParticleProperty"), _T("ParticleSystem") )
REGISTER_ABSTRACT_COMPONENT(FCEngine::ParticleEmitterEntityBase)

END_REGISTER_COMPONENT