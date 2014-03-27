#ifndef EVENT_EVENT_BASE_H__INCLUDE
#define EVENT_EVENT_BASE_H__INCLUDE

class EventDispatcher;

class EventBase
{
public:
    EventBase(int type)
        : _type(type)
    {}

    virtual ~EventBase(){}

    int Type() const
    {
        return _type;
    }

protected:
    int _type;
};

#endif