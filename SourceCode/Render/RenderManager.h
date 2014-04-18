#ifndef RENDER_RENDERMANAGER_H__INCLUDE
#define RENDER_RENDERMANAGER_H__INCLUDE

class CShaderProgram;
class CCamera;
class CTexture;
struct GLFWwindow;

class CRenderManager
{
    BEATS_DECLARE_SINGLETON(CRenderManager);
public:

    bool InitializeWithWindow(size_t uWidth, size_t uHeight);
    bool Initialize();
    void SetWindowSize(int m_iWidth, int m_iHeight);
    void GetWindowSize(int& nWidth, int& nHeight);
    bool InitShaderFile();
#ifdef USE_UBO
    void InitUBOList();
    void DeleteUBOList();
    GLuint GetUBO(ECommonUniformBlockType type) const;
    void UpdateUBO(ECommonUniformBlockType type, const GLvoid **data, const GLsizeiptr *size, size_t count);
#endif
    void UpdateCamera();
    void RenderCoordinate(const kmMat4* pMatrix);
    void RenderLine(const CVertexPC& start,const CVertexPC& end);
    void RenderTriangle(const CVertexPC& pt1, const CVertexPC& pt2, const CVertexPC& pt3);

    GLFWwindow* GetMainWindow() const;
    CCamera* GetCamera() const;
    void SetCamera(CCamera* camera);
    void ApplyTexture( int index, GLuint texture );

    bool InitGlew();
    void SwapBuffers();
    void Render();
    void SwitchPolygonMode();

    static bool glew_dynamic_binding();

    static void onGLFWError(int errorID, const char* errorDesc);
    static void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify);
    static void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y);
    static void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
    static void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void onGLFWCharCallback(GLFWwindow* window, unsigned int character);
    static void onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y);
    static void onGLFWframebuffersize(GLFWwindow* window, int w, int h);

    bool IsLeftMouseDown() { return m_bLeftMouseDown; }
    void GetMousePos(size_t& x, size_t& y) { x = m_uCurMousePosX, y = m_uCurMousePosY; }

    CShaderProgram* GetShaderProgram(GLuint uVertexShader, GLuint uPixelShader);

    void ContextFlush();
private:
    bool InitGridData();
    bool InitLineBuffer();
    bool InitTriangleBuffer();
    void RenderGrid();
    void RenderLineImpl();
    void RenderTriangleImpl();

private:
#ifdef USE_UBO
    typedef std::map<ECommonUniformBlockType, GLuint> UBOList;
    UBOList m_UBOList;
#endif

    bool m_bIsRenta;
    bool m_bLeftMouseDown;
    GLFWwindow* m_pMainWindow;
    int m_iWidth;
    int m_iHeight;
    CCamera* m_pCamera;
    GLuint m_uCurrPolygonMode;
    size_t m_uLastMousePosX;
    size_t m_uLastMousePosY;
    size_t m_uCurMousePosX;
    size_t m_uCurMousePosY;
    float m_fPressStartYaw;
    float m_fPressStartPitch;
    GLuint m_uGridVAO;
    GLuint m_uGridVBO;
    GLuint m_uGridVertexCount;
    GLuint m_uLineVAO;
    GLuint m_uLineVBO;
    GLuint m_uTriangleVAO;
    GLuint m_uTriangleVBO;
    // TODO: Use material to replace it!
    CShaderProgram* m_pLineShaderProgram;

    std::vector<CVertexPC> m_renderLines;
    std::vector<CVertexPC> m_renderTriangles;
    std::vector<CShaderProgram*> m_shaderProgramPool;
};

#endif