#include "stdafx.h"
#include "ComponentRenderWindow.h"
#include "../../Components/Component/ComponentEditorProxy.h"
#include "../../Components/Component/ComponentManager.h"
#include "../../Components/Component/ComponentGraphic.h"
#include "../../Components/DependencyDescription.h"
#include "../../Components/DependencyDescriptionLine.h"
#include "../../Components/ComponentPublic.h"
#include "../../components/Component/ComponentProject.h"
#include "ComponentGraphics_DX.h"
#include "../BDTWxFrame.h"
#include "../BDTWxApp.h"
#include <wx/numdlg.h>

static const float MAX_Z_VALUE = 1000.f;
static const float MIN_CELL_SIZE = 5.f;

enum EComponentMenuType
{
    eCMT_Copy,
    eCMT_Paste,
    eCMT_Delete,
    eCMT_Search,

    eCMT_Count,
    eCMT_Force32Bit = 0xFFFFFFFF
};

CComponentRenderWindow::CComponentRenderWindow( wxWindow *parent, 
                                               wxWindowID id /*= wxID_ANY*/, 
                                               const wxPoint& pos /*= wxDefaultPosition*/, 
                                               const wxSize& size /*= wxDefaultSize*/, 
                                               long style /*= 0*/ )
                                               : wxWindow(parent, id, pos, size, style)
                                               , m_pD3dDevice(NULL)
                                               , m_pDXFont(NULL)
                                               , m_pDraggingComponent(NULL)
                                               , m_pConnectComponent(NULL)
                                               , m_pCopyComponent(NULL)
                                               , m_pDraggingDependency(NULL)
                                               , m_pSelectedDependencyLine(NULL)
                                               , m_cellSize(15.0f)
                                               , m_bInit(false)
                                               , m_backGroundColor(0xff044080)
                                               , m_gridLineColor(0x80ffffff)
                                               , m_pMainFrame(NULL)
{
    ZeroMemory(&m_d3dPresentParam, sizeof(m_d3dPresentParam));
    m_bInit = InitDirectXRender(GetHWND(), GetSize().x, GetSize().y, true);
    m_pMainFrame = CBDTWxApp::GetBDTWxApp()->GetMainFrame();
    m_pComponentMenu = new wxMenu;
    m_pComponentMenu->Append(eCMT_Copy, _T("拷贝"));
    m_pComponentMenu->Append(eCMT_Paste, _T("粘贴"));
    m_pComponentMenu->Append(eCMT_Delete, _T("删除"));
    m_pComponentMenu->Append(eCMT_Search, _T("搜索"));

    Connect( wxEVT_IDLE, wxIdleEventHandler( CComponentRenderWindow::OnIdle ), NULL, this );
    Connect( wxEVT_PAINT, wxPaintEventHandler( CComponentRenderWindow::OnPaint ), NULL, this );
    Connect( wxEVT_SIZE, wxSizeEventHandler( CComponentRenderWindow::OnSize ), NULL, this );
    Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler(CComponentRenderWindow::OnMouseMidScroll), NULL, this);
    Connect( wxEVT_MOTION, wxMouseEventHandler(CComponentRenderWindow::OnMouseMove), NULL, this);
    Connect( wxEVT_RIGHT_UP, wxMouseEventHandler(CComponentRenderWindow::OnMouseRightUp), NULL, this);
    Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler(CComponentRenderWindow::OnMouseRightDown), NULL, this);

    Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler(CComponentRenderWindow::OnMouseLeftDown), NULL, this);
    Connect( wxEVT_LEFT_UP, wxMouseEventHandler(CComponentRenderWindow::OnMouseLeftUp), NULL, this);
    Connect( wxEVT_CHAR, wxKeyEventHandler(CComponentRenderWindow::OnKeyPressed), NULL, this);

    m_pComponentMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CComponentRenderWindow::OnComponentMenuClicked), NULL, this);

}

CComponentRenderWindow::~CComponentRenderWindow()
{
    Disconnect( wxEVT_IDLE, wxIdleEventHandler( CComponentRenderWindow::OnIdle ), NULL, this );
    Disconnect( wxEVT_PAINT, wxPaintEventHandler( CComponentRenderWindow::OnPaint ), NULL, this );
    Disconnect( wxEVT_SIZE, wxSizeEventHandler( CComponentRenderWindow::OnSize ), NULL, this );
    Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler(CComponentRenderWindow::OnMouseMidScroll), NULL, this);
    Disconnect( wxEVT_MOTION, wxMouseEventHandler(CComponentRenderWindow::OnMouseMove), NULL, this);
    Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler(CComponentRenderWindow::OnMouseRightUp), NULL, this);
    Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler(CComponentRenderWindow::OnMouseRightDown), NULL, this);
    Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler(CComponentRenderWindow::OnMouseLeftDown), NULL, this);
    Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler(CComponentRenderWindow::OnMouseLeftUp), NULL, this);
    Disconnect( wxEVT_CHAR, wxKeyEventHandler(CComponentRenderWindow::OnKeyPressed), NULL, this);
    m_pComponentMenu->Disconnect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CComponentRenderWindow::OnComponentMenuClicked), NULL, this);
    BEATS_SAFE_DELETE(m_pComponentMenu);
    BEATS_SAFE_DELETE(m_pCopyComponent);
    ReleaseDirectX();
}

