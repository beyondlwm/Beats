#ifndef GUI_RENDERER_RENDER_LAYER_H__INCLUDE
#define GUI_RENDERER_RENDER_LAYER_H__INCLUDE

#include "GUI/Animation/Animatable.h"

class CTextureFrag;

namespace FCGUI
{
    class RenderLayer : public Animatable
    {
    public:
        typedef std::vector<CTextureFrag *> FrameList;

        enum Property
        {
            PROP_CURR_FRAME,
        };

        RenderLayer(const TString &textureFragName);
        RenderLayer(CTextureFrag *textureFrag);
        RenderLayer(const FrameList &frames);
        ~RenderLayer();

        void SetCurrFrame(size_t currFrame);
        size_t CurrFrame() const;
        CTextureFrag *GetTextureFrag() const;

    private:
        void initAnimProp();

    private:
        FrameList _frames;
        size_t _currFrame;
    };
}

#endif // !GUI_RENDERER_RENDER_LAYER_H__INCLUDE
