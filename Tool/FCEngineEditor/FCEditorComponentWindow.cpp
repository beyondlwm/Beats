#include "stdafx.h"
#include <wx/numdlg.h>
#include "FCEditorComponentWindow.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"
#include "Render/RenderManager.h"
#include "Render/Renderer.h"
#include "Render/Camera.h"
#include "Render/ShaderProgram.h"
#include "Render/Shader.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentEditorProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescriptionLine.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"

BEGIN_EVENT_TABLE(CFCEditorComponentWindow, wxGLCanvas)
    EVT_SIZE(CFCEditorComponentWindow::OnSize)
    EVT_IDLE(CFCEditorComponentWindow::OnIdle)
    EVT_ERASE_BACKGROUND(CFCEditorComponentWindow::OnEraseBackground)
    EVT_MOUSEWHEEL(CFCEditorComponentWindow::OnMouseMidScroll)
    EVT_MOTION(CFCEditorComponentWindow::OnMouseMove)
    EVT_RIGHT_UP(CFCEditorComponentWindow::OnMouseRightUp)
    EVT_RIGHT_DOWN(CFCEditorComponentWindow::OnMouseRightDown)
    EVT_LEFT_UP(CFCEditorComponentWindow::OnMouseLeftUp)
    EVT_LEFT_DOWN(CFCEditorComponentWindow::OnMouseLeftDown)

END_EVENT_TABLE()

static const float MAX_Z_VALUE = -1000.f;
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

CFCEditorComponentWindow::CFCEditorComponentWindow(wxWindow *parent,
                                                     wxGLContext* pContext,
                                                     wxWindowID id,
                                                     const wxPoint& pos,
                                                     const wxSize& size, long style,
                                                     const wxString& name)
                                                     : wxGLCanvas(parent, id, NULL, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
                                                     , m_glRC(NULL)
                                                     , m_pCamera(NULL)
                                                     , m_iWidth(0)
                                                     , m_iHeight(0)
                                                     , m_fCellSize(15.0)
                                                     , m_pLineProgram(NULL)
                                                     , m_pDraggingComponent(NULL)
                                                     , m_pConnectComponent(NULL)
                                                     , m_pCopyComponent(NULL)
                                                     , m_pDraggingDependency(NULL)
                                                     , m_pSelectedDependencyLine(NULL)
{
    if (pContext)
    {
        m_glRC = pContext;
    }
    else
    {
        m_glRC = new wxGLContext(this);
    }
    m_pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    m_pCamera = new CCamera(CCamera::eCT_2D);
    kmVec2Fill(&m_startDragPos, 0, 0);

    m_pComponentMenu = new wxMenu;
    m_pComponentMenu->Append(eCMT_Copy, _T("拷贝"));
    m_pComponentMenu->Append(eCMT_Paste, _T("粘贴"));
    m_pComponentMenu->Append(eCMT_Delete, _T("删除"));
    m_pComponentMenu->Append(eCMT_Search, _T("搜索"));
    m_pComponentMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CFCEditorComponentWindow::OnComponentMenuClicked), NULL, this);
}

CFCEditorComponentWindow::~CFCEditorComponentWindow()
{
    BEATS_SAFE_DELETE(m_pCamera);
    BEATS_SAFE_DELETE(m_pComponentMenu);
    BEATS_SAFE_DELETE(m_pCopyComponent);
}

wxGLContext* CFCEditorComponentWindow::GetGLContext() const
{
    return m_glRC;
}

void CFCEditorComponentWindow::UpdateAllDependencyLine()
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
                    pDesc->GetDependencyLine(j)->UpdateRect(m_fCellSize, true);
                }
            }
        }
    }
}

void CFCEditorComponentWindow::DeleteSelectedComponent()
{
    CComponentBase* pSelectedComponent = m_pMainFrame->GetSelectedComponent();
    if (pSelectedComponent != NULL)
    {
        int iResult = wxMessageBox(_T("真的要删除这个组件吗？"), _T("删除确认"), wxYES_NO | wxCENTRE);
        if (iResult == wxYES)
        {
            CComponentManager::GetInstance()->DeleteComponent(pSelectedComponent);
            m_pMainFrame->SelectComponent(NULL);
        }
    }
}

