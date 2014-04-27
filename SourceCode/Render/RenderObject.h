#ifndef RENDER_RENDEROBJECT_H__INCLUDE
#define RENDER_RENDEROBJECT_H__INCLUDE

class CRenderObject
{
public:
    CRenderObject();
    virtual ~CRenderObject();

    virtual void PreRender() = 0;
    virtual void Render() = 0;
    virtual void PostRender() = 0;
};

#endif