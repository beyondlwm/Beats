#include "stdafx.h"
#include "RenderManager.h"
#include "ShaderProgram.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "RenderObjectManager.h"
#include "CCConfiguration.h"
#include "Camera.h"
#include "Texture.h"
#include "SpriteFrameBatchManager.h"
#include "Resource/ResourceManager.h"
#include "Spline/Curve.h"
#include "Render/RenderState.h"

#include "renderer.h"
#include "GUI/System.h"
#include "Event/MouseEvent.h"
#include "Event/EventType.h"
#include "Event/KeyboardEvent.h"

CRenderManager* CRenderManager::m_pInstance = NULL;

CRenderManager::CRenderManager()
    : m_pMainWindow(NULL)
    , m_iWidth(0)
    , m_iHeight(0)
    , m_bIsRenta(false)
    , m_pCamera(new CCamera)
    , m_bLeftMouseDown(false)
    , m_bRightMouseDown(false)
    , m_uCurrPolygonMode(GL_FILL)
    , m_uPressPosX(0)
    , m_uPressPosY(0)
    , m_uCurMousePosX(0)
    , m_uCurMousePosY(0)
    , m_uGridVAO(0)
    , m_uGridVBO(0)
    , m_uGridVertexCount(0)
    , m_uLineVAO(0)
    , m_uLineVBO(0)
    , m_uTriangleVAO(0)
    , m_uTriangleVBO(0)
    , m_fPressStartYaw(0.f)
    , m_fPressStartPitch(0.f)
{

}

CRenderManager::~CRenderManager()
{
#ifdef USE_UBO
    DeleteUBOList();
#endif
    BEATS_SAFE_DELETE(m_pCamera);
}

bool CRenderManager::InitializeWithWindow(size_t uWidth, size_t uHeight)
{
    glfwSetErrorCallback(CRenderManager::onGLFWError);
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    m_pMainWindow = glfwCreateWindow(uWidth, uHeight, "Unknown window", NULL, NULL);
    glfwMakeContextCurrent(m_pMainWindow);

    glfwGetWindowSize(m_pMainWindow, &m_iWidth, &m_iHeight);
    int frameBufferW, frameBufferH;
    glfwGetFramebufferSize(m_pMainWindow, &frameBufferW, &frameBufferH);

    m_bIsRenta = frameBufferW == 2 * m_iWidth && frameBufferH == 2 * m_iHeight;
    if (m_bIsRenta)
    {
        glfwSetWindowSize(m_pMainWindow, uWidth / 2, uHeight / 2);
    }

    glfwSetMouseButtonCallback(m_pMainWindow,CRenderManager::onGLFWMouseCallBack);
    glfwSetCursorPosCallback(m_pMainWindow,CRenderManager::onGLFWMouseMoveCallBack);
    glfwSetScrollCallback(m_pMainWindow, CRenderManager::onGLFWMouseScrollCallback);
    glfwSetCharCallback(m_pMainWindow, CRenderManager::onGLFWCharCallback);
    glfwSetKeyCallback(m_pMainWindow, CRenderManager::onGLFWKeyCallback);
    glfwSetWindowPosCallback(m_pMainWindow, CRenderManager::onGLFWWindowPosCallback);
    glfwSetFramebufferSizeCallback(m_pMainWindow, CRenderManager::onGLFWframebuffersize);

    bool bRet = Initialize();

    kmVec2 resolution;
    kmVec2Fill(&resolution, (float)m_iWidth, (float)m_iHeight);
    FCGUI::System::GetInstance()->OnResolutionChanged(resolution);

    return bRet;
}