void CComponentRenderWindow::OnPaint( wxPaintEvent& event )
{
    event.Skip(true);
}

void CComponentRenderWindow::Render()
{
    HRESULT hr = m_pD3dDevice->TestCooperativeLevel();
    switch (hr)
    {
    case D3D_OK:
        {
            hr = m_pD3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_backGroundColor, 1.0f, 0);
            BEATS_ASSERT(hr == D3D_OK);
            D3DXMATRIX viewMatrix;
            D3DXMatrixTranslation(&viewMatrix, m_currentClientPos.x, m_currentClientPos.y, 0);
            m_pD3dDevice->SetTransform(D3DTS_VIEW, &viewMatrix);
            m_pD3dDevice->SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);

            hr = m_pD3dDevice->BeginScene();

            m_pD3dDevice->SetFVF(SVertex2::VertexFormat);
            m_pD3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
            m_pD3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
            m_pD3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW);
            m_pD3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            m_pD3dDevice->SetTexture(0, NULL);
            m_pD3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
            m_pD3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
            m_pD3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
            m_pD3dDevice->SetRenderState(D3DRS_ALPHAREF, 0);
            m_pD3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

            RenderGridLine();
            RenderDraggingDependencyLine();

            m_pD3dDevice->SetFVF(SVertex::VertexFormat);
            RenderComponents();

            hr = m_pD3dDevice->EndScene();
            hr = m_pD3dDevice->Present( NULL, NULL, NULL, NULL );
        }
        break;
    case D3DERR_DEVICELOST:
        {
            //Do nothing but pause all rendering logic.
        }
        break;
    case D3DERR_DEVICENOTRESET:
        {
            bool bRet = ResetDevice();
            BEATS_ASSERT(bRet, _T("Reset d3d device failed! check if some resource are not released!"));
        }
        break;
    }
}

bool CComponentRenderWindow::InitDirectXRender(const HWND& hwnd, int width, int height, bool bWindow)
{
    m_currentClientPos.x = 0;
    m_currentClientPos.y = 0;
    IDirect3D9* pD3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    BEATS_ASSERT(pD3d9 != NULL, _T("Create Render D3D9 device failed!"));
    bool bRet = false;
    if (pD3d9 != NULL)
    {
        D3DCAPS9 caps;
        pD3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

        m_d3dPresentParam.BackBufferFormat           = D3DFMT_X8R8G8B8;
        m_d3dPresentParam.MultiSampleType            = D3DMULTISAMPLE_NONE;
        m_d3dPresentParam.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
        m_d3dPresentParam.hDeviceWindow              = hwnd;
        m_d3dPresentParam.Windowed                   = bWindow;
        m_d3dPresentParam.EnableAutoDepthStencil     = true; 
        m_d3dPresentParam.AutoDepthStencilFormat     = D3DFMT_D24S8;
        m_d3dPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
        m_d3dPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        
        if (bWindow)
        {
            m_d3dPresentParam.BackBufferWidth             = 0;
            m_d3dPresentParam.BackBufferHeight             = 0;
            m_d3dPresentParam.BackBufferCount             = 0;
        }
        else
        {
            m_d3dPresentParam.BackBufferWidth          = width;
            m_d3dPresentParam.BackBufferHeight         = height;
            m_d3dPresentParam.BackBufferCount          = 1;
        }

        bool supportHardwareVextexProcess = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
        int vp = supportHardwareVextexProcess ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        HRESULT hr = pD3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &m_d3dPresentParam, &m_pD3dDevice);
        BEATS_ASSERT(hr == S_OK, _T("Create Render Device Failed!"));
        if (hr == S_OK)
        {            
            CComponentGraphic_DX::RecreateTextures(m_pD3dDevice, CBDTWxApp::GetBDTWxApp()->GetWorkingPath().c_str());
            bRet = ResetDevice();
        }
        pD3d9->Release();
    }

    return bRet;
}