void CFCEditorComponentWindow::DeleteSelectedDependencyLine()
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

void CFCEditorComponentWindow::OnIdle(wxIdleEvent& /*event*/)
{
    Render();
}

void CFCEditorComponentWindow::Render()
{
    FC_CHECK_GL_ERROR_DEBUG();
    if (m_pLineProgram == NULL)
    {
        SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("PointColorShader.vs"), false);
        SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("PointColorShader.ps"), false);

        m_pLineProgram = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
    }
    if ( IsShownOnScreen() )
    {
        SetCurrent(*m_glRC);
    }
    CRenderer* pRenderer = CRenderer::GetInstance();
    CRenderManager::GetInstance()->SetCamera(m_pCamera);
    pRenderer->UseProgram(m_pLineProgram->ID());
    pRenderer->Viewport(0, 0, m_iWidth, m_iHeight);
    pRenderer->ClearColor((float)0x4 / 0xFF, (float)0x40 / 0xFF, (float)0x80 / 0xFF, 1.0f);
    pRenderer->ClearDepth(1.0F);
    pRenderer->ClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderGridLine();

    RenderDraggingDependencyLine();

    CSpriteFrameBatchManager::GetInstance()->Clear();
    RenderComponents();

    pRenderer->LineWidth(1.0f);
    CRenderManager::GetInstance()->ContextFlush();
    CSpriteFrameBatchManager::GetInstance()->Render();
    SwapBuffers();
}

void CFCEditorComponentWindow::RenderGridLine()
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
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    const wxSize currentSize = GetSize();
    size_t clientAreaGridCountX = currentSize.x / m_fCellSize;
    size_t clientAreaGridCountY = currentSize.y / m_fCellSize;

    kmVec3 cameraPos;
    m_pCamera->GetViewPos(cameraPos);
    kmVec2 startPos;
    startPos.x = -cameraPos.x - clientAreaGridCountX * m_fCellSize;
    startPos.y = -cameraPos.y - clientAreaGridCountY * m_fCellSize;

    // Clip the pos to grid.
    startPos.x = (int)(startPos.x / m_fCellSize) * m_fCellSize;
    startPos.y = (int)(startPos.y / m_fCellSize) * m_fCellSize;

    for (size_t j = 0; j < clientAreaGridCountY * 2; ++j)
    {
        CVertexPC startPoint;
        startPoint.color = 0x60606060;
        startPoint.position.x = startPos.x;
        startPoint.position.y = startPos.y + j * m_fCellSize;
        startPoint.position.z = MAX_Z_VALUE;
        
        CVertexPC endPoint;
        endPoint.color = 0x60606060;
        endPoint.position.x = startPos.x + clientAreaGridCountX * 2 * m_fCellSize;
        endPoint.position.y = startPos.y + j * m_fCellSize;
        endPoint.position.z = MAX_Z_VALUE;

        pRenderMgr->RenderLine(startPoint, endPoint);
    }
    for (size_t i = 0; i < clientAreaGridCountX * 2; ++i)
    {
        CVertexPC startPoint;
        startPoint.color = 0x60606060;
        startPoint.position.x = startPos.x + i * m_fCellSize;
        startPoint.position.y = startPos.y;
        startPoint.position.z = MAX_Z_VALUE;

        CVertexPC endPoint;
        endPoint.color = 0x60606060;
        endPoint.position.x = startPos.x + i * m_fCellSize;
        endPoint.position.y = startPos.y + clientAreaGridCountY * 2 * m_fCellSize;
        endPoint.position.z = MAX_Z_VALUE;

        pRenderMgr->RenderLine(startPoint, endPoint);
    }

    //Draw Center Pos
    CVertexPC tmp[4];
    tmp[0].position.x = 0;
    tmp[0].position.y = m_fCellSize;
    tmp[0].position.z = MAX_Z_VALUE;
    tmp[0].color = 0xffffffff;

    tmp[1].position.x = 0;
    tmp[1].position.y = -m_fCellSize;
    tmp[1].position.z = MAX_Z_VALUE;
    tmp[1].color = 0xffffffff;


    tmp[2].position.x = -m_fCellSize;
    tmp[2].position.y = 0;
    tmp[2].position.z = MAX_Z_VALUE;
    tmp[2].color = 0xffffffff;

    tmp[3].position.x = m_fCellSize;
    tmp[3].position.y = 0;
    tmp[3].position.z = MAX_Z_VALUE;
    tmp[3].color = 0xffffffff;
    pRenderMgr->RenderLine(tmp[0], tmp[1]);
    pRenderMgr->RenderLine(tmp[2], tmp[3]);
}