bool CRenderManager::Initialize()
{
    // check OpenGL version at first
#ifdef _DEBUG
    const GLubyte* glVersion = glGetString(GL_VERSION);
    TCHAR szVersionTCHAR[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToTCHAR((const char*)glVersion, szVersionTCHAR, MAX_PATH);
    BEATS_ASSERT( atof((const char*)glVersion) >= 1.5, _T("OpenGL 1.5 or higher is required (your version is %s). Please upgrade the driver of your video card."), szVersionTCHAR);
#endif
    // Enable point size by default.
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    bool bRet = InitGlew() && InitShaderFile();

    if (bRet)
    {
        // Configuration. Gather GPU info
        cocos2d::Configuration *conf = cocos2d::Configuration::getInstance();
        conf->gatherGPUInfo();
        CCLOG("%s\n",conf->getInfo().c_str());

        CHECK_GL_ERROR_DEBUG();
    }

    bRet = bRet && InitGridData() && InitLineBuffer() && InitTriangleBuffer();
    BEATS_ASSERT(bRet, _T("Initialize render manager failed!"));
#ifdef USE_UBO
    InitUBOList();
#endif
    return bRet;
}

void CRenderManager::SetWindowSize(int width, int height)
{
    m_iWidth = width;
    m_iHeight = height;
}

bool CRenderManager::InitShaderFile()
{
    bool bRet = true;

    return bRet;
}

#ifdef USE_UBO
void CRenderManager::InitUBOList()
{
    for(int i = 0; i < UNIFORM_BLOCK_COUNT; ++i)
    {
        ECommonUniformBlockType type = static_cast<ECommonUniformBlockType>(i);
        GLuint ubo;
        CRenderer::GetInstance()->GenBuffers(1, &ubo);
        m_UBOList[type] = ubo;
    }
}

void CRenderManager::DeleteUBOList()
{
    for(auto ubopair : m_UBOList)
    {
        CRenderer::GetInstance()->DeleteBuffers(1, &ubopair.second);
    }
    m_UBOList.clear();
}

GLuint CRenderManager::GetUBO(ECommonUniformBlockType type) const
{
    GLuint uRet = 0;
    auto itr = m_UBOList.find(type);
    if(itr != m_UBOList.end())
    {
        uRet = itr->second;
    }
    return uRet;
}

void CRenderManager::UpdateUBO(ECommonUniformBlockType type, const GLvoid **data, const GLsizeiptr *size, size_t count)
{
    GLuint ubo = GetUBO(type);
    if(ubo >= 0)
    {
        GLsizeiptr totalsize = 0;
        for(size_t i = 0; i < count; ++i)
        {
            totalsize += size[i];
        }
        CRenderer::GetInstance()->BindBuffer(GL_UNIFORM_BUFFER, ubo);
        CRenderer::GetInstance()->BufferData(GL_UNIFORM_BUFFER, totalsize, 0, GL_DYNAMIC_DRAW);

        GLsizeiptr offset = 0;
        for(size_t i = 0; i < count; ++i)
        {
            CRenderer::GetInstance()->BufferSubData(GL_UNIFORM_BUFFER, offset, size[i], data[i]);
            offset += size[i];
        }
    }
}
#endif

bool CRenderManager::glew_dynamic_binding()
{
    const char *gl_extensions = (const char*)glGetString(GL_EXTENSIONS);

    // If the current opengl driver doesn't have framebuffers methods, check if an extension exists
    if (glGenFramebuffers == NULL)
    {
        BEATS_PRINT(_T("OpenGL: glGenFramebuffers is NULL, try to detect an extension"));
        if (strstr(gl_extensions, "ARB_framebuffer_object"))
        {
            BEATS_PRINT(_T("OpenGL: ARB_framebuffer_object is supported"));

            glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbuffer");
            glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbuffer");
            glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffers");
            glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffers");
            glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorage");
            glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameteriv");
            glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebuffer");
            glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebuffer");
            glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffers");
            glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffers");
            glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatus");
            glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1D");
            glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2D");
            glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3D");
            glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbuffer");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
            glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap");
        }
        else
            if (strstr(gl_extensions, "EXT_framebuffer_object"))
            {
                BEATS_PRINT(_T("OpenGL: EXT_framebuffer_object is supported"));
                glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbufferEXT");
                glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbufferEXT");
                glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
                glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffersEXT");
                glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorageEXT");
                glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameterivEXT");
                glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebufferEXT");
                glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebufferEXT");
                glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
                glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffersEXT");
                glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
                glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1DEXT");
                glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
                glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3DEXT");
                glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");
                glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
                glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmapEXT");
            }
            else
            {
                BEATS_PRINT(_T("OpenGL: No framebuffers extension is supported"));
                BEATS_PRINT(_T("OpenGL: Any call to Fbo will crash!"));
                return false;
            }
    }
    return true;
}

bool CRenderManager::InitGlew()
{
    GLenum GlewInitResult = glewInit();
    bool bRet = GlewInitResult == GLEW_OK;
    BEATS_ASSERT(bRet, _T("OpenGL error: %s"), (TCHAR *)glewGetErrorString(GlewInitResult));
    BEATS_WARNING(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader, _T("GLSL is not ready!"));
    BEATS_WARNING(glewIsSupported("GL_VERSION_2_0"), _T("OpenGL 2.0 is not supported!"));
    bool bFrameBufferSupport = glew_dynamic_binding();
    BEATS_ASSERT(bFrameBufferSupport,  _T("No OpenGL framebuffer support. Please upgrade the driver of your video card."));
    return bRet && bFrameBufferSupport;
}
static float fSpeed = 0.05f;
static float fFrame = 0.0f;

void CRenderManager::SetupVPMatrix(bool proj2D)
{
    BEATS_ASSERT(m_iWidth > 0 && m_iHeight > 0);
    fFrame = fFrame + 0.016f;
    if ( fFrame > 0.1f )
    {
        fFrame = 0.0f;
        CRenderManager::GetInstance()->UpdateCamera();
    }
    kmMat4 matrixProj;

    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLLoadIdentity();

    if(proj2D)
    {
        kmMat4OrthographicProjection(&matrixProj, 
            0.0f, static_cast<kmScalar>(m_iWidth),
            static_cast<kmScalar>(m_iHeight), 0.0f,
            -1000.1f, 10000.0f);
    }
    else
    {
        kmMat4PerspectiveProjection(&matrixProj, 60, (float)m_iWidth/m_iHeight, 0.1f, 1000.0f);
    }

    kmGLMultMatrix(&matrixProj);

    kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLLoadIdentity();
    if(!proj2D)
    {
        m_pCamera->Update();
    }
    TransferMVPMatrix();
}

void CRenderManager::Render()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->Viewport(0, 0, m_iWidth, m_iHeight);
    pRenderer->ClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    pRenderer->ClearDepth(1.0F);
    pRenderer->ClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    pRenderer->EnableGL(GL_DEPTH_TEST);
    pRenderer->EnableGL(GL_CULL_FACE);

    CSpriteFrameBatchManager::GetInstance()->Clear();
    // 3D rendering
    CRenderObjectManager::GetInstance()->Render();
    SetupVPMatrix(false);
    SetFarPane(25.f);
    RenderGrid();
    RenderLineImpl();
    RenderTriangleImpl();

    // 2D rendering
    CSpriteFrameBatchManager::GetInstance()->Render();

    // GUI rendering
    CSpriteFrameBatchManager::GetInstance()->Clear();
    FCGUI::System::GetInstance()->Render();
    CSpriteFrameBatchManager::GetInstance()->Render();

    SwapBuffers();    
}