bool CComponentRenderWindow::ResetDevice(bool bRestore /*= true*/)
{
    // Release render targets, depth stencil surfaces, additional swap chains, state blocks, font
    // and D3DPOOL_DEFAULT resources associated with the device for once.
    if (m_pDXFont != NULL)
    {
        m_pDXFont->Release();
        m_pDXFont = NULL;
    }

    bool bRet = m_pD3dDevice->Reset(&m_d3dPresentParam) == D3D_OK;
    if (bRet && bRestore)
    {
        D3DXMatrixOrthoLH(&m_projectionMatrix, GetSize().x, GetSize().y, 0, 1000.f);

        //Recreate font.
        D3DXFONT_DESC descDefault;
        memset(&descDefault, 0, sizeof(D3DXFONT_DESC));
        descDefault.CharSet = DEFAULT_CHARSET;
        descDefault.Height = 14;
        descDefault.Width = 6;
        descDefault.Weight = FW_NORMAL;
        descDefault.Quality = PROOF_QUALITY;
        _tcscpy(descDefault.FaceName, _T("Arial"));
        bRet = D3DXCreateFontIndirect(m_pD3dDevice, &descDefault, &m_pDXFont) == D3D_OK;
    }
    return bRet;
}

bool CComponentRenderWindow::ReleaseDirectX()
{
    if (m_bInit)
    {
        CComponentGraphic_DX::ReleaseTextures();
        m_pD3dDevice->Release();
        m_pD3dDevice = NULL;
        m_pDXFont->Release();
        m_pDXFont = NULL;
        m_bInit = false;
    }
    return !m_bInit;
}

void CComponentRenderWindow::RenderGridLine()
{
/*   _________________________
    |                         |
    |    ________________     |
    |    |               |    |
    |    |       c       |    |
    |    |_______________|    |                
    |                         |
    s_________________________|
    
    s : position that start to draw line
    c : current position, also means center of the screen
    inner rect: our rendering area, also means screen
    outside rect: the area to draw line
    the size of outside rect is just twice as the screen size.
*/
    const wxSize currentSize = GetSize();
    size_t clientAreaGridCountX = currentSize.x / m_cellSize;
    size_t clientAreaGridCountY = currentSize.y / m_cellSize;

    D3DXVECTOR2 startPos;
    startPos.x = -m_currentClientPos.x - clientAreaGridCountX * m_cellSize;
    startPos.y = -m_currentClientPos.y - clientAreaGridCountY * m_cellSize;

    // Clip the pos to grid.
    startPos.x = (int)(startPos.x / m_cellSize) * m_cellSize;
    startPos.y = (int)(startPos.y / m_cellSize) * m_cellSize;
    std::vector<SVertex2> linePosBuff;

    for (size_t j = 0; j < clientAreaGridCountY * 2; ++j)
    {
        SVertex2 linePos;
        linePos.m_color = 0x60606060;
        linePos.m_position.x = startPos.x;
        linePos.m_position.y = startPos.y + j * m_cellSize;
        linePos.m_position.z = MAX_Z_VALUE;
        linePosBuff.push_back(linePos);
        linePos.m_position.x = startPos.x + clientAreaGridCountX * 2 * m_cellSize;
        linePos.m_position.y = startPos.y + j * m_cellSize;
        linePos.m_position.z = MAX_Z_VALUE;
        linePosBuff.push_back(linePos);
    }
    for (size_t i = 0; i < clientAreaGridCountX * 2; ++i)
    {
        SVertex2 linePos;
        linePos.m_color = 0x60606060;
        linePos.m_position.x = startPos.x + i * m_cellSize;
        linePos.m_position.y = startPos.y;
        linePos.m_position.z = MAX_Z_VALUE;
        linePosBuff.push_back(linePos);
        linePos.m_position.x = startPos.x + i * m_cellSize;
        linePos.m_position.y = startPos.y + clientAreaGridCountY * 2 * m_cellSize;
        linePos.m_position.z = MAX_Z_VALUE;
        linePosBuff.push_back(linePos);
    }
    m_pD3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, linePosBuff.size() * 0.5f, &*linePosBuff.begin(), sizeof(SVertex2));

    //Draw Center Pos
    SVertex2 tmp[4];
    tmp[0].m_position.x = 0;
    tmp[0].m_position.y = m_cellSize;
    tmp[0].m_position.z = MAX_Z_VALUE;
    tmp[0].m_color = 0xffffffff;
    tmp[1].m_position.x = 0;
    tmp[1].m_position.y = -m_cellSize;
    tmp[1].m_position.z = MAX_Z_VALUE;
    tmp[1].m_color = 0xffffffff;


    tmp[2].m_position.x = -m_cellSize;
    tmp[2].m_position.y = 0;
    tmp[2].m_position.z = MAX_Z_VALUE;
    tmp[2].m_color = 0xffffffff;

    tmp[3].m_position.x = m_cellSize;
    tmp[3].m_position.y = 0;
    tmp[3].m_position.z = MAX_Z_VALUE;
    tmp[3].m_color = 0xffffffff;

    m_pD3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 4, tmp, sizeof(SVertex2));
}

