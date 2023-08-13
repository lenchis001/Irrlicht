#pragma once

#include "map"

#include "boost/any.hpp"
#include "boost/function.hpp"
#include "boost/interprocess/sync/scoped_lock.hpp"
#include "boost/thread.hpp"

namespace GUIEditor::Utils {
    template <class T>
    class EventHandler {
        typedef boost::function<void(T)> EventHandlerFunction;

    public:
        inline void callHandlers(T eventData = (T)NULL);

        inline void addEventHandler(std::wstring handlerName, EventHandler<T>& eventHandler);
        inline void addEventHandler(std::wstring handlerName, EventHandlerFunction eventHandler);
        inline void removeEventHandler(std::wstring handlerName);

    private:
        boost::mutex _mutex;

        std::map<std::wstring, EventHandlerFunction> _eventHandlers;
    };

    template <class T>
    inline void EventHandler<T>::callHandlers(T eventData)
    {
        boost::interprocess::scoped_lock<boost::mutex> scopeBlocker(_mutex);
        for (auto eventHandler : _eventHandlers) {
            eventHandler.second(eventData);
        }
    }

    template <class T>
    inline void EventHandler<T>::addEventHandler(std::wstring handlerName, EventHandlerFunction eventHandler)
    {
        boost::interprocess::scoped_lock<boost::mutex> scopeBlocker(_mutex);

        _eventHandlers.insert({ handlerName, eventHandler });
    }

    template <class T>
    inline void EventHandler<T>::addEventHandler(std::wstring handlerName, EventHandler<T>& eventHandler)
    {
        boost::interprocess::scoped_lock<boost::mutex> scopeBlocker(_mutex);

        _eventHandlers.insert({ handlerName, [&](T eventData) {
                                   eventHandler.callHandlers(eventData);
                               } });
    }

    template <class T>
    inline void EventHandler<T>::removeEventHandler(std::wstring handlerName)
    {
        boost::interprocess::scoped_lock<boost::mutex> scopeBlocker(_mutex);

        if (_eventHandlers.find(handlerName) != _eventHandlers.end())
            _eventHandlers.erase(handlerName);
    }
}