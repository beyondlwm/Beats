#ifndef FCENGINEEDITOR_MAINAPP_H__INCLUDE
#define FCENGINEEDITOR_MAINAPP_H__INCLUDE

class wxGLContext;
class wxGLCanvas;
class wxPGEditor;
class CEditorMainFrame;
class CEngineEditor : public wxApp
{
public:
    CEngineEditor();
    virtual ~CEngineEditor();

    virtual bool OnInit();
    virtual int OnExit();

    void CreatePtrEditor();
    wxPGEditor* GetPtrEditor() const;
    CEditorMainFrame* GetMainFrame() const;
    void CreateGLContext(wxGLCanvas* pCanvas);
    wxGLContext* GetGLContext() const;
    const TString& GetWorkingPath();
private:

    wxGLContext* m_glRC;
    CEditorMainFrame* m_pMainFrame;
    wxPGEditor* m_pPtrButtonEditor;
    TString m_szWorkingPath;
};


#endif