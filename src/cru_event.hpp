#pragma once

#include <type_traits>
#include <functional>
#include <unordered_map>

#include "base.hpp"

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
        using EventHandler = std::function<void(ArgsType&)>;
        using EventHandlerToken = long;

        Event() = default;
        Event(const Event&) = delete;
        Event& operator = (const Event&) = delete;
        Event(Event&&) = delete;
        Event& operator = (Event&&) = delete;
        ~Event() = default;

        EventHandlerToken AddHandler(const EventHandler& handler)
		{
            const auto token = current_token_++;
		    handlers_.emplace(token, handler);
            return token;
		}

		void RemoveHandler(const EventHandlerToken token) {
			auto find_result = handlers_.find(token);
			if (find_result != handlers_.cend())
				handlers_.erase(find_result);
		}

		void Raise(ArgsType& args) {
			for (const auto& handler : handlers_)
				(handler.second)(args);
		}

        bool IsNoHandler() const
		{
		    return handlers_.empty();
		}

    private:
        std::unordered_map<EventHandlerToken, EventHandler> handlers_;

        EventHandlerToken current_token_ = 0;
    };
}
