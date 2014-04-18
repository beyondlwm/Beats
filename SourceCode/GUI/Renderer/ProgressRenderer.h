#ifndef GUI_RENDERER_PROGRESS_RENDERER_H__INCLUDE
#define GUI_RENDERER_PROGRESS_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
#include "GUI/Window/Progress.h"

class CTextureFrag;

namespace FCGUI
{
    class  ProgressRenderer : public BaseRenderer
    {
        DEFINE_RENDERER_TYPE(RENDERER_PROGRESS)
    public:
        ProgressRenderer();

        void  AddLayer(const TString &fillTextName, const TString &backTextName, Progress::State state);
        void  AddLayer(CTextureFrag* fillLayer, CTextureFrag *backLayer, Progress::State state);
    };
}

#endif