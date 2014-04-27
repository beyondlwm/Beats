#include "stdafx.h"
#include "EventDispatcher.h"
#include "BaseEvent.h"

EventDispatcher::EventDispatcher()
{

}

EventDispatcher::~EventDispatcher()
{

}

void EventDispatcher::SubscribeEvent( int type, const EventHandler &handler )
{
    _handlers[type].push_back(handler);
}

void EventDispatcher::DispatchEvent( BaseEvent *event )
{
    auto itr = _handlers.find(event->Type());
    if(itr != _handlers.end())
    {
        event->SetSource(this);
        for(auto handler : itr->second)
        {
            handler(event);
            if(event->Stopped())
                break;
        }
    }
}