void CRenderManager::SwitchPolygonMode()
{
    if (m_uCurrPolygonMode == GL_FILL)
    {
        m_uCurrPolygonMode = GL_POINT;
    }
    else
    {
        ++m_uCurrPolygonMode;
    }
    CRenderer::GetInstance()->PointSize(5.0F);
    CRenderer::GetInstance()->PolygonMode(GL_FRONT_AND_BACK, m_uCurrPolygonMode);
}

void CRenderManager::SwapBuffers()
{
    glfwSwapBuffers(m_pMainWindow);
}

void CRenderManager::onGLFWError( int errorID, const char* errorDesc )
{
    TCHAR szBuffer[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToTCHAR(errorDesc, szBuffer, MAX_PATH);
    BEATS_ASSERT(false, _T("An error was detected! ID : %d\ndesc : %s"), errorID, szBuffer);
}

void CRenderManager::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify)
{
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();

    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            pRenderMgr->m_uPressPosX = pRenderMgr->m_uCurMousePosX;
            pRenderMgr->m_uPressPosY = pRenderMgr->m_uCurMousePosY;
            pRenderMgr->m_bLeftMouseDown = true;
        }
        else if(action == GLFW_RELEASE)
        {
            pRenderMgr->m_bLeftMouseDown = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            pRenderMgr->m_bRightMouseDown = true;
            pRenderMgr->m_uPressPosX = pRenderMgr->m_uCurMousePosX;
            pRenderMgr->m_uPressPosY = pRenderMgr->m_uCurMousePosY;
        }
        else if (action == GLFW_RELEASE)
        {
            pRenderMgr->m_bRightMouseDown = false;
        }
    }

    int eventType = 0;
    if(action == GLFW_PRESS)
    {
        eventType = EVENT_MOUSE_PRESS;
    }
    else if(action == GLFW_RELEASE)
    {
        eventType = EVENT_MOUSE_RELEASE;
    }

    MouseEvent event(eventType, button, 0);
    FCGUI::System::GetInstance()->InjectMouseEvent(&event);
}

