#ifndef EVENT_EVENT_DISPATCHER_H__INCLUDE
#define EVENT_EVENT_DISPATCHER_H__INCLUDE

//forward declaration
class BaseEvent;

class EventDispatcher
{
	//type definition
public:
	typedef std::function<void(BaseEvent *)> EventHandler;
	typedef std::vector<EventHandler> EventHandlerList;
	typedef std::map<int, EventHandlerList> EventHandlerMap;

public:
	EventDispatcher();
	virtual ~EventDispatcher();

	virtual void SubscribeEvent(int type, const EventHandler &handler);
    template <typename MemberFunc, typename ObjectType>
    void SubscribeEvent(int type, const MemberFunc &func, ObjectType *obj);
	virtual void DispatchEvent(BaseEvent *event);

protected:
	EventHandlerMap _handlers;
};

template <typename MemberFunc, typename ObjectType>
void EventDispatcher::SubscribeEvent( int type, const MemberFunc &func, ObjectType *obj )
{
    SubscribeEvent(type, std::bind(func, obj, std::placeholders::_1));
}

#endif