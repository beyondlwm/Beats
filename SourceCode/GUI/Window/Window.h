#ifndef GUI_WINDOW_WINDOW_H__INCLUDE
#define GUI_WINDOW_WINDOW_H__INCLUDE

#include "Event\EventDispatcher.h"
#include "GUI\GUIEventType.h"

namespace FCGUI
{
    //forward declaration
    class BehaviorBase;
    class RendererBase;

    class Window : public EventDispatcher
    {
        //type definition
    public:
        typedef std::map<TString, Window *> WindowListType;

        //event definition
    public:
        enum WindowEventType
        {
            EVENT_GUI_WINDOW = ::EVENT_GUI_WINDOW,
            EVENT_GUI_WINDOW_MOVED,
            EVENT_GUI_WINDOW_SIZED,
            EVENT_GUI_WINDOW_ROTATED,
            EVENT_GUI_WINDOW_SCALED,
            EVENT_GUI_WINDOW_ANCHOR_CHANGED,
        };

    public:
        Window();
        Window(const TString &name);
        Window(const TString &name, kmScalar x, kmScalar y, kmScalar xPercent, kmScalar yPercent,
            kmScalar width, kmScalar height, kmScalar widthPercent, kmScalar heightPercent);
        virtual ~Window();

        void Init();

        //component
        void SetBehavior(BehaviorBase *behavior);
        BehaviorBase *Behavior() const;
        void SetRenderer(RendererBase *renderer);
        RendererBase *Renderer() const;

        //attribute
        void SetName(const TString &name);
        TString Name() const;
        void SetPos(kmScalar x, kmScalar y);
        void SetPos(kmVec2 pos);
        kmVec2 Pos() const;
        void SetPosPercent(kmScalar x, kmScalar y);
        void SetPosPercent(kmVec2 posPercent);
        kmVec2 PosPercent() const;
        kmVec2 RealPos() const;    //calculated from pos, pospercent, and parentsize
        void SetSize(kmScalar width, kmScalar height);
        void SetSize(kmVec2 size);
        kmVec2 Size() const;
        void SetSizePercent(kmScalar width, kmScalar height);
        void SetSizePercent(kmVec2 sizePercent);
        kmVec2 SizePercent() const;
        kmVec2 RealSize() const;    //calculated from size, sizepercent, and parentsize
        void SetArea(kmScalar left, kmScalar top, kmScalar leftPercent, kmScalar topPercent,
            kmScalar right, kmScalar rightPercent, kmScalar bottom, kmScalar bottomPercent);
        void SetAnchor(kmScalar x, kmScalar y);
        void SetAnchor(kmVec2 anchor);
        kmVec2 Anchor() const;
        void SetRotate(kmScalar rotate);
        kmScalar Rotate() const;
        void SetScale(kmScalar scaleX, kmScalar scaleY);
        void SetScale(kmVec2 scale);
        kmVec2 Scale() const;
        void Show();
        void Hide();
        void ToggleVisible();
        bool Visible();

        void OnParentSized();

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

    protected:
        //component
        BehaviorBase *_behavior;
        RendererBase *_renderer;

        //attribute
        TString _name;
        kmVec2 _pos;
        kmVec2 _posPercent;
        kmVec2 _realPos;
        kmVec2 _size;
        kmVec2 _sizePercent;
        kmVec2 _realSize;
        kmVec2 _anchor;
        kmScalar _rotate;
        kmVec2 _scale;
        bool _visible;

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