void CRenderManager::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    pRenderMgr->m_uCurMousePosX = (size_t)x;
    pRenderMgr->m_uCurMousePosY = (size_t)y;

    int width = 0, height = 0;
    glfwGetFramebufferSize(window,&width, &height);

    if(x < width  && x > 0 && y < height && y > 0)
    {
        if (pRenderMgr->m_bRightMouseDown)
        {
            CCamera* pCamera = pRenderMgr->GetCamera();
            int iDeltaX = pRenderMgr->m_uCurMousePosX - pRenderMgr->m_uPressPosX;
            int iDeltaY = pRenderMgr->m_uCurMousePosY - pRenderMgr->m_uPressPosY;

            float fYawValue = -(float)iDeltaX / pRenderMgr->m_iWidth * 5.0F;
            pCamera->RotateX(fYawValue);

            float fPitchValue = (float)iDeltaY / pRenderMgr->m_iHeight * 5.0F;
            pCamera->RotateY( fPitchValue);
        }
        if (pRenderMgr->m_bLeftMouseDown)
        {
            CCamera* pCamera = pRenderMgr->GetCamera();
            int iDeltaX = pRenderMgr->m_uCurMousePosX - pRenderMgr->m_uPressPosX;
            int iDeltaY = pRenderMgr->m_uCurMousePosY - pRenderMgr->m_uPressPosY;

            float fYawValue = (float)iDeltaX / pRenderMgr->m_iWidth * 5.0F;
            pCamera->Yaw(fYawValue);

            float fPitchValue = (float)iDeltaY / pRenderMgr->m_iHeight * 5.0F;
            pCamera->Pitch( fPitchValue);
        }
    }

    MouseEvent event(EVENT_MOUSE_MOVE, 0, 0, (float)x, (float)y);
    FCGUI::System::GetInstance()->InjectMouseEvent(&event);
}

void CRenderManager::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
{
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    CCamera*  pCamera =  pRenderMgr->GetCamera();

    float fSpeed =  (float)y * 0.2F;
    pCamera->MoveStraight(-fSpeed);

    MouseEvent event(EVENT_MOUSE_SCROLL, 0, 0, (float)x, (float)y);
    FCGUI::System::GetInstance()->InjectMouseEvent(&event);
}

void CRenderManager::onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    CCamera* pCamera = CRenderManager::GetInstance()->GetCamera();

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        CRenderManager::GetInstance()->UpdateCamera();
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            case GLFW_KEY_R:
                kmVec3 eye, look, up;
                kmVec3Fill(&eye,0.0F,0.5F,10.0F);
                kmVec3Fill(&look,0.0F,0.0F,0.0F);
                kmVec3Fill(&up,0.0F,1.0F,0.0F);
                pCamera->SetCamera(eye, look, up);
                break;
            case GLFW_KEY_F3:
                {
                    CRenderManager::GetInstance()->SwitchPolygonMode();
                }
                break;
            default:
                break;
            }
        }
    }

    int eventType = 0;
    switch(action)
    {
    case GLFW_PRESS:
        eventType = EVENT_KEY_PRESS;
        break;
    case GLFW_RELEASE:
        eventType = EVENT_KEY_RELEASE;
        break;
    case GLFW_REPEAT:
        eventType = EVENT_KEY_REPEAT;
        break;
    }

    KeyboardEvent event(eventType, key, mods);
    FCGUI::System::GetInstance()->InjectKeyboardEvent(&event);
}

void CRenderManager::onGLFWCharCallback(GLFWwindow* window, unsigned int character)
{
    KeyboardEvent event(EVENT_CHAR, static_cast<int>(character));
    FCGUI::System::GetInstance()->InjectKeyboardEvent(&event);
}

void CRenderManager::onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y)
{

}

void CRenderManager::onGLFWframebuffersize(GLFWwindow* window, int w, int h)
{

}

