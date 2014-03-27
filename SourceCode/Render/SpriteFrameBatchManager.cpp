#include "stdafx.h"
#include "SpriteFrameBatchManager.h"
#include "SpriteFrameBatch.h"
#include "RenderManager.h"
#include "SpriteFrame.h"
#include "Texture.h"
#include "renderer.h"
#include "Resource/ResourceManager.h"
#include "ShaderProgram.h"

CSpriteFrameBatchManager *CSpriteFrameBatchManager::m_pInstance = nullptr;

CSpriteFrameBatchManager::CSpriteFrameBatchManager()
{

}

CSpriteFrameBatchManager::~CSpriteFrameBatchManager()
{
    for(auto batch : m_batches)
    {
        delete batch.second;
    }
}

void CSpriteFrameBatchManager::Render()
{
    if(!m_program)
    {
        m_program = CResourceManager::GetInstance()->GetResource<CShaderProgram>(
            _T("..\\SourceCode\\Shader\\vs_pt.txt@..\\SourceCode\\Shader\\fs_t.txt"), false);
    }
    CRenderer::GetInstance()->UseProgram(m_program->ID());
    CRenderManager::GetInstance()->SetupVPMatrix(true);
    CRenderer::GetInstance()->DisableGL(GL_DEPTH_TEST);
    CRenderer::GetInstance()->EnableGL(GL_BLEND);
    CRenderer::GetInstance()->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for(auto batch : m_batches)
    {
        batch.second->PreRender();
        batch.second->Render();
        batch.second->PostRender();
    }
    CRenderer::GetInstance()->EnableGL(GL_DEPTH_TEST);
    CRenderer::GetInstance()->DisableGL(GL_BLEND);
}

void CSpriteFrameBatchManager::AddSpriteFrame( CSpriteFrame *frame, const kmMat4 &transform )
{
    if(!frame)
        return;

    GLuint textureID = frame->Texture()->ID();
    auto itr = m_batches.find(textureID);
    if(itr == m_batches.end())
    {
        CSpriteFrameBatch *batch = new CSpriteFrameBatch;
        m_batches[textureID] = batch;
        batch->AddSpriteFrame(frame, transform);
    }
    else
    {
        itr->second->AddSpriteFrame(frame, transform);
    }
}

void CSpriteFrameBatchManager::Clear()
{
    for(auto batch : m_batches)
    {
        batch.second->Clear();
    }
}
