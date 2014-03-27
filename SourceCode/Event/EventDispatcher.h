#ifndef EVENT_EVENT_DISPATCHER_H__INCLUDE
#define EVENT_EVENT_DISPATCHER_H__INCLUDE

//forward declaration
class EventBase;

class EventDispatcher
{
    //type definition
public:
    typedef std::function<void(EventBase *)> EventHandler;
    typedef std::vector<EventHandler> EventHandlerList;
    typedef std::map<int, EventHandlerList> EventHandlerMap;

public:
    EventDispatcher();
    virtual ~EventDispatcher();

    virtual void SubscribeEvent(int type, EventHandler handler);
    virtual void DispatchEvent(EventBase *event) const;

protected:
    EventHandlerMap _handlers;
};

#endif