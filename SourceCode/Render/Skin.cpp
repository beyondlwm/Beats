#include "stdafx.h"
#include "Skin.h"
#include "RenderManager.h"
#include "Utility\BeatsUtility\Serializer.h"
#include "Renderer.h"

CSkin::CSkin()
    :     m_pSkeleton(NULL)
    ,    m_vertices(nullptr)
    ,    m_uVAO(0)
    ,   m_uVertexCount(0)
{
    memset(m_uVBO, 0, sizeof(m_uVBO));
}

CSkin::~CSkin()
{
    BEATS_SAFE_DELETE_ARRAY(m_vertices);
}

bool CSkin::Load()
{
    bool bRet = false;
    BEATS_ASSERT(!IsLoaded(), _T("Can't Load a skin which is already loaded!"));

    // Load From File
    CSerializer serializer(_T("..\\Resource\\skin\\org.skin"));
    CSerializer tmpVerticesBufferPos, tmpVerticesBufferUV;
    CSerializer indexBuffer;
    size_t uVertexCount = 0;
    serializer >> uVertexCount;
    m_uVertexCount = uVertexCount;
    m_vertices = new CVertexPTB[uVertexCount];
    float x, y, z;
    float u,v;

    for (size_t i = 0; i < uVertexCount; ++i)
    {
        ESkeletonBoneType bone, bone1, bone2, bone3;
        float weight, weight1, weight2,weight3;
        serializer >> x >> y >> z >> u >> v;
        serializer >> bone;
        serializer >> weight;
        serializer >> bone1;
        serializer >> weight1;
        serializer >> bone2;
        serializer >> weight2;
        serializer >> bone3;
        serializer >> weight3;

        CVertexPTB &vertex = m_vertices[i];
        kmVec3Fill(&vertex.position,x,y,z);
        vertex.tex = CTex(u,v);
        vertex.bones = CIVector4(bone, bone1, bone2, bone3);
        kmVec4Fill(&vertex.weights,weight,weight1,weight2,weight3);

#ifdef _DEBUG
        float sum = weight + weight1 + weight2+weight3;
        BEATS_ASSERT(sum < 1.01F, _T("Weight can't be greater than 1.01F, cur Value : %f!"), sum);
        BEATS_WARNING(sum > 0.99F, _T("Weight can't be smaller than 0.99F, cur Value : %f!"), sum);
#endif
    }
    
    for (size_t i = 0; i < uVertexCount; ++i)
    {
        indexBuffer << (short)i;
    }

    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->GenVertexArrays(1, &m_uVAO);
    pRenderer->GenBuffers(2, m_uVBO);

#ifndef SW_SKEL_ANIM
    buildVBOVertex(m_vertices, m_uVertexCount*sizeof(CVertexPTB));
    BEATS_SAFE_DELETE_ARRAY(m_vertices);
#endif
    buildVBOIndex(indexBuffer.GetBuffer(), indexBuffer.GetWritePos());
    buildVAO();

    SetLoadedFlag(true);

    return bRet;
}

void CSkin::buildVAO()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->BindVertexArray(m_uVAO);

    pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uVBO[0]);

    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_POSITION);
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_TEXCOORD0);
#ifndef SW_SKEL_ANIM
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_BONE_INDICES);
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_WEIGHTS);
#endif

#ifdef SW_SKEL_ANIM
#define VERTEX_TYPE CVertexPT
#else
#define VERTEX_TYPE CVertexPTB
#endif

    pRenderer->VertexAttribPointer(ATTRIB_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE,
        sizeof(VERTEX_TYPE), (const GLvoid *)offsetof(VERTEX_TYPE, position));
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_TEXCOORD0, 2, GL_FLOAT, GL_FALSE,
        sizeof(VERTEX_TYPE), (const GLvoid *)offsetof(VERTEX_TYPE, tex));
#ifndef SW_SKEL_ANIM
    pRenderer->VertexAttribIPointer(ATTRIB_INDEX_BONE_INDICES, 4, GL_INT, 
        sizeof(VERTEX_TYPE), (const GLvoid *)offsetof(VERTEX_TYPE, bones));
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_WEIGHTS, 4, GL_FLOAT, GL_FALSE,
        sizeof(VERTEX_TYPE), (const GLvoid *)offsetof(VERTEX_TYPE, weights));
