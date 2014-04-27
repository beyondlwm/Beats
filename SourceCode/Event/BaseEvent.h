#ifndef EVENT_BASE_EVENT_H__INCLUDE
#define EVENT_BASE_EVENT_H__INCLUDE

class EventDispatcher;

class BaseEvent
{
public:
    BaseEvent(int type)
        : _type(type)
        , _stopPropagation(false)
        , _source(nullptr)
    {}

    virtual ~BaseEvent(){}

    int Type() const
    {
        return _type;
    }

    void StopPropagation()
    {
        _stopPropagation = true;
    }

    bool Stopped() const
    {
        return _stopPropagation;
    }

    void SetSource(EventDispatcher *source)
    {
        _source = source;
    }

    EventDispatcher *Source() const
    {
        return _source;
    }

protected:
    int _type;
    bool _stopPropagation;
    EventDispatcher *_source;
};

#endif