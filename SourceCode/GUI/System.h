#ifndef GUI_SYSTEM_H__INCLUDE
#define GUI_SYSTEM_H__INCLUDE

#include "PublicDef.h"

//forward declaration
class MouseEvent;
class KeyboardEvent;

namespace FCGUI
{
	//forward declaration
	class Window;

	class System
	{
		BEATS_DECLARE_SINGLETON(System);
	public:
		void Update(float deltaTime);
		void Render();

        bool InjectMouseEvent(MouseEvent *event);
        bool InjectKeyboardEvent(KeyboardEvent *event);

		void OnResolutionChanged(kmVec2 resolution);
		kmVec2 GetResolution() const;

	private:
		void preRender();
		void postRender();

	private:
		kmVec2 _resolution;
	};
}

#endif