#ifndef GUI_RENDERER_BASE_RENDERER_H__INCLUDE
#define GUI_RENDERER_BASE_RENDERER_H__INCLUDE 

#include "RendererDefs.h" 

//forward declaration
class CSpriteFrame;
class BaseEvent;

namespace FCGUI
{
	//forward declaration
	class Window;

	//rendering children and layers
	class BaseRenderer
	{
	public:
		BaseRenderer();
		virtual ~BaseRenderer();

        void SetWindow(Window *window);

		virtual RendererType Type() const;

		virtual void Render(const kmMat4 &parentTransform) const;

		virtual void AddLayer(CSpriteFrame *layer);

	protected:
		virtual void setVertices(const Window *window);

		virtual void renderChildren(const kmMat4 &parentTransform) const;
		virtual void renderLayers(const kmMat4 &worldTransform) const;

	private:
		void onWindowEvent(BaseEvent *event);

	protected:
		Window *_window;

		std::vector<CSpriteFrame *> _layers;
	};
}

#endif