void CFCEditorComponentWindow::RenderComponents()
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
                pGraphic->OnRender(m_fCellSize, pComponent == m_pMainFrame->GetSelectedComponent());
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
        kmVec2 worldPos;
        kmVec2Fill(&worldPos, fMouseWorldPosX, fMouseWorldPosY);
        ConvertWorldPosToGridPos(&worldPos, &x, &y);
        pDraggingGraphics->SetPosition(x, y);
        pDraggingGraphics->OnRender(m_fCellSize, true);

        //BEATS_PRINT(_T("You must be dragging at world Pos%f, %f gridPos %d %d\n"), fMouseWorldPosX, fMouseWorldPosY, x, y);
    }

}

void CFCEditorComponentWindow::RenderDraggingDependencyLine()
{
    CComponentEditorProxy* pSelectedComponent = m_pMainFrame->GetSelectedComponent();
    if (wxGetMouseState().LeftIsDown() && pSelectedComponent)
    {
        if (m_pDraggingDependency != NULL)
        {
            int iDependencyPosX = 0; 
            int iDependencyPosY = 0;
            size_t uIndex = m_pDraggingDependency->GetIndex();
            pSelectedComponent->GetGraphics()->GetDependencyPosition(uIndex, &iDependencyPosX, &iDependencyPosY);
            CVertexPC line[2];
            ConvertGridPosToWorldPos(iDependencyPosX, iDependencyPosY, &line[0].position.x, &line[0].position.y);
            line[0].position.x += m_fCellSize * 0.5f * pSelectedComponent->GetGraphics()->GetDependencyWidth();
            line[0].position.y -= m_fCellSize * 0.5f * pSelectedComponent->GetGraphics()->GetDependencyHeight();
            wxPoint pos = wxGetMouseState().GetPosition();
            // Convert screen mouse position to rendering area position
            wxPoint clientPos = ScreenToClient(wxGetMousePosition());
            ConvertWindowPosToWorldPos(clientPos, &line[1].position.x, &line[1].position.y);
            line[0].color = (m_pConnectComponent != NULL) ? 0x00ff00ff : 0xff0000ff;
            line[1].color = (m_pConnectComponent != NULL) ? 0x00ff00ff : 0xff0000ff;
            CRenderManager::GetInstance()->RenderLine(line[0], line[1]);
        }
    }
}

void CFCEditorComponentWindow::ConvertWorldPosToGridPos(const kmVec2* pVectorPos, int* pOutGridX, int* pOutGridY)
{
    *pOutGridX = floor(pVectorPos->x / m_fCellSize);
    *pOutGridY = ceil(pVectorPos->y / m_fCellSize);
}

void CFCEditorComponentWindow::ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY)
{
    *pOutWorldPosX = gridX * m_fCellSize;
    *pOutWorldPosY = gridY * m_fCellSize;
}

void CFCEditorComponentWindow::ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY)
{
    wxSize clientSize = GetClientSize();
    kmVec3 cameraPos;
    m_pCamera->GetViewPos(cameraPos);
    *pOutWorldPosX = (windowPos.x - clientSize.x * 0.5f) - cameraPos.x;
    *pOutWorldPosY = (windowPos.y - clientSize.y * 0.5f) - cameraPos.y;
}

void CFCEditorComponentWindow::OnSize(wxSizeEvent& /*event*/)
{
    ResetProjectionMode();
}

void CFCEditorComponentWindow::OnEraseBackground(wxEraseEvent& /*event*/)
{

}