#endif

#undef VERTEX_TYPE

    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uVBO[1]);

    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    pRenderer->BindVertexArray(0);    //Unbind VAO
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}

void CSkin::buildVBOVertex(const GLvoid *data, GLsizeiptr size)
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_uVBO[0]);
#ifdef SW_SKEL_ANIM
    GLenum usage = GL_DYNAMIC_DRAW;
#else
    GLenum usage = GL_STATIC_DRAW;
#endif
    pRenderer->BufferData(GL_ARRAY_BUFFER, size, data, usage);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}

void CSkin::buildVBOIndex(const GLvoid *data, GLsizeiptr size)
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uVBO[1]);
    pRenderer->BufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}

bool CSkin::Unload()
{
    bool bRet = IsLoaded();
    if (bRet)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        pRenderer->DeleteBuffers(2, m_uVBO);
        pRenderer->DeleteBuffers(1, &m_uVAO);
        SetLoadedFlag(false);
    }
    return bRet;
}

GLuint CSkin::GetVAO() const
{
    return m_uVAO;
}

#ifdef SW_SKEL_ANIM
void CSkin::CalcSkelAnim(const CAnimationController::BoneMatrixMap &matrices)
{
    CVertexPT *vertices = new CVertexPT[m_uVertexCount];
    for(size_t i = 0; i < m_uVertexCount; ++i)
    {
        const CVertexPTB &vertex = m_vertices[i];
        CVertexPT &vertex1 = vertices[i];
        if(matrices.empty())
        {
            vertex1.position = vertex.position;
            vertex1.tex = vertex.tex;
            continue;
        }

        kmVec3  pos;
        kmVec3Fill(&pos,vertex.position.x,vertex.position.y, vertex.position.z);

        kmVec3 finalpos;
        kmMat4 mat, mat2, mat3, mat4;
        if(vertex.bones.x >= 0)
        {
            auto itr = matrices.find(static_cast<ESkeletonBoneType>(vertex.bones.x));
            BEATS_ASSERT(itr != matrices.end());
            mat = itr->second;
            kmVec3 postmp;
            kmVec3Transform(&postmp,&pos,&mat);
            kmVec3Scale(&postmp, &postmp, vertex.weights.x);
            finalpos = postmp;
        }
        if(vertex.bones.y >= 0)
        {
            auto itr = matrices.find(static_cast<ESkeletonBoneType>(vertex.bones.y));
            BEATS_ASSERT(itr != matrices.end());
            mat2 = itr->second;
            kmVec3 postmp;
            kmVec3Transform(&postmp,&pos,&mat2);
            kmVec3Scale(&postmp, &postmp, vertex.weights.y);
            kmVec3Add(&finalpos,&finalpos,&postmp);
        }
        if(vertex.bones.z >= 0)
        {
            auto itr = matrices.find(static_cast<ESkeletonBoneType>(vertex.bones.z));
            BEATS_ASSERT(itr != matrices.end());
            mat3 = itr->second;
            kmVec3 postmp;
            kmVec3Transform(&postmp,&pos,&mat3);
            kmVec3Scale(&postmp, &postmp, vertex.weights.z);
            kmVec3Add(&finalpos,&finalpos,&postmp);
        }
        if(vertex.bones.w >= 0)
        {
            auto itr = matrices.find(static_cast<ESkeletonBoneType>(vertex.bones.w));
            BEATS_ASSERT(itr != matrices.end());
            mat4 = itr->second;
            kmVec3 postmp;
            kmVec3Transform(&postmp,&pos,&mat4);
            kmVec3Scale(&postmp, &postmp, vertex.weights.w);
            kmVec3Add(&finalpos,&finalpos,&postmp);
        }
        vertex1.position = finalpos;
        
        vertex1.tex = vertex.tex;
    }

    buildVBOVertex(vertices, m_uVertexCount*sizeof(CVertexPT));
    BEATS_SAFE_DELETE_ARRAY(vertices);
}
#endif