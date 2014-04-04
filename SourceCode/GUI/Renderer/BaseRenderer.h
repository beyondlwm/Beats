#ifndef GUI_RENDERER_BASE_RENDERER_H__INCLUDE
#define GUI_RENDERER_BASE_RENDERER_H__INCLUDE 

#include "RendererDefs.h" 
#include "Render/CommonTypes.h"

//forward declaration
class CTextureFrag;
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

		virtual void AddLayer(CTextureFrag *layer);

		virtual void AddLayer(const TString &textureFragName);

	protected:
		virtual void setVertices(const Window *window);

		virtual void renderChildren(const kmMat4 &parentTransform) const;
		virtual void renderLayers(const kmMat4 &worldTransform) const;

	private:
		void onWindowEvent(BaseEvent *event);

	protected:
		Window *_window;

        CQuadP _quad;
		std::vector<CTextureFrag *> _layers;
	};
}

#endif