void CFCEditorComponentWindow::OnMouseMidScroll(wxMouseEvent& event)
{
    // Resize render area.
    int rotation = event.GetWheelRotation();
    kmVec3 cameraPos;
    m_pCamera->GetViewPos(cameraPos);

    int gridPosX = cameraPos.x / m_fCellSize;
    int gridPosY = cameraPos.y / m_fCellSize;

    float delta = m_fCellSize* 0.1f * rotation / event.GetWheelDelta();
    m_fCellSize += delta;
    if (m_fCellSize <= MIN_CELL_SIZE)
    {
        m_fCellSize = MIN_CELL_SIZE;
    }
    float minSize = min(GetSize().GetWidth(), GetSize().GetHeight());
    if (m_fCellSize > minSize)
    {
        m_fCellSize = minSize;
    }
    m_pCamera->SetViewPos(gridPosX * m_fCellSize, gridPosY * m_fCellSize, cameraPos.z);
    UpdateAllDependencyLine();
    event.Skip(true);
}

void CFCEditorComponentWindow::OnMouseMove(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    // Drag the screen
    if(event.RightIsDown())
    {
        float deltaX = pos.x - m_startDragPos.x;
        float deltaY = pos.y - m_startDragPos.y;
        kmVec3 cameraPos;
        m_pCamera->GetViewPos(cameraPos);
        cameraPos.x += deltaX;
        cameraPos.y += deltaY;
        m_pCamera->SetViewPos(cameraPos.x, cameraPos.y, cameraPos.z);
        m_startDragPos.x = pos.x;
        m_startDragPos.y = pos.y;
        //BEATS_PRINT(_T("Screen Moved, Current Client Pos %f, %f\n"), m_currentClientPos.x, m_currentClientPos.y);
    }
    if (event.LeftIsDown())
    {
        m_pConnectComponent = NULL;

        //Drag Component
        CComponentEditorProxy* pSelectedComponent = m_pMainFrame->GetSelectedComponent();
        if (pSelectedComponent != NULL)
        {
            if (m_pDraggingDependency == NULL)
            {
                kmVec2 worldPos;
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
                        pDesc->GetDependencyLine(j)->UpdateRect(m_fCellSize, true);
                    }
                }
                for (size_t i = 0; i < pSelectedComponent->GetBeConnectedDependencyLines().size(); ++i)
                {
                    CDependencyDescriptionLine* pDescLine = pSelectedComponent->GetBeConnectedDependencyLines()[i];
                    pDescLine->UpdateRect(m_fCellSize, true);
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
    event.Skip(true);
}

void CFCEditorComponentWindow::OnMouseRightUp(wxMouseEvent& event)
{
    if (CComponentManager::GetInstance()->GetProject()->GetRootDirectory() != NULL)
    {
        wxPoint pos = event.GetPosition();
        CComponentEditorProxy* pClickedComponent = HitTestForComponent(pos);
        m_pMainFrame->SelectComponent(pClickedComponent);
        m_pComponentMenu->Enable(eCMT_Copy, pClickedComponent != NULL);
        m_pComponentMenu->Enable(eCMT_Delete, pClickedComponent != NULL);
        m_pComponentMenu->Enable(eCMT_Paste, pClickedComponent == NULL && m_pCopyComponent != NULL);
        // Force render before menu pop up.
        Render();
        PopupMenu(m_pComponentMenu, wxDefaultPosition);
    }

    event.Skip(true);

}

void CFCEditorComponentWindow::OnMouseRightDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    m_startDragPos.x = pos.x;
    m_startDragPos.y = pos.y;
    event.Skip(true);
}

void CFCEditorComponentWindow::OnMouseLeftDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    m_startDragPos.x = pos.x;
    m_startDragPos.y = pos.y;
    EComponentAeraRectType aeraType = eCART_Invalid;
    void* pData = NULL;
    CComponentEditorProxy* pClickedComponent = HitTestForComponent(pos, &aeraType, &pData);
    BEATS_ASSERT(pClickedComponent == NULL || aeraType != eCART_Invalid);
    m_pMainFrame->SelectComponent(pClickedComponent);
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
    event.Skip(true);
}

