#ifndef RENDER_TEXTURE_FRAG_MANAGER_H__INCLUDE
#define RENDER_TEXTURE_FRAG_MANAGER_H__INCLUDE

class CTextureFrag;
class CTexture;

class CTextureFragManager
{
    BEATS_DECLARE_SINGLETON(CTextureFragManager);
public:
    CTextureFrag *CreateTextureFrag(const TString &name, 
        SharePtr<CTexture> texture, kmVec2 origin, kmVec2 size);
    CTextureFrag *GetTextureFrag(const TString &name);

private:
    std::map<TString, CTextureFrag *> m_textureFrags;
};

#endif