void CComponentRenderWindow::RenderComponents()
{
    //1. Render all component instance.
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = CComponentManager::GetInstance()->GetInstance()->GetComponentInstanceMap();
    std::map<size_t, std::map<size_t, CComponentBase*>*>::const_iterator iter = pInstanceMap->begin();
    for (; iter != pInstanceMap->end(); ++iter)
    {
        std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin();
        for (; subIter != iter->second->end(); ++subIter)
        {
            CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(subIter->second);
            CComponentGraphic* pGraphic = pComponent->GetGraphics();
            if (pGraphic)
            {
                pGraphic->OnRender(m_cellSize, pComponent == CBDTWxApp::GetBDTWxApp()->GetMainFrame()->GetSelectedComponent());
            }
        }
    }

    //2. Render dragging component
    if (m_pDraggingComponent)
    {
        // Convert screen mouse position to rendering area position
        wxPoint clientPos = ScreenToClient(wxGetMousePosition());
        float fMouseWorldPosX = 0;
        float fMouseWorldPosY = 0;
        ConvertWindowPosToWorldPos(clientPos, &fMouseWorldPosX, &fMouseWorldPosY);
        CComponentGraphic* pDraggingGraphics = m_pDraggingComponent->GetGraphics();
        int x = 0;
        int y = 0;
        D3DXVECTOR2 worldPos = D3DXVECTOR2(fMouseWorldPosX, fMouseWorldPosY);
        ConvertWorldPosToGridPos(&worldPos, &x, &y);
        pDraggingGraphics->SetPosition(x, y);
        pDraggingGraphics->OnRender(m_cellSize, true);

        //BEATS_PRINT(_T("You must be dragging at world Pos%f, %f gridPos %d %d\n"), fMouseWorldPosX, fMouseWorldPosY, x, y);
    }
}

void CComponentRenderWindow::RenderDraggingDependencyLine()
{
    CComponentEditorProxy* pSelectedComponent = CBDTWxApp::GetBDTWxApp()->GetMainFrame()->GetSelectedComponent();
    if (wxGetMouseState().LeftIsDown() && pSelectedComponent)
    {
        if (m_pDraggingDependency != NULL)
        {
            int iDependencyPosX = 0; 
            int iDependencyPosY = 0;
            size_t uIndex = m_pDraggingDependency->GetIndex();
            pSelectedComponent->GetGraphics()->GetDependencyPosition(uIndex, &iDependencyPosX, &iDependencyPosY);
            SVertex2 line[2];
            ConvertGridPosToWorldPos(iDependencyPosX, iDependencyPosY, &line[0].m_position.x, &line[0].m_position.y);
            line[0].m_position.x += m_cellSize * 0.5f * pSelectedComponent->GetGraphics()->GetDependencyWidth();
            line[0].m_position.y -= m_cellSize * 0.5f * pSelectedComponent->GetGraphics()->GetDependencyHeight();
            wxPoint pos = wxGetMouseState().GetPosition();
            // Convert screen mouse position to rendering area position
            wxPoint clientPos = ScreenToClient(wxGetMousePosition());
            ConvertWindowPosToWorldPos(clientPos, &line[1].m_position.x, &line[1].m_position.y);
            line[0].m_color = (m_pConnectComponent != NULL) ? 0xff00ff00 : 0xffff0000;
            line[1].m_color = (m_pConnectComponent != NULL) ? 0xff00ff00 : 0xffff0000;
            m_pD3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, line, sizeof(SVertex2));
        }
    }
}

void CComponentRenderWindow::OnSize( wxSizeEvent& event )
{
    int height = event.GetSize().GetHeight();
    int width = event.GetSize().GetWidth();
    if ( m_bInit && height > 0 && width > 0)
    {   
        m_d3dPresentParam.BackBufferHeight = height;
        m_d3dPresentParam.BackBufferWidth = width;

        ResetDevice();
        size_t minSize = min(height, width);
        if (m_cellSize > minSize)
        {
            m_cellSize = minSize;
        }
    }
    event.Skip(true);
}

void CComponentRenderWindow::OnMouseMidScroll( wxMouseEvent& event )
{
    if (m_bInit)
    {
        // Resize render area.
        int rotation = event.GetWheelRotation();
        int gridPosX = m_currentClientPos.x / m_cellSize;
        int gridPosY = m_currentClientPos.y / m_cellSize;

        float delta = m_cellSize* 0.1f * rotation / event.GetWheelDelta();
        m_cellSize += delta;
        if (m_cellSize <= MIN_CELL_SIZE)
        {
            m_cellSize = MIN_CELL_SIZE;
        }
        float minSize = min(GetSize().GetWidth(), GetSize().GetHeight());
        if (m_cellSize > minSize)
        {
            m_cellSize = minSize;
        }
        m_currentClientPos.x = gridPosX * m_cellSize;
        m_currentClientPos.y = gridPosY * m_cellSize;
        UpdateAllDependencyLine();
    }

    event.Skip(true);
}