void CFCEditorComponentWindow::OnMouseLeftUp(wxMouseEvent& /*event*/)
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
        pLine->UpdateRect(m_fCellSize, true);
    }
    if (m_pDraggingDependency != NULL && !m_pDraggingDependency->IsVisible())
    {
        m_pDraggingDependency->Show();
    }
    m_pConnectComponent = NULL;
    m_pDraggingDependency = NULL;
}

void CFCEditorComponentWindow::OnComponentMenuClicked(wxMenuEvent& event)
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
            kmVec2 mouseWorldPos;
            ConvertWindowPosToWorldPos(mouseWindowPos, &mouseWorldPos.x, &mouseWorldPos.y);
            int iGridPosX, iGridPosY;
            ConvertWorldPosToGridPos(&mouseWorldPos, &iGridPosX, &iGridPosY);
            pNewComponentEditorProxy->GetGraphics()->SetPosition(iGridPosX - 1, iGridPosY + 1);
        }
        break;
    case eCMT_Search:
        {
            int iComponentId = wxGetNumberFromUser(_T("搜索组件"), _T("组件ID"), _T("搜索组件"), 0, 0xFFFFFFFF, 0x7FFFFFFE);
            if (iComponentId != 0xFFFFFFFF)
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
                            //TODO:How To Activate File?
                            //m_pMainFrame->ActivateFile(filePath.c_str());
                            CComponentBase* pComponent = CComponentManager::GetInstance()->GetComponentInstance(iComponentId);
                            m_pMainFrame->SelectComponent(static_cast<CComponentEditorProxy*>(pComponent));
                        }
                    }
                    else
                    {
                        CComponentBase* pComponent = CComponentManager::GetInstance()->GetComponentInstance(iComponentId);
                        m_pMainFrame->SelectComponent(static_cast<CComponentEditorProxy*>(pComponent));
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

CComponentEditorProxy* CFCEditorComponentWindow::HitTestForComponent( wxPoint pos, EComponentAeraRectType* pOutAreaType/* = NULL*/, void** pReturnData /* = NULL*/ )
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
            float width = pGraphics->GetWidth() * m_fCellSize;
            float height = pGraphics->GetHeight() * m_fCellSize;
            float fDelatX = fClickWorldPosX - fWorldPosX;
            if ( fDelatX > 0 && fDelatX < width)
            {
                float fDelatY = fClickWorldPosY - fWorldPosY;
                if (fDelatY > 0 && fDelatY < height)
                {
                    pClickedComponent = static_cast<CComponentEditorProxy*>(subIter->second);
                    if (pOutAreaType != NULL)
                    {
                        kmVec2 worldPos;
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
                    bool bInLine = pLine->HitTest(fClickWorldPosX, fClickWorldPosY);
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

void CFCEditorComponentWindow::ResetProjectionMode()
{
    if ( IsShownOnScreen() )
    {
        SetCurrent(*m_glRC);
    }
    GetClientSize(&m_iWidth, &m_iHeight);
    m_pCamera->SetWidth(m_iWidth);
    m_pCamera->SetHeight(m_iHeight);
    kmVec2 centerOffset;
    kmVec2Fill(&centerOffset, m_iWidth * -0.5f, m_iHeight * -0.5f);
    m_pCamera->SetCameraCenterOffset(centerOffset);
}

void CFCEditorComponentWindow::SetDraggingFileName( const TCHAR* pFileName )
{
    m_draggingFilePath.clear();
    if (pFileName != NULL)
    {
        m_draggingFilePath.assign(pFileName);
    }  
}

const TCHAR* CFCEditorComponentWindow::GetDraggingFileName()
{
    return m_draggingFilePath.c_str();
}

void CFCEditorComponentWindow::SetDraggingComponent( CComponentEditorProxy* pDraggingComponent )
{
    m_pDraggingComponent = pDraggingComponent;
}

CComponentEditorProxy* CFCEditorComponentWindow::GetDraggingComponent()
{
    return m_pDraggingComponent;
}
