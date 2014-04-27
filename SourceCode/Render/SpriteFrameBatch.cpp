#include "stdafx.h"
#include "SpriteFrameBatch.h"
#include "RenderManager.h"
#include "Texture.h"
#include "Renderer.h"
#include "TextureFrag.h"

CSpriteFrameBatch::CSpriteFrameBatch()
    : m_texture(0)
    , m_count(0)
    , m_VAO(0)
{
    memset(m_quads, 0, sizeof(m_quads));
    CRenderer::GetInstance()->GenVertexArrays(1, &m_VAO);
    CRenderer::GetInstance()->GenBuffers(2, m_VBO);
    setupVAO();
}

CSpriteFrameBatch::~CSpriteFrameBatch()
{
    CRenderer::GetInstance()->DeleteVertexArrays(1, &m_VAO);
    CRenderer::GetInstance()->DeleteBuffers(2, m_VBO);
}

void CSpriteFrameBatch::PreRender()
{
    if(m_count > 0)
    {
        updateVBO();
        CRenderer::GetInstance()->BindVertexArray(m_VAO);
        CRenderManager::GetInstance()->ApplyTexture(0, m_texture);
    }
}

void CSpriteFrameBatch::Render()
{
    if(m_count > 0)
    {
        CRenderer::GetInstance()->DrawElements(
            GL_TRIANGLES, m_count*6, GL_UNSIGNED_SHORT, 0);
    }
}

void CSpriteFrameBatch::PostRender()
{
    if(m_count > 0)
    {
        CRenderManager::GetInstance()->ApplyTexture(0, 0);
        CRenderer::GetInstance()->BindVertexArray(0);
    }
}

void CSpriteFrameBatch::AddQuad(const CQuadP &quad, CTextureFrag *frag, const kmMat4 &transform)
{
    BEATS_ASSERT(m_texture == 0 || frag->Texture()->ID() == m_texture,
        _T("The texture ID of texture frag in a batch must be same."));
    if(m_texture == 0)
    {
        m_texture = frag->Texture()->ID();
    }
    CQuadPT &quadPT = m_quads[m_count];
    kmVec3Transform(&quadPT.tl.position, &quad.tl, &transform);
    kmVec3Transform(&quadPT.tr.position, &quad.tr, &transform);
    kmVec3Transform(&quadPT.bl.position, &quad.bl, &transform);
    kmVec3Transform(&quadPT.br.position, &quad.br, &transform);
    quadPT.tl.tex = frag->Quad().tl;
    quadPT.tr.tex = frag->Quad().tr;
    quadPT.bl.tex = frag->Quad().bl;
    quadPT.br.tex = frag->Quad().br;
    ++m_count;
}

void CSpriteFrameBatch::AddQuad( const CQuadPT &quad, SharePtr<CTexture> texture )
{
    BEATS_ASSERT(m_texture == 0 || texture->ID() == m_texture,
        _T("The texture ID of texture frag in a batch must be same."));
    if(m_texture == 0)
    {
        m_texture = texture->ID();
    }
    m_quads[m_count++] = quad;
}

void CSpriteFrameBatch::setupVAO()
{
    CRenderer *pRenderer = CRenderer::GetInstance();
    pRenderer->BindVertexArray(m_VAO);

    pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);

    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_POSITION);
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_TEXCOORD0);
    
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_POSITION, 
        3, GL_FLOAT, GL_FALSE, sizeof(CVertexPT), (const GLvoid *)offsetof(CVertexPT, position));
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_TEXCOORD0,
        2, GL_FLOAT, GL_FALSE, sizeof(CVertexPT), (const GLvoid *)offsetof(CVertexPT, tex));

    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[1]);

    pRenderer->BindVertexArray(0);
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
}

void CSpriteFrameBatch::updateVBO()
{
    CRenderer *pRenderer = CRenderer::GetInstance();

    pRenderer->BindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
    pRenderer->BufferData(GL_ARRAY_BUFFER, m_count*sizeof(CQuadPT), m_quads, GL_DYNAMIC_DRAW);

    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[1]);
    pRenderer->BufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * 12, nullptr, GL_DYNAMIC_DRAW);
    GLushort *pIndex = (GLushort *)pRenderer->MapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    GLushort *pIndexWritter = pIndex;
    for(size_t i = 0; i < m_count; ++i)
    {
        GLushort start = static_cast<GLushort>(i*4);
        *(pIndexWritter++) = start;
        *(pIndexWritter++) = start + 1;
        *(pIndexWritter++) = start + 2;
        *(pIndexWritter++) = start + 3;
        *(pIndexWritter++) = start + 2;
        *(pIndexWritter++) = start + 1;
    }
    pRenderer->UnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void CSpriteFrameBatch::Clear()
{
    m_texture = 0;
    m_count = 0;
    memset(m_quads, 0, sizeof(m_quads));
}