void CComponentRenderWindow::OnIdle( wxIdleEvent& /*event*/ )
{
    if (m_bInit && m_pD3dDevice)
    {
        Render();
    }
}

void CComponentRenderWindow::OnMouseMove( wxMouseEvent& event )
{
    if (m_bInit && m_pD3dDevice)
    {
        wxPoint pos = event.GetPosition();
        // Drag the screen
        if(event.RightIsDown())
        {
            float deltaX = pos.x - m_startDragPos.x;
            float deltaY = pos.y - m_startDragPos.y;
            m_currentClientPos.x += deltaX;
            m_currentClientPos.y -= deltaY;
            m_startDragPos.x = pos.x;
            m_startDragPos.y = pos.y;
            //BEATS_PRINT(_T("Screen Moved, Current Client Pos %f, %f\n"), m_currentClientPos.x, m_currentClientPos.y);
        }
        if (event.LeftIsDown())
        {
            m_pConnectComponent = NULL;
            
            //Drag Component
            CComponentEditorProxy* pSelectedComponent = CBDTWxApp::GetBDTWxApp()->GetMainFrame()->GetSelectedComponent();
            if (pSelectedComponent != NULL)
            {
                if (m_pDraggingDependency == NULL)
                {
                    D3DXVECTOR2 worldPos;
                    ConvertWindowPosToWorldPos(pos, &worldPos.x, &worldPos.y);
                    int gridPosX = 0;
                    int gridPosY = 0;
                    ConvertWorldPosToGridPos(&worldPos, &gridPosX, &gridPosY);
                    pSelectedComponent->GetGraphics()->SetPosition(gridPosX, gridPosY);
                    for (size_t i = 0; i < pSelectedComponent->GetDependencies().size(); ++i)
                    {
                        CDependencyDescription* pDesc = pSelectedComponent->GetDependencies()[i];
                        size_t uLineCount = pDesc->GetDependencyLineCount();
                        for (size_t j = 0; j < uLineCount; ++j)
                        {
                            pDesc->GetDependencyLine(j)->UpdateRect(m_cellSize);
                        }
                    }
                    for (size_t i = 0; i < pSelectedComponent->GetBeConnectedDependencyLines().size(); ++i)
                    {
                        CDependencyDescriptionLine* pDescLine = pSelectedComponent->GetBeConnectedDependencyLines()[i];
                        pDescLine->UpdateRect(m_cellSize);
                    }
                    //BEATS_PRINT(_T("You must dragging component instance %s to worldPos %f %f, gridPos %d %d\n"), pSelectedComponent->GetClassName(), worldPos.x, worldPos.y, gridPosX, gridPosY);
                }
                else
                {
                    EComponentAeraRectType aeraType;
                    CComponentEditorProxy* pComponentMoveTo = HitTestForComponent(pos, &aeraType);
                    if (aeraType == eCART_Connection)
                    {
                        if (m_pDraggingDependency->IsMatch(pComponentMoveTo) && pComponentMoveTo != m_pDraggingDependency->GetOwner() && !m_pDraggingDependency->IsInDependency(pComponentMoveTo))
                        {
                            m_pConnectComponent = pComponentMoveTo;
                        }
                    }
                }
            }
        }
        else
        {
            m_pDraggingComponent = NULL;
        }
    }
    event.Skip(true);
}

void CComponentRenderWindow::OnMouseLeftDown( wxMouseEvent& event )
{
    if (m_bInit && m_pD3dDevice)
    {
        wxPoint pos = event.GetPosition();
        m_startDragPos.x = pos.x;
        m_startDragPos.y = pos.y;
        EComponentAeraRectType aeraType;
        void* pData = NULL;
        CComponentEditorProxy* pClickedComponent = HitTestForComponent(pos, &aeraType, &pData);

        CBDTWxFrame* pMainFrame = CBDTWxApp::GetBDTWxApp()->GetMainFrame();
        if (pMainFrame)
        {
            pMainFrame->SelecteComponent(pClickedComponent);
        }
        if (m_pSelectedDependencyLine != NULL)
        {
            m_pSelectedDependencyLine->SetSelected(false);
            m_pSelectedDependencyLine = NULL;
        }
        if (aeraType == eCART_Dependency)
        {
            BEATS_ASSERT(pClickedComponent != NULL);
            m_pDraggingDependency = (CDependencyDescription*)pData;
            if (!m_pDraggingDependency->IsListType())
            {
                m_pDraggingDependency->Hide();
            }
        }
        else if (aeraType == eCART_DependencyLine)
        {
            BEATS_ASSERT(pClickedComponent == NULL);
            m_pSelectedDependencyLine = (CDependencyDescriptionLine*)pData;
            BEATS_ASSERT(m_pSelectedDependencyLine != NULL);
            m_pSelectedDependencyLine->SetSelected(true);
        }
    }
    event.Skip(true);
}

