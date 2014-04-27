#ifndef GUI_RENDERER_RENDERER_H__INCLUDE
#define GUI_RENDERER_RENDERER_H__INCLUDE 

#include "RendererDefs.h" 

//forward declaration
class CSpriteFrame;
class EventBase;

namespace FCGUI
{
     //forward declaration
     class Window;

     //rendering children and layers
     class RendererBase
     {
     public:
          RendererBase();
          virtual ~RendererBase();

        void SetWindow(Window *window);

          virtual RendererType Type() const;

          virtual void Render(const kmMat4 &parentTransform) const;

          virtual void AddLayer(CSpriteFrame *layer);

     protected:
          virtual void calcTransform(const Window *window);
          virtual void setVertices(const Window *window);

          virtual void renderChildren(const kmMat4 &parentTransform) const;

     private:
          void onWindowEvent(EventBase *event);

     protected:
          Window *_window;
          kmMat4 _transform;

          std::vector<CSpriteFrame *> _layers;
     };
}

#endif