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

void ForceReferenceSymbolForComponent()
{
    // When you are writing in a lib project, this cpp may not be referenced by the exe project.
    // so we add this empty function and make it extern in componentpublic.h, every cpp in the exe project,
    // which contain the componentpublic.h file will reference this symbol.
}

START_REGISTER_COMPONENT
REGISTER_COMOPNENT(CModel, _T("Ä£ÐÍ"), _T("Render\\CModel"))
REGISTER_COMOPNENT(CSkeleton, _T("¹Ç÷À"), _T("Render\\CSkeleton"))
REGISTER_COMOPNENT(CMaterial, _T("²ÄÖÊ"), _T("Render\\CMaterial"))
REGISTER_COMOPNENT(CRenderState, _T("äÖÈ¾×´Ì¬"), _T("Render\\CRenderState"))

REGISTER_COMOPNENT(CBoolRenderStateParam, _T("CBoolRenderStateParam"), _T("Render\\CBoolRenderStateParam"))
REGISTER_COMOPNENT(CUintRenderStateParam, _T("CUintRenderStateParam"), _T("Render\\CUintRenderStateParam"))
REGISTER_COMOPNENT(CIntRenderStateParam, _T("CIntRenderStateParam"), _T("Render\\CIntRenderStateParam"))
REGISTER_COMOPNENT(CFloatRenderStateParam, _T("CFloatRenderStateParam"), _T("Render\\CFloatRenderStateParam"))
REGISTER_COMOPNENT(CFunctionRenderStateParam, _T("CFunctionRenderStateParam"), _T("Render\\CFunctionRenderStateParam"))
REGISTER_COMOPNENT(CBlendRenderStateParam, _T("CBlendRenderStateParam"), _T("Render\\CBlendRenderStateParam"))
REGISTER_COMOPNENT(CBlendEquationRenderStateParam, _T("CBlendEquationParam"), _T("Render\\CBlendEquationParam"))
REGISTER_COMOPNENT(CCullModeRenderStateParam, _T("CCullModeParam"), _T("Render\\CCullModeParam"))
REGISTER_COMOPNENT(CClockWiseRenderStateParam, _T("CClockWiseParam"), _T("Render\\CClockWiseParam"))
REGISTER_COMOPNENT(CPolygonModeRenderStateParam, _T("CPolygonModeParam"), _T("Render\\CPolygonModeParam"))
REGISTER_COMOPNENT(CShadeModeRenderStateParam, _T("CShadeModeParam"), _T("Render\\CShadeModeParam"))
REGISTER_COMOPNENT(CStencilRenderStateParam, _T("CStencilParam"), _T("Render\\CStencilParam"))

REGISTER_COMOPNENT(FCGUI::Window, _T("Window"), _T("GUI\\Window"))

REGISTER_COMOPNENT(FCEngine::ParticleSystem, _T("ParticleSystem"), _T("ParticleSystem\\ParticleSystem"))
REGISTER_COMOPNENT(FCEngine::ParticleEmitter, _T("ParticleEmitter"), _T("ParticleSystem\\") )
REGISTER_COMOPNENT(FCEngine::PointParticleEmitterEntity, _T("PointEmitter"), _T("ParticleSystem\\") )
REGISTER_COMOPNENT(FCEngine::BoxParticleEmitterEntity, _T("BoxEmitter"), _T("ParticleSystem\\") )
REGISTER_COMOPNENT(FCEngine::BallParticleEmitterEntity, _T("BallEmitter"), _T("ParticleSystem\\") )
REGISTER_COMOPNENT(FCEngine::ConeParticleEmitterEntity, _T("ConeEmitter"), _T("ParticleSystem\\") )
REGISTER_COMOPNENT(FCEngine::ColorParticleAnimation, _T("ColorAnimation"), _T("ParticleSystem\\") )
REGISTER_COMOPNENT(FCEngine::ScaleParticleAnimation, _T("ScaleAnimation"), _T("ParticleSystem\\") )
REGISTER_COMOPNENT(FCEngine::RotateParticleAnimation, _T("RotateAnimation"), _T("ParticleSystem\\") )
REGISTER_COMOPNENT(FCEngine::ParticleProperty, _T("ParticleProperty"), _T("ParticleSystem\\") )

END_REGISTER_COMPONENT

struct SLuancher
{
    SLuancher()
    {
        CResourcePathManager::GetInstance()->Init();
        const TString& strSourceCodePath = CResourcePathManager::GetInstance()->GetResourcePath(eRPT_SourceCode);
        std::vector<TString> scanPath;
        scanPath.push_back(strSourceCodePath);
        CEnumStrGenerator::GetInstance()->Init(scanPath);
    }
    ~SLuancher()
    {
        CEnumStrGenerator * pInstance= CEnumStrGenerator::GetInstance();
        if (pInstance !=NULL)
        {
            CEnumStrGenerator::Destroy();
        }

        CComponentManager::Destroy();
        CResourcePathManager::Destroy();
    }
}launchme;