void CComponentRenderWindow::OnMouseLeftUp(wxMouseEvent& /*event*/)
{
    CDependencyDescriptionLine* pLine = NULL;

    if (m_pConnectComponent != NULL)
    {
        BEATS_ASSERT(m_pDraggingDependency != NULL);
        if (m_pDraggingDependency->GetDependencyLineCount() > 0 && !m_pDraggingDependency->IsListType())
        {
            pLine = m_pDraggingDependency->SetDependency(0, m_pConnectComponent);
        }
        else
        {
            pLine = m_pDraggingDependency->AddDependency(m_pConnectComponent);
        }
    }
    if (pLine != NULL)
    {
        pLine->UpdateRect(m_cellSize);
    }
    if (m_pDraggingDependency != NULL && !m_pDraggingDependency->IsVisible())
    {
        m_pDraggingDependency->Show();
    }
    m_pConnectComponent = NULL;
    m_pDraggingDependency = NULL;
}

void CComponentRenderWindow::OnMouseRightUp( wxMouseEvent& event )
{
    if (CComponentManager::GetInstance()->GetProject()->GetRootDirectory() != NULL)
    {
        wxPoint pos = event.GetPosition();
        CComponentEditorProxy* pClickedComponent = HitTestForComponent(pos);
        CBDTWxFrame* pMainFrame = CBDTWxApp::GetBDTWxApp()->GetMainFrame();
        if (pMainFrame)
        {
            pMainFrame->SelecteComponent(pClickedComponent);
        }
        m_pComponentMenu->Enable(eCMT_Copy, pClickedComponent != NULL);
        m_pComponentMenu->Enable(eCMT_Delete, pClickedComponent != NULL);
        m_pComponentMenu->Enable(eCMT_Paste, pClickedComponent == NULL && m_pCopyComponent != NULL);
        // Force render before menu pop up.
        Render();
        PopupMenu(m_pComponentMenu, wxDefaultPosition);
    }

    event.Skip(true);
}

void CComponentRenderWindow::OnMouseRightDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    m_startDragPos.x = pos.x;
    m_startDragPos.y = pos.y;
    event.Skip(true);
}

IDirect3DDevice9* CComponentRenderWindow::GetRenderDevice() const
{
    return m_pD3dDevice;
}

LPD3DXFONT CComponentRenderWindow::GetRenderFont() const
{
    return m_pDXFont;
}

void CComponentRenderWindow::SetDraggingComponent( CComponentEditorProxy* pDraggingComponent )
{
    m_pDraggingComponent = pDraggingComponent;
}

CComponentEditorProxy* CComponentRenderWindow::GetDraggingComponent()
{
    return m_pDraggingComponent;
}

void CComponentRenderWindow::ConvertWorldPosToGridPos( const D3DXVECTOR2* pVectorPos, int* pOutGridX, int* pOutGridY )
{
    // Clip to left top corner.
    *pOutGridX = floor(pVectorPos->x / m_cellSize);
    *pOutGridY = ceil(pVectorPos->y / m_cellSize);
}

void CComponentRenderWindow::ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY)
{
    *pOutWorldPosX = gridX * m_cellSize;
    *pOutWorldPosY = gridY * m_cellSize;
}

void CComponentRenderWindow::ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY)
{
    if (m_bInit && m_pD3dDevice)
    {
        wxSize clientSize = GetClientSize();        
        *pOutWorldPosX = (windowPos.x - clientSize.x * 0.5f) - m_currentClientPos.x;
        *pOutWorldPosY = (clientSize.y * 0.5f - windowPos.y) - m_currentClientPos.y;
    }
}

