#ifndef RENDER_TEXTURE_FRAG_H__INCLUDE
#define RENDER_TEXTURE_FRAG_H__INCLUDE

#include "Geometry.h"
#include "CommonTypes.h"

class CTexture;

class CTextureFrag
{
public:
	CTextureFrag(const TString &name, SharePtr<CTexture> texture, kmVec2 origin, kmVec2 size);

    TString Name() const;

	const CQuadT &Quad() const;

	SharePtr<CTexture> Texture() const;

    kmVec2 Origin() const;

    kmVec2 Size() const;

private:
    TString m_name;
	CQuadT m_quad;
	SharePtr<CTexture> m_texture;
	kmVec2 m_origin;
	kmVec2 m_size;
};

#endif