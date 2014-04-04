#ifndef GUI_WINDOW_WINDOW_H__INCLUDE
#define GUI_WINDOW_WINDOW_H__INCLUDE

#include "Event\EventDispatcher.h"
#include "GUI\GUIEventType.h"
#include "Utility\BeatsUtility\ComponentSystem\Component\ComponentBase.h"

//forward declaration
class MouseEvent;

namespace FCGUI
{
	//forward declaration
	class BaseRenderer;
    class BaseLayout;

	class Window : public EventDispatcher, public CComponentBase
	{
		typedef CComponentBase super;
        DECLARE_REFLECT_GUID(Window, 0xB4ADE61C, CComponentBase)
		//type definition
	public:
		typedef std::map<TString, Window *> WindowListType;
        typedef std::function<bool(Window *)> WindowVisitor;

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

            EVENT_CHILD_ADDED,
            EVENT_CHILD_REMOVE,
		};

        enum HitTestResult
        {
            HIT_NONE,
            HIT_CONTENT,
            HIT_TOP_LEFT,
            HIT_TOP_RIGHT,
            HIT_BOTTOM_LEFT,
            HIT_BOTTOM_RIGHT,
            HIT_ANCHOR,
            HIT_ROTATION_HANDLE,
        };

	public:
        Window();
        Window(const TString &name);
        Window(CSerializer& serializer);
		virtual ~Window();

		void Init();
        void SetPosSize(kmScalar x, kmScalar y, kmScalar xPercent, kmScalar yPercent,
            kmScalar width, kmScalar height, kmScalar widthPercent, kmScalar heightPercent);
        void SetArea(kmScalar left, kmScalar top, kmScalar leftPercent, kmScalar topPercent,
            kmScalar right, kmScalar bottom, kmScalar rightPercent, kmScalar bottomPercent);

        //component
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
        void SetPercentPos(kmScalar x, kmScalar y);
		void SetPercentPos(kmVec2 posPercent);
		kmVec2 PercentPos() const;
		kmVec2 RealPos() const;	//calculated from pos, pospercent, and parentsize
        void SetSize(kmScalar width, kmScalar height);
		void SetSize(kmVec2 size);
		kmVec2 Size() const;
        void SetPercentSize(kmScalar width, kmScalar height);
		void SetPercentSize(kmVec2 sizePercent);
		kmVec2 PercentSize() const;
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
        void Localize(kmScalar &x, kmScalar &y) const;
        HitTestResult HitTest(kmScalar x, kmScalar y, bool localized = false) const;

		void Show();
		void Hide();
		void ToggleVisible();
		bool Visible();

        //event
        virtual bool OnMouseEvent(MouseEvent *event);
        virtual void Update(float deltaTime);

        //hierachy
		void SetParent(Window *parent);
        Window *Parent() const;
		void AddChild(Window *window);
		void RemoveChild(Window *window);
		void RemoveChild(const TString &name);
		void DestroyChild(Window *window, bool delay = false);
		void DestroyChild(const TString &name, bool delay = false);
		Window *GetChild(const TString &name, bool recursively = true) const;
		const WindowVisitor &Traverse(const WindowVisitor &visitor);

	protected:
		void onParentSized();
		void calcRealPos();
		void calcRealSize();
		void calcTransform();

	protected:
		//attribute
		TString _name;
		kmVec2 _pos;
		kmVec2 _percentPos;
		kmVec2 _realPos;
		kmVec2 _size;
		kmVec2 _percentSize;
		kmVec2 _realSize;
		kmVec2 _anchor;
		kmScalar _rotation;
		kmVec2 _scale;
		bool _visible;
		kmMat4 _transform;  //transform in parent space

		//component
        BaseLayout *_layout;
		BaseRenderer *_renderer;

		//window hierachy
		Window *_parent;
		WindowListType _children;
		std::vector<Window *> _recycled;
	};
}

#endif