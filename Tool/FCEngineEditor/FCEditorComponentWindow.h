#ifndef FCENGINEEDITOR_FCEDITORCOMPONENTWINDOW_H__INCLUDE
#define FCENGINEEDITOR_FCEDITORCOMPONENTWINDOW_H__INCLUDE

#include "wx/glcanvas.h"

class CCamera;
class CShaderProgram;
class CComponentEditorProxy;
class CDependencyDescription;
class CDependencyDescriptionLine;
class CEditorMainFrame;

class CFCEditorComponentWindow : public wxGLCanvas
{
public:
    CFCEditorComponentWindow(wxWindow *parent,
        wxGLContext* pShareContext = NULL,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("FCEditorGLWindow"));

    virtual ~CFCEditorComponentWindow();
    wxGLContext* GetGLContext() const;

    void UpdateAllDependencyLine();
    void DeleteSelectedComponent();
    void DeleteSelectedDependencyLine();

    void SetDraggingFileName(const TCHAR* pFileName);
    const TCHAR* GetDraggingFileName();
    void SetDraggingComponent(CComponentEditorProxy* pDraggingComponent);
    CComponentEditorProxy* GetDraggingComponent();

protected:
    void OnIdle(wxIdleEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseMidScroll(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseRightUp(wxMouseEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnComponentMenuClicked(wxMenuEvent& event);

private:
    void ResetProjectionMode();
    void UpdateCamera();
    void Render();
    void RenderGridLine();
    void RenderComponents();
    void RenderDraggingDependencyLine();

    void ConvertWorldPosToGridPos(const kmVec2* pVectorPos, int* pOutGridX, int* pOutGridY);
    void ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY);
    void ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY);
    CComponentEditorProxy* HitTestForComponent( wxPoint pos, enum EComponentAeraRectType* pOutAreaType = NULL, void** pReturnData = NULL);


private:
    int m_iWidth;
    int m_iHeight;
    float m_fCellSize;
    wxGLContext* m_glRC;
    CCamera* m_pCamera;
    CEditorMainFrame* m_pMainFrame;
    TString m_draggingFilePath;
    CShaderProgram* m_pLineProgram;
    CComponentEditorProxy* m_pDraggingComponent;
    CComponentEditorProxy* m_pConnectComponent;
    CComponentEditorProxy* m_pCopyComponent;
    CDependencyDescription* m_pDraggingDependency;
    CDependencyDescriptionLine* m_pSelectedDependencyLine;
    wxMenu*    m_pComponentMenu;

    kmVec2 m_startDragPos;

    wxDECLARE_NO_COPY_CLASS(CFCEditorComponentWindow);
    DECLARE_EVENT_TABLE()
};
#endif