void CRenderManager::ApplyTexture( int index, GLuint texture )
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->ActiveTexture(GL_TEXTURE0 + index);
    pRenderer->TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    pRenderer->TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    pRenderer->TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT );
    pRenderer->TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT );
    pRenderer->BindTexture(GL_TEXTURE_2D, texture);

    GLuint currProgram = pRenderer->GetCurrentState()->GetCurrentShaderProgram();
    BEATS_ASSERT(currProgram != 0);
    GLint textureLocation = pRenderer->GetUniformLocation(currProgram, COMMON_UNIFORM_NAMES[UNIFORM_TEX0 + index]);
    pRenderer->SetUniform1i( textureLocation, index);
}

GLFWwindow* CRenderManager::GetMainWindow() const
{
    return m_pMainWindow;
}

CCamera* CRenderManager::GetCamera() const
{
    return m_pCamera;
}

void CRenderManager::UpdateCamera()
{
    float fSpeed = 0;
    int type = NOMOVE;
    if ( glfwGetKey(CRenderManager::GetInstance()->GetMainWindow(),GLFW_KEY_LEFT_SHIFT) || glfwGetKey(CRenderManager::GetInstance()->GetMainWindow(),GLFW_KEY_RIGHT_SHIFT) )
    {
        fSpeed = 5.0F * 0.016F;
    }
    else
    {
        fSpeed = 1.0F * 0.016F;
    }

    if ( glfwGetKey(CRenderManager::GetInstance()->GetMainWindow(),GLFW_KEY_A) )
    {
        type = MOVETRANVERSE;
    }
    if ( glfwGetKey(CRenderManager::GetInstance()->GetMainWindow(),GLFW_KEY_D) )
    {
        type = MOVETRANVERSE;
        fSpeed *= -1;
    } 
    if ( glfwGetKey(CRenderManager::GetInstance()->GetMainWindow(),GLFW_KEY_W) )
    {
        type = MOVESTRAIGHT;
        fSpeed *= -1;
    }
    if ( glfwGetKey(CRenderManager::GetInstance()->GetMainWindow(),GLFW_KEY_S) )
    {
        type = MOVESTRAIGHT;
    }
    if ( glfwGetKey(CRenderManager::GetInstance()->GetMainWindow(),GLFW_KEY_UP) )
    {
        type = MOVEUPDOWN;
        
    }
    if ( glfwGetKey(CRenderManager::GetInstance()->GetMainWindow(),GLFW_KEY_DOWN))
    {
        type = MOVEUPDOWN;
        fSpeed *= -1;
    }

    CRenderManager::GetInstance()->UpdateCamera(fSpeed, type);
}

void CRenderManager::UpdateCamera(float fSpeed, int type)
{

    CCamera* pCamera = CRenderManager::GetInstance()->GetCamera();
    switch (type)
    {
    case MOVETRANVERSE:
        pCamera->MoveTransverse(fSpeed);
        break;
    case MOVESTRAIGHT:
        pCamera->MoveStraight(fSpeed);
        break;
    case MOVEUPDOWN:
        pCamera->MoveUpDown(fSpeed);
        break;
    default:
        break;
    }
}

void CRenderManager::SetFarPane(float fFarPane)
{
    //Reset the far plane.
    kmMat4 matrixProj;
    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLLoadIdentity();
    kmMat4PerspectiveProjection(&matrixProj, 60, (float)m_iWidth/m_iHeight, 0.1f, fFarPane);
    kmGLMultMatrix(&matrixProj);

    TransferMVPMatrix();
}

void CRenderManager::TransferMVPMatrix()
{
    kmMat4 projection;
    kmMat4 viewMatrix;
    kmGLGetMatrix(KM_GL_PROJECTION, &projection);
    kmGLGetMatrix(KM_GL_MODELVIEW, &viewMatrix);
    kmMat4 vpmatrix;
    kmMat4Multiply(&vpmatrix, &projection, &viewMatrix);
    kmMat4 modelmatrix;
    kmMat4Identity(&modelmatrix);

#ifdef USE_UBO
    const GLvoid *matrices[4] = {
        vpmatrix.mat,
        modelmatrix.mat,
        viewMatrix.mat,
        projection.mat,
    };
    GLsizeiptr sizes[4] = {
        sizeof(kmMat4),
        sizeof(kmMat4),
        sizeof(kmMat4),
        sizeof(kmMat4),
    };

    UpdateUBO(UNIFORM_BLOCK_MVP_MATRIX, matrices, sizes, 4);
#else
    GLuint uCurrentShader = CRenderer::GetInstance()->GetCurrentState()->GetCurrentShaderProgram();
    if(uCurrentShader != 0)
    {  
        GLint MVPLocation = CRenderer::GetInstance()->GetUniformLocation(uCurrentShader, COMMON_UNIFORM_NAMES[UNIFORM_MVP_MATRIX]);
        CRenderer::GetInstance()->SetUniformMatrix4fv(MVPLocation, (const float*)vpmatrix.mat, 1);
    }
#endif
}

