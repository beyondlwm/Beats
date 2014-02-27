#include "stdafx.h"
#include "CEGUISystemImpl.h"
#include "RendererModules\Direct3D9\CEGUIDirect3D9Renderer.h"
#include "CEGUIDefaultResourceProvider.h"
#include "CEGUISystem.h"
#include "CEGUIImageset.h"
#include "CEGUIFont.h"
#include "CEGUIScheme.h"
#include "CEGUIWindowManager.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "CEGUIScriptModule.h"
#include "CEGUIXMLParser.h"
#include "CEGUIAnimationManager.h"
#include "CEGUIGeometryBuffer.h"
#include "CEGUIRenderingRoot.h"
#include "CEGUI.h"
#include "CEGUIEvent.h"
#include "CEGUISubscriberSlot.h"
#include "../../Render/RenderManager.h"
#include "../../Control/Mouse.h"
#include "../../Control/Keyboard.h"
#include "../../ui/UIManager.h"


CCEGUISystemImpl::CCEGUISystemImpl()
: m_pRenderer(NULL)
{

}

CCEGUISystemImpl::~CCEGUISystemImpl()
{
    CEGUI::Direct3D9Renderer::destroySystem();
}

void CCEGUISystemImpl::Init()
{
    m_pRenderer = &CEGUI::Direct3D9Renderer::bootstrapSystem(CRenderManager::GetInstance()->GetDevice());
    InitResource();
    CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
    // clearing this queue actually makes sure it's created(!)
    m_pRenderer->getDefaultRenderingRoot().clearGeometry(CEGUI::RQ_OVERLAY);
    CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
    CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::DefaultWindow* root = (CEGUI::DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");
    CEGUI::System::getSingleton().setGUISheet(root);
    CEGUI::FontManager::getSingleton().create("STZHONGS.font", "fonts");
}

void CCEGUISystemImpl::Update()
{
    UpdateMouseInput();
    UpdateKeyboardInput();
    CEGUI::System& guiSystem = CEGUI::System::getSingleton();
    guiSystem.renderGUI();
    RestoreRenderState();//this is for reverting change to the render state by CEGUI.
}

void CCEGUISystemImpl::InitResource()
{
    /// Resource Init
    CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

    char dataPathPrefix[64];
    strcpy_s(dataPathPrefix, "datafiles");
    char resourcePath[512];
    // for each resource type, set a resource group directory
    sprintf_s(resourcePath, "%s/%s", dataPathPrefix, "schemes/");
    rp->setResourceGroupDirectory("schemes", resourcePath);
    sprintf_s(resourcePath, "%s/%s", dataPathPrefix, "imagesets/");
    rp->setResourceGroupDirectory("imagesets", resourcePath);
    sprintf_s(resourcePath, "%s/%s", dataPathPrefix, "fonts/");
    rp->setResourceGroupDirectory("fonts", resourcePath);
    sprintf_s(resourcePath, "%s/%s", dataPathPrefix, "layouts/");
    rp->setResourceGroupDirectory("layouts", resourcePath);
    sprintf_s(resourcePath, "%s/%s", dataPathPrefix, "looknfeel/");
    rp->setResourceGroupDirectory("looknfeels", resourcePath);
    sprintf_s(resourcePath, "%s/%s", dataPathPrefix, "lua_scripts/");
    rp->setResourceGroupDirectory("lua_scripts", resourcePath);
    sprintf_s(resourcePath, "%s/%s", dataPathPrefix, "xml_schemas/");
    rp->setResourceGroupDirectory("schemas", resourcePath);   
    sprintf_s(resourcePath, "%s/%s", dataPathPrefix, "animations/");
    rp->setResourceGroupDirectory("animations", resourcePath);

    InitResourceGroup();
}

void CCEGUISystemImpl::InitResourceGroup()
{
    // set the default resource groups to be used
    CEGUI::Imageset::setDefaultResourceGroup("imagesets");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
    CEGUI::AnimationManager::setDefaultResourceGroup("animations");

    // setup default group for validation schemas
    CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
    if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
        parser->setProperty("SchemaDefaultResourceGroup", "schemas");
}

void CCEGUISystemImpl::UpdateMouseInput()
{
    CMouse* pMouse = CMouse::GetInstance();
    if (pMouse->IsMoving())
    {
        CEGUI::System::getSingleton().injectMousePosition((float)(pMouse->GetRelativePosX()), (float)(pMouse->GetRelativePosY()));
        if (pMouse->IsJustEnter())
        {
            CUIManager::GetInstance()->ShowWinCursor(false);
        }
        else if (pMouse->IsJustLeave())
        {
            CUIManager::GetInstance()->ShowWinCursor(true);
        }
    }
    for (int i = 0; i < eMB_Count; ++i)
    {
        switch (pMouse->GetMouseBtnState((EMouseButton)i))
        {
        case eMBS_JustRelease:
            CEGUI::System::getSingleton().injectMouseButtonUp((CEGUI::MouseButton)i);
            break;
        case eMBS_JustPress:
            CEGUI::System::getSingleton().injectMouseButtonDown((CEGUI::MouseButton)i);
            break;
        default:
            break;
        }
    }
}

void CCEGUISystemImpl::UpdateKeyboardInput()
{
    CKeyboard* pKeyboard = CKeyboard::GetInstance();
    for (int i = 0; i < 256; ++i)
    {
        EKeyState keyState = pKeyboard->GetKeyState((EKeyCode)i);
        if (keyState == eKS_JustPress)
        {
            CEGUI::System::getSingleton().injectKeyDown((EKeyCode)i);
        }
        else if (keyState == eKS_JustRelease)
        {
            CEGUI::System::getSingleton().injectKeyUp((EKeyCode)i);
        }
    }
}

void CCEGUISystemImpl::RestoreRenderState()
{
    LPDIRECT3DDEVICE9 pDevice = m_pRenderer->getDevice();
    pDevice->SetFVF(NULL);
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
}