CComponentEditorProxy* CComponentRenderWindow::HitTestForComponent( wxPoint pos, EComponentAeraRectType* pOutAreaType/* = NULL*/, void** pReturnData /* = NULL*/ )
{
    float fClickWorldPosX = 0;
    float fClickWorldPosY = 0;
    ConvertWindowPosToWorldPos(pos, &fClickWorldPosX, &fClickWorldPosY);
    //BEATS_PRINT(_T("Left Click at window pos :%d, %d worldPos: %f, %f\n"), pos.x, pos.y, fClickWorldPosX, fClickWorldPosY);
    bool bFoundResult = false;
    CComponentEditorProxy* pClickedComponent = NULL;
    // Find the component for select. reverse iterator the instance map, because the last one will always be rendered at the top, so we will select top first.
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = CComponentManager::GetInstance()->GetInstance()->GetComponentInstanceMap();
    std::map<size_t, std::map<size_t, CComponentBase*>*>::const_reverse_iterator iter = pInstanceMap->rbegin();
    for (; iter != pInstanceMap->rend() && !bFoundResult; ++iter)
    {
        std::map<size_t, CComponentBase*>::const_reverse_iterator subIter = iter->second->rbegin();
        for (; subIter != iter->second->rend() && !bFoundResult; ++subIter)
        {
            CComponentGraphic* pGraphics = static_cast<CComponentEditorProxy*>(subIter->second)->GetGraphics();
            int x = 0;
            int y = 0;
            pGraphics->GetPosition(&x, &y);    // Left top pos.
            float fWorldPosX = 0;
            float fWorldPosY = 0;
            ConvertGridPosToWorldPos(x, y, &fWorldPosX, &fWorldPosY);
            float width = pGraphics->GetWidth() * m_cellSize;
            float height = pGraphics->GetHeight() * m_cellSize;
            float fDelatX = fClickWorldPosX - fWorldPosX;
            if ( fDelatX > 0 && fDelatX < width)
            {
                float fDelatY = fWorldPosY - fClickWorldPosY;
                if (fDelatY > 0 && fDelatY < height)
                {
                    pClickedComponent = static_cast<CComponentEditorProxy*>(subIter->second);
                    if (pOutAreaType != NULL)
                    {
                        D3DXVECTOR2 worldPos;
                        worldPos.x = fClickWorldPosX;
                        worldPos.y = fClickWorldPosY;
                        int iClickGridPosX = 0;
                        int iClickGridPosY = 0;
                        ConvertWorldPosToGridPos(&worldPos, &iClickGridPosX, &iClickGridPosY);
                        bFoundResult = true;
                        *pOutAreaType = pGraphics->HitTestForAreaType(iClickGridPosX, iClickGridPosY, pReturnData);
                    }
                    break;
                }
            }
            //check dependency line.
            CComponentEditorProxy* pComponentProxy = static_cast<CComponentEditorProxy*>(subIter->second);
            for (size_t i = 0; i < pComponentProxy->GetDependencies().size() && !bFoundResult; ++i)
            {
                CDependencyDescription* pDependency = pComponentProxy->GetDependency(i);
                int iDependencyX, iDependencyY;
                pGraphics->GetDependencyPosition(i, &iDependencyX, &iDependencyY);
                for (size_t j = 0; j < pDependency->GetDependencyLineCount(); ++j)
                {
                    CDependencyDescriptionLine* pLine = pDependency->GetDependencyLine(j);
                    bool bInLine = pLine->HitTest(fClickWorldPosX, fClickWorldPosY, m_cellSize);
                    if (bInLine)
                    {
                        if (pOutAreaType != NULL && pReturnData != NULL)
                        {
                            *pOutAreaType = eCART_DependencyLine;
                            *pReturnData = pLine;
                            bFoundResult = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    return pClickedComponent;
}

void CComponentRenderWindow::OnComponentMenuClicked( wxMenuEvent& event )
{
    CComponentEditorProxy* pComponentProxy = m_pMainFrame->GetSelectedComponent();
    switch (event.GetId())
    {
    case eCMT_Delete:
        {
            BEATS_ASSERT(pComponentProxy != NULL, _T("Impossible to delete component from menu when there is no components selected."));
            DeleteSelectedComponent();
        }        
        break;
    case eCMT_Copy:
        {
            BEATS_ASSERT(pComponentProxy != NULL, _T("Impossible to copy component from menu when there is no components selected."));
            if (pComponentProxy != m_pCopyComponent)
            {
                BEATS_SAFE_DELETE(m_pCopyComponent);
                m_pCopyComponent = static_cast<CComponentEditorProxy*>(pComponentProxy->Clone(true, NULL));
            }
        }
        break;
    case eCMT_Paste:
        {
            BEATS_ASSERT(pComponentProxy == NULL && m_pCopyComponent != NULL, _T("Impossible to paste component from menu when there is a component selected or no copy component."));
            CComponentEditorProxy* pNewComponentEditorProxy = static_cast<CComponentEditorProxy*>(CComponentManager::GetInstance()->CreateComponentByRef(m_pCopyComponent, true));
            wxPoint mouseWindowPos = wxGetMousePosition();
            mouseWindowPos = ScreenToClient(mouseWindowPos);
            D3DXVECTOR2 mouseWorldPos;
            ConvertWindowPosToWorldPos(mouseWindowPos, &mouseWorldPos.x, &mouseWorldPos.y);
            int iGridPosX, iGridPosY;
            ConvertWorldPosToGridPos(&mouseWorldPos, &iGridPosX, &iGridPosY);
            pNewComponentEditorProxy->GetGraphics()->SetPosition(iGridPosX - 1, iGridPosY + 1);
        }
        break;
    case eCMT_Search:
        {
            int iComponentId = wxGetNumberFromUser(_T("搜索组件"), _T("组件ID"), _T("搜索组件"), 0, 0xFFFFFFFF, 0x7FFFFFFE);
            if (m_pMainFrame != NULL && iComponentId != 0xFFFFFFFF)
            {
                CComponentProject* pProject = CComponentManager::GetInstance()->GetProject();
                size_t uFileId = pProject->QueryFileId(iComponentId, false);
                if (uFileId != 0xFFFFFFFF)
                {
                    TString filePath = pProject->GetComponentFileName(uFileId);
                    if (filePath.compare(CComponentManager::GetInstance()->GetCurrentWorkingFilePath()) != 0)
                    {
                        TCHAR szInfo[1024];
                        _stprintf(szInfo, _T("搜索到ID为%d的组件位于文件%s,是否跳转到该组件?"), iComponentId, filePath.c_str());
                        int iAnswer = MessageBox(NULL, szInfo, _T("搜索结果"), MB_YESNO);
                        if (iAnswer == IDYES)
                        {
                            m_pMainFrame->ActivateFile(filePath.c_str());
                            CComponentBase* pComponent = CComponentManager::GetInstance()->GetComponentInstance(iComponentId);
                            m_pMainFrame->SelecteComponent(static_cast<CComponentEditorProxy*>(pComponent));
                        }
                    }
                    else
                    {
                        CComponentBase* pComponent = CComponentManager::GetInstance()->GetComponentInstance(iComponentId);
                        m_pMainFrame->SelecteComponent(static_cast<CComponentEditorProxy*>(pComponent));
                    }

                }
                else
                {
                    TCHAR szInfo[1024];
                    _stprintf(szInfo, _T("未能找到ID为%d的组件."), iComponentId);
                    MessageBox(NULL, szInfo, _T("搜索结果"), MB_OK);
                }
            }
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Never reach here!"));
        break;
    }
}

void CComponentRenderWindow::OnKeyPressed(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DELETE)
    {
        DeleteSelectedComponent();
        DeleteSelectedDependencyLine();
    }

    event.Skip(true);
}

void CComponentRenderWindow::DeleteSelectedComponent()
{
    CComponentEditorProxy* pSelectedComponent = m_pMainFrame->GetSelectedComponent();
    if (pSelectedComponent != NULL)
    {
        int iResult = wxMessageBox(_T("真的要删除这个组件吗？"), _T("删除确认"), wxYES_NO | wxCENTRE);
        if (iResult == wxYES)
        {
            CComponentManager::GetInstance()->DeleteComponent(pSelectedComponent);
            m_pMainFrame->SelecteComponent(NULL);
        }
    }
}

void CComponentRenderWindow::DeleteSelectedDependencyLine()
{
    if (m_pSelectedDependencyLine != NULL)
    {
        int iResult = wxMessageBox(_T("真的要删除这个依赖吗？"), _T("删除确认"), wxYES_NO | wxCENTRE);
        if (iResult == wxYES)
        {
            BEATS_SAFE_DELETE(m_pSelectedDependencyLine);
        }
    }
}

void CComponentRenderWindow::UpdateAllDependencyLine()
{
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = CComponentManager::GetInstance()->GetInstance()->GetComponentInstanceMap();
    std::map<size_t, std::map<size_t, CComponentBase*>*>::const_reverse_iterator iter = pInstanceMap->rbegin();
    for (; iter != pInstanceMap->rend(); ++iter)
    {
        std::map<size_t, CComponentBase*>::const_reverse_iterator subIter = iter->second->rbegin();
        for (; subIter != iter->second->rend(); ++subIter)
        {
            CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(subIter->second);
            size_t uDepedencyCount = pComponent->GetDependencies().size();
            for (size_t i = 0; i < uDepedencyCount; ++i)
            {
                CDependencyDescription* pDesc = pComponent->GetDependency(i);
                size_t uDependencyLineCount = pDesc->GetDependencyLineCount();
                for (size_t j = 0; j < uDependencyLineCount; ++j)
                {
                    pDesc->GetDependencyLine(j)->UpdateRect(m_cellSize);
                }
            }
        }
    }
}

void CComponentRenderWindow::SetDraggingFileName(const TCHAR* pFileName)
{
    m_draggingFilePath.clear();
    if (pFileName != NULL)
    {
        m_draggingFilePath.assign(pFileName);
    }    
}

const TCHAR* CComponentRenderWindow::GetDraggingFileName()
{
    return m_draggingFilePath.c_str();
}
