#include "stdafx.h"
#include "SpriteFrameBatchManager.h"
#include "SpriteFrameBatch.h"
#include "RenderManager.h"
#include "SpriteFrame.h"
#include "Texture.h"
#include "renderer.h"
#include "Resource/ResourceManager.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "TextureFrag.h"

CSpriteFrameBatchManager *CSpriteFrameBatchManager::m_pInstance = nullptr;

CSpriteFrameBatchManager::CSpriteFrameBatchManager()
{
    SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("PointTexShader.vs"), false);
    SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("PointTexShader.ps"), false);
    BEATS_ASSERT(pVS && pPS, _T("Load Shader Failed!"));
    m_pProgram = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
}

CSpriteFrameBatchManager::~CSpriteFrameBatchManager()
{
    for(auto batch : m_batches)
    {
        BEATS_ASSERT(batch.second);
    }
}

void CSpriteFrameBatchManager::Render()
{
    CRenderer::GetInstance()->UseProgram(m_pProgram->ID());
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

void CSpriteFrameBatchManager::AddQuad(const CQuadP &quad, CTextureFrag *frag, const kmMat4 &transform)
{
    if(frag)
	{
	    GLuint textureID = frag->Texture()->ID();
	    auto itr = m_batches.find(textureID);
		CSpriteFrameBatch *batch = nullptr;
	    if(itr == m_batches.end())
	    {
	        batch = new CSpriteFrameBatch;
	        m_batches[textureID] = batch;
	    }
	    else
	    {
	        batch = itr->second;
	    }
		batch->AddQuad(quad, frag, transform);
	}
}

void CSpriteFrameBatchManager::AddQuad( const CQuadPT &quad, SharePtr<CTexture> texture )
{
    if(texture)
	{
	    GLuint textureID = texture->ID();
	    auto itr = m_batches.find(textureID);
		CSpriteFrameBatch *batch = nullptr;
	    if(itr == m_batches.end())
	    {
	        batch = new CSpriteFrameBatch;
	        m_batches[textureID] = batch;
	    }
	    else
	    {
	        batch = itr->second;
	    }
		batch->AddQuad(quad, texture);
	}
}

void CSpriteFrameBatchManager::Clear()
{
    for(auto batch : m_batches)
    {
        batch.second->Clear();
    }
}
