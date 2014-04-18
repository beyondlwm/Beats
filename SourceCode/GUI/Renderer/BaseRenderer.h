#ifndef GUI_RENDERER_BASE_RENDERER_H__INCLUDE
#define GUI_RENDERER_BASE_RENDERER_H__INCLUDE 

#include "RendererDefs.h" 
#include "Render/CommonTypes.h"
#include "RenderLayer.h"

//forward declaration
class CTextureFrag;
class BaseEvent;

namespace FCGUI
{
	//forward declaration
	class Window;
    class RenderLayer;

	//rendering children and layers
	class BaseRenderer
	{
        DEFINE_RENDERER_TYPE(RENDERER_BASE);
	public:
		BaseRenderer();
		virtual ~BaseRenderer();

        void SetWindow(Window *window);

		virtual void Render(const kmMat4 &parentTransform) const;

		void AddLayer(const TString &textureFragName, unsigned int state = 0);
        void AddLayer(CTextureFrag *textureFrag, unsigned int state = 0);
        void AddLayer(const RenderLayer::FrameList &frames, unsigned int state = 0);

        RenderLayer *GetLayer(size_t index, unsigned int state = 0);

	protected:
		virtual void setVertices(const Window *window);

		virtual void renderChildren(const kmMat4 &parentTransform) const;
		virtual void renderLayers(const kmMat4 &worldTransform) const;

	private:
		void onWindowEvent(BaseEvent *event);

	protected:
        typedef std::vector<RenderLayer *> LayerList;
        typedef std::map<unsigned int, LayerList> StateLayerList;

		Window *_window;

        CQuadP _quad;
        StateLayerList _stateLayers;
	};
}

#endif