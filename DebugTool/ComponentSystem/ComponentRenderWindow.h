#ifndef BEATS_COMPONENTS_COMPONENTRENDERWINDOW_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENTRENDERWINDOW_H__INCLUDE

#include <d3dx9.h>

class CComponentEditorProxy;
class CDependencyDescription;
class CDependencyDescriptionLine;
class CBDTWxFrame;

class CComponentRenderWindow : public wxWindow
{
public:
    CComponentRenderWindow(wxWindow *parent, wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0);

    ~CComponentRenderWindow();

    IDirect3DDevice9* GetRenderDevice() const;
    LPD3DXFONT GetRenderFont() const;
    void SetDraggingComponent(CComponentEditorProxy* pDraggingComponent);
    CComponentEditorProxy* GetDraggingComponent();
    void UpdateAllDependencyLine();
    void SetDraggingFileName(const TCHAR* pFileName);
    const TCHAR* GetDraggingFileName();
protected:
    void OnIdle( wxIdleEvent& event );
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseMidScroll(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseRightUp(wxMouseEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnComponentMenuClicked(wxMenuEvent& event);
    void OnKeyPressed(wxKeyEvent& event);

private:
    bool InitDirectXRender(const HWND& hwnd, int width, int height, bool bWindow);
    bool ResetDevice(bool bRestore = true);
    bool ReleaseDirectX();
    void Render();
    void RenderGridLine();
    void RenderComponents();
    void RenderDraggingDependencyLine();
    void ConvertWorldPosToGridPos(const D3DXVECTOR2* pVectorPos, int* pOutGridX, int* pOutGridY);
    void ConvertGridPosToWorldPos( int gridX, int gridY, float* pOutWorldPosX, float* pOutWorldPosY);
    void ConvertWindowPosToWorldPos(const wxPoint& windowPos, float* pOutWorldPosX, float* pOutWorldPosY);
    CComponentEditorProxy* HitTestForComponent(wxPoint pos, enum EComponentAeraRectType* pOutAreaType = NULL, void** pReturnData = NULL);
    void DeleteSelectedComponent();
    void DeleteSelectedDependencyLine();

private:
    bool m_bInit;
    float m_cellSize;
    D3DXVECTOR2    m_currentClientPos;    // Center pos of screen.
    D3DXVECTOR2    m_startDragPos;
    D3DCOLOR m_backGroundColor;
    D3DCOLOR m_gridLineColor;
    IDirect3DDevice9* m_pD3dDevice;
    LPD3DXFONT m_pDXFont;
    D3DPRESENT_PARAMETERS m_d3dPresentParam;
    CComponentEditorProxy* m_pDraggingComponent;
    CComponentEditorProxy* m_pConnectComponent;
    CComponentEditorProxy* m_pCopyComponent;
    CDependencyDescription* m_pDraggingDependency;
    CDependencyDescriptionLine* m_pSelectedDependencyLine;
    wxMenu*    m_pComponentMenu;
    CBDTWxFrame* m_pMainFrame;
    TString m_draggingFilePath;
    D3DXMATRIX m_projectionMatrix;
};

#endif