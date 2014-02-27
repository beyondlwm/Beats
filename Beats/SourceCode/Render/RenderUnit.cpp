#include "stdafx.h"
#include "RenderUnit.h"
#include "../Game/Object/BaseObject.h"
#include "../../../Utility/Math/Vector3.h"
#include "../Physics/PhysicsElement.h"
#include "RenderManager.h"
#include "../Game/Object/GameObject/Beat.h"
#include "../Function/Bind.h"

CRenderUnit::CRenderUnit(CBaseObject* pOwner)
: m_active(true)
, m_pOwner(pOwner)
, m_color(0)
, m_pRenderDevice(CRenderManager::GetInstance()->GetDevice())
, m_pTexture(NULL)
{
    HRESULT hr = D3DXCreateTextureFromFile(m_pRenderDevice, _T("Texture\\Tex.JPG"), &m_pTexture);
    BEATS_ASSERT(hr == S_OK, _T("Create Text Failed!"));
}

CRenderUnit::~CRenderUnit()
{

}

void CRenderUnit::Render(float /*timeDeltaMs*/)
{
    if (m_active)
    {
        CVector3 ownerPos = m_pOwner->GetPhysics()->GetWorldPos();
        Function<void(void)> function  = Bind(*this, &CRenderUnit::PreRender);
        CRenderManager::GetInstance()->SendQuadToCache(ownerPos, 18, 18, static_cast<CMusicBeat*>(m_pOwner)->GetGraphics()->GetColor(), &function);
    }
}

bool CRenderUnit::IsRecycle()
{
    return m_recycleFlag;
}

bool CRenderUnit::IsActive()
{
    return m_active;
}

void CRenderUnit::Reset()
{
    m_active = false;
    m_recycleFlag = false;
}

void CRenderUnit::SetActive( bool active )
{
    m_active = active;
}

void CRenderUnit::PreRender()
{
    m_pRenderDevice->SetRenderState(D3DRS_LIGHTING, false);
    m_pRenderDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_pRenderDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pRenderDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

    m_pRenderDevice->SetTexture(0, m_pTexture);

    m_pRenderDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pRenderDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pRenderDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    m_pRenderDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_pRenderDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

}

void CRenderUnit::SetColor( unsigned long color )
{
    m_color = color;
}

unsigned long CRenderUnit::GetColor()
{
    return m_color;
}