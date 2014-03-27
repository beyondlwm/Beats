#include "stdafx.h"
#include "EventDispatcher.h"
#include "EventBase.h"

EventDispatcher::EventDispatcher()
{

}

EventDispatcher::~EventDispatcher()
{

}

void EventDispatcher::SubscribeEvent( int type, EventHandler handler )
{
    EventHandlerList &handlerList = _handlers[type];
    handlerList.push_back(handler);
}

void EventDispatcher::DispatchEvent( EventBase *event ) const
{
    auto itr = _handlers.find(event->Type());
    if(itr != _handlers.end())
    {
        const EventHandlerList &handlerList = itr->second;
        for(auto handler : handlerList)
        {
            handler(event);
        }
    }
}

