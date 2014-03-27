#ifndef RENDER_TEXTURE_FRAG_H__INCLUDE
#define RENDER_TEXTURE_FRAG_H__INCLUDE

#include "Geometry.h"
#include "CommonTypes.h"

class CTexture;

class CTextureFrag
{
public:
	CTextureFrag(SharePtr<CTexture> texture, CPoint origin, CSize size);

	const CQuadT &Quad() const;

	SharePtr<CTexture> Texture() const;

private:
	CQuadT m_quad;
	SharePtr<CTexture> m_texture;
	CPoint m_origin;
	CSize m_size;
};

#endif