bool CRenderManager::InitGridData()
{
    bool bRet = true;
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->GenVertexArrays(1, &m_uGridVAO);
    pRenderer->BindVertexArray(m_uGridVAO);

    pRenderer->GenBuffers(1, &m_uGridVBO);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uGridVBO);

    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_POSITION);
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_COLOR);

    pRenderer->VertexAttribPointer(ATTRIB_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE,
        sizeof(CVertexPC), (const GLvoid *)offsetof(CVertexPC, position));
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(CVertexPC), (const GLvoid *)offsetof(CVertexPC, color));

    // draw grid at x-z pane
    std::vector<CVertexPC> linePoints;
    int nLineLength = 100;
    int nHalfLineLength = nLineLength / 2;
    CVertexPC vertex;
    for (int i = -nHalfLineLength; i < nHalfLineLength; ++i)
    {
        vertex.color = i == 0 ? 0xFFFFFFFF : 0x999999FF;
        kmVec3Fill(&vertex.position, (float)-nHalfLineLength, 0, (float)i);
        linePoints.push_back(vertex);
        kmVec3Fill(&vertex.position, (float)nHalfLineLength, 0, (float)i);
        linePoints.push_back(vertex);
        kmVec3Fill(&vertex.position, (float)i, 0, (float)-nHalfLineLength);
        linePoints.push_back(vertex);
        kmVec3Fill(&vertex.position, (float)i, 0, (float)nHalfLineLength);
        linePoints.push_back(vertex);
    }
    m_uGridVertexCount = linePoints.size();
    pRenderer->BufferData(GL_ARRAY_BUFFER, linePoints.size() * sizeof(CVertexPC), linePoints.data(), GL_STATIC_DRAW);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    pRenderer->BindVertexArray(0);
    return bRet;
}

bool CRenderManager::InitLineBuffer()
{
    bool bRet = true;
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->GenVertexArrays(1, &m_uLineVAO);
    pRenderer->BindVertexArray(m_uLineVAO);

    pRenderer->GenBuffers(1, &m_uLineVBO);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uLineVBO);

    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_POSITION);
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_COLOR);

    pRenderer->VertexAttribPointer(ATTRIB_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE,
        sizeof(CVertexPC), (const GLvoid *)offsetof(CVertexPC, position));
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(CVertexPC), (const GLvoid *)offsetof(CVertexPC, color));

    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    pRenderer->BindVertexArray(0);

    FC_CHECK_GL_ERROR_DEBUG();

    return bRet;
}

bool CRenderManager::InitTriangleBuffer()
{
    bool bRet = true;
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->GenVertexArrays(1, &m_uTriangleVAO);
    pRenderer->BindVertexArray(m_uTriangleVAO);

    pRenderer->GenBuffers(1, &m_uTriangleVBO);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uTriangleVBO);

    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_POSITION);
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_COLOR);

    pRenderer->VertexAttribPointer(ATTRIB_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE,
        sizeof(CVertexPC), (const GLvoid *)offsetof(CVertexPC, position));
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(CVertexPC), (const GLvoid *)offsetof(CVertexPC, color));

    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    pRenderer->BindVertexArray(0);

    FC_CHECK_GL_ERROR_DEBUG();

    return bRet;
}

void CRenderManager::RenderGrid()
{
    CRenderer* pRenderer = CRenderer::GetInstance();

    pRenderer->BindTexture(GL_TEXTURE_2D, 0);
    static SharePtr<CShaderProgram> shader(
        CResourceManager::GetInstance()->GetResource<CShaderProgram>(
        _T("..\\SourceCode\\Shader\\vs.txt@..\\SourceCode\\Shader\\fs.txt"), false));
    pRenderer->UseProgram(shader->ID());

    FC_CHECK_GL_ERROR_DEBUG();
    pRenderer->BindVertexArray(m_uGridVAO);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uGridVBO);

    pRenderer->LineWidth(2.0f);

    pRenderer->DrawArrays(GL_LINES, 0, m_uGridVertexCount);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    pRenderer->BindVertexArray(0);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderManager::RenderLineImpl()
{
    if (m_renderLines.size() > 0)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        pRenderer->DisableGL(GL_DEPTH_TEST);
        pRenderer->BindVertexArray(m_uLineVAO);
        pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uLineVBO);
        pRenderer->BufferData(GL_ARRAY_BUFFER, m_renderLines.size() * sizeof(CVertexPC), m_renderLines.data(), GL_DYNAMIC_DRAW);
        pRenderer->DrawArrays(GL_LINES, 0, m_renderLines.size());
        pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
        pRenderer->BindVertexArray(0);
        FC_CHECK_GL_ERROR_DEBUG();
        m_renderLines.clear();
    }
}

void CRenderManager::RenderTriangleImpl()
{
    if (m_renderTriangles.size() > 0)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        pRenderer->DisableGL(GL_DEPTH_TEST);
        pRenderer->BindVertexArray(m_uTriangleVAO);
        pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uTriangleVBO);
        pRenderer->BufferData(GL_ARRAY_BUFFER, m_renderTriangles.size() * sizeof(CVertexPC), m_renderTriangles.data(), GL_DYNAMIC_DRAW);
        pRenderer->DrawArrays(GL_TRIANGLES, 0, m_renderTriangles.size());
        pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
        pRenderer->BindVertexArray(0);
        FC_CHECK_GL_ERROR_DEBUG();
        m_renderTriangles.clear();
    }
}

void CRenderManager::RenderCoordinate(const kmMat4* pMatrix)
{
    static const size_t X_AIXS_COLOR = 0xFF0000FF;
    static const size_t Y_AIXS_COLOR = 0x00FF00FF;
    static const size_t Z_AIXS_COLOR = 0x0000FFFF;
    kmVec3 upVec, rightVec, forwardVec;

    kmMat4GetUpVec3(&upVec, pMatrix);
    kmMat4GetRightVec3(&rightVec, pMatrix);
    kmMat4GetForwardVec3(&forwardVec, pMatrix);

    float fPosW = (*pMatrix).mat[15];
    kmVec3 pos;
    kmVec3Fill(&pos,(*pMatrix).mat[12] / fPosW, (*pMatrix).mat[13] / fPosW, (*pMatrix).mat[14] / fPosW);

    kmVec3Add(&rightVec,&rightVec,&pos);
    kmVec3Add(&upVec,&upVec,&pos);
    kmVec3Add(&forwardVec,&forwardVec,&pos);

    CVertexPC originPos, xPos, yPos, zPos;
    originPos.position.x = pos.x;
    originPos.position.y = pos.y;
    originPos.position.z = pos.z;
    originPos.color = X_AIXS_COLOR;

    xPos.position.x = rightVec.x;
    xPos.position.y = rightVec.y;
    xPos.position.z = rightVec.z;
    xPos.color = X_AIXS_COLOR;
    RenderLine(originPos,xPos);

    yPos.position.x = upVec.x;
    yPos.position.y = upVec.y;
    yPos.position.z = upVec.z;
    yPos.color = Y_AIXS_COLOR;
    originPos.color = Y_AIXS_COLOR;
    RenderLine(originPos,yPos);

    zPos.position.x = forwardVec.x;
    zPos.position.y = forwardVec.y;
    zPos.position.z = forwardVec.z;
    zPos.color = Z_AIXS_COLOR;
    originPos.color = Z_AIXS_COLOR;
    RenderLine(originPos,zPos);
}

void CRenderManager::RenderLine(const CVertexPC& start,const CVertexPC& end)
{
    m_renderLines.push_back(start);
    m_renderLines.push_back(end);
}

void CRenderManager::RenderTriangle( const CVertexPC& pt1, const CVertexPC& pt2, const CVertexPC& pt3 )
{
    m_renderTriangles.push_back(pt1);
    m_renderTriangles.push_back(pt2);
    m_renderTriangles.push_back(pt3);
}

