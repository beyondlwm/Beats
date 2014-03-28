#ifndef GUI_WINDOW_WINDOW_H__INCLUDE
#define GUI_WINDOW_WINDOW_H__INCLUDE

#include "Event\EventDispatcher.h"
#include "GUI\GUIEventType.h"

//forward declaration
class MouseEvent;

namespace FCGUI
{
	//forward declaration
	class BehaviorBase;
	class BaseRenderer;
    class BaseLayout;

	class Window : public EventDispatcher
	{
		//type definition
	public:
		typedef std::map<TString, Window *> WindowListType;

		//event definition
	public:
		enum EventType
		{
			EVENT_WINDOW = FCGUI::EVENT_WINDOW,
			EVENT_MOVED,
			EVENT_SIZED,
			EVENT_ROTATED,
			EVENT_SCALED,
			EVENT_ANCHOR_CHANGED,
		};

	public:
        Window(const TString &name);
		virtual ~Window();

		void Init();
        void SetPosSize(kmScalar x, kmScalar y, kmScalar xPercent, kmScalar yPercent,
            kmScalar width, kmScalar height, kmScalar widthPercent, kmScalar heightPercent);
        void SetArea(kmScalar left, kmScalar top, kmScalar leftPercent, kmScalar topPercent,
            kmScalar right, kmScalar bottom, kmScalar rightPercent, kmScalar bottomPercent);

        //component
        void SetBehavior(BehaviorBase *behavior);
		BehaviorBase *Behavior() const;
        void SetRenderer(BaseRenderer *renderer);
		BaseRenderer *Renderer() const;
        void SetLayout(BaseLayout *layout);
        BaseLayout *Layout() const;

        //attribute
		void SetName(const TString &name);
		TString Name() const;
        void SetPos(kmScalar x, kmScalar y);
		void SetPos(kmVec2 pos);
		kmVec2 Pos() const;
        void SetPosPercent(kmScalar x, kmScalar y);
		void SetPosPercent(kmVec2 posPercent);
		kmVec2 PosPercent() const;
		kmVec2 RealPos() const;	//calculated from pos, pospercent, and parentsize
        void SetSize(kmScalar width, kmScalar height);
		void SetSize(kmVec2 size);
		kmVec2 Size() const;
        void SetSizePercent(kmScalar width, kmScalar height);
		void SetSizePercent(kmVec2 sizePercent);
		kmVec2 SizePercent() const;
		kmVec2 RealSize() const;	//calculated from size, sizepercent, and parentsize
        void SetAnchor(kmScalar x, kmScalar y);
		void SetAnchor(kmVec2 anchor);
		kmVec2 Anchor() const;
        kmVec2 RealAnchor() const;
		void SetRotation(kmScalar rotation);
		kmScalar Rotation() const;
        void SetScale(kmScalar scaleX, kmScalar scaleY);
		void SetScale(kmVec2 scale);
		kmVec2 Scale() const;
        const kmMat4 &Transform() const;
        kmMat4 WorldTransform() const;
        void Localize(kmVec3 &pos) const;
        bool HitTest(kmScalar x, kmScalar y) const;

		void Show();
		void Hide();
		void ToggleVisible();
		bool Visible();

        //event
		void OnParentSized();
        virtual bool OnMouseEvent(MouseEvent *event);
        virtual void Update(float deltaTime);

        //hierachy
		void SetParent(Window *parent);
		void AddChild(Window *window);
		void RemoveChild(Window *window);
		void RemoveChild(const TString &name);
		void DestroyChild(Window *window);
		void DestroyChild(const TString &name);
		Window *GetChild(const TString &name, bool recursively = true) const;
		template <typename Func>
		const Func &Traverse(const Func &func);

	protected:
		void calcRealPos();
		void calcRealSize();
		void calcTransform();

	protected:
		//attribute
		TString _name;
		kmVec2 _pos;
		kmVec2 _posPercent;
		kmVec2 _realPos;
		kmVec2 _size;
		kmVec2 _sizePercent;
		kmVec2 _realSize;
		kmVec2 _anchor;
		kmScalar _rotation;
		kmVec2 _scale;
		bool _visible;
		kmMat4 _transform;  //transform in parent space

		//component
        BaseLayout *_layout;
		BehaviorBase *_behavior;
		BaseRenderer *_renderer;

		//window hierachy
		Window *_parent;
		WindowListType _children;
	};

	template <typename Func>
	const Func &Window::Traverse( const Func &func )
	{
		for(auto child : _children)
		{
			func(child.second);
		}
		return func;
	}

}

#endif