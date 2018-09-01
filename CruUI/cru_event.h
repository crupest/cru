#pragma once

#include <type_traits>
#include <list>
#include <memory>
#include <algorithm>

#include "base.h"

namespace cru {
	//Base class of all event args.
	class BasicEventArgs : public Object
	{
	public:
		explicit BasicEventArgs(Object* sender)
            : sender_(sender)
		{
		    
		}
	    BasicEventArgs(const BasicEventArgs& other) = default;
	    BasicEventArgs(BasicEventArgs&& other) = default;
	    BasicEventArgs& operator=(const BasicEventArgs& other) = default;
	    BasicEventArgs& operator=(BasicEventArgs&& other) = default;
		~BasicEventArgs() override = default;

		//Get the sender of the event.
		Object* GetSender() const
		{
            return sender_;
		}

	private:
		Object* sender_;
	};


	//A non-copyable non-movable Event class.
	//It stores a list of event handlers.
	//TArgsType must be subclass of BasicEventArgs.
    template<typename TArgsType>
    class Event
	{
    public:
		static_assert(std::is_base_of_v<BasicEventArgs, TArgsType>,
			"TArgsType must be subclass of BasicEventArgs.");


        using ArgsType = TArgsType;
        using EventHandler = Action<ArgsType&>;
        using EventHandlerPtr = std::shared_ptr<EventHandler>;

        Event() = default;
        Event(const Event&) = delete;
        Event& operator = (const Event&) = delete;
        Event(Event&&) = delete;
        Event& operator = (Event&&) = delete;
        ~Event() = default;

        //Create a EventHandlerPtr from the given handler,
        //add it to list and return it.
		EventHandlerPtr AddHandler(EventHandler&& handler)
		{
			EventHandlerPtr ptr = std::make_shared<EventHandler>(std::move(handler));
			handlers_.push_back(ptr);
			return ptr;
		}

		void AddHandler(EventHandlerPtr handler) {
			handlers_.push_back(handler);
		}

		void RemoveHandler(EventHandlerPtr handler) {
			auto find_result = std::find(handlers_.cbegin(), handlers_.cend(), handler);
			if (find_result != handlers_.cend())
				handlers_.erase(find_result);
		}

		void Raise(ArgsType& args) {
			for (auto ptr : handlers_)
				(*ptr)(args);
		}

    private:
        std::list<EventHandlerPtr> handlers_;
    };
}
