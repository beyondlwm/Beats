#ifndef GUI_RENDERER_RENDERER_FACTORY_H__INCLUDE
#define GUI_RENDERER_RENDERER_FACTORY_H__INCLUDE

namespace FCGUI
{
    class BaseRenderer;

    class RendererFactory
    {
    public:
        virtual ~RendererFactory(){}
        virtual BaseRenderer *Create() = 0;
    };

    template <typename T>
    class TplRendererFactory : public RendererFactory
    {
    public:
        virtual BaseRenderer *Create() override
        {
            return new T;
        }
    };
}

#endif