#pragma once
#include "base.hpp"

#include <functional>
#include <map>
#include <memory>
#include <utility>

namespace cru {
using EventHandlerRevoker = std::function<void()>;

// A non-copyable non-movable Event class.
// It stores a list of event handlers.
template <typename... TArgs>
class Event {
 private:
  using EventResolver = std::function<Event*()>;
  class EventHandlerRevokerImpl {
   public:
    EventHandlerRevokerImpl(const std::shared_ptr<EventResolver>& resolver,
                            long token)
        : resolver_(resolver), token_(token) {}
    EventHandlerRevokerImpl(const EventHandlerRevokerImpl& other) = default;
    EventHandlerRevokerImpl(EventHandlerRevokerImpl&& other) = default;
    EventHandlerRevokerImpl& operator=(const EventHandlerRevokerImpl& other) =
        default;
    EventHandlerRevokerImpl& operator=(EventHandlerRevokerImpl&& other) =
        default;
    ~EventHandlerRevokerImpl() = default;

    void operator()() {
      const auto true_resolver = resolver_.lock();
      if (true_resolver) {
        (*true_resolver)()->RemoveHandler(token_);
      }
    }

   private:
    std::weak_ptr<EventResolver> resolver_;
    long token_;
  };

 public:
  using EventHandler = std::function<void(TArgs...)>;

  Event()
      : event_resolver_(new std::function<Event*()>([this] { return this; })) {}
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  Event(Event&&) = delete;
  Event& operator=(Event&&) = delete;
  ~Event() = default;

  EventHandlerRevoker AddHandler(const EventHandler& handler) {
    const auto token = current_token_++;
    handlers_.emplace(token, handler);
    return EventHandlerRevoker(EventHandlerRevokerImpl(event_resolver_, token));
  }

  EventHandlerRevoker AddHandler(EventHandler&& handler) {
    const auto token = current_token_++;
    handlers_.emplace(token, std::move(handler));
    return EventHandlerRevoker(EventHandlerRevokerImpl(event_resolver_, token));
  }

  template <typename Arg>
  EventHandlerRevoker AddHandler(Arg&& handler) {
    static_assert(std::is_invocable_v<Arg, TArgs...>, "Handler not invocable.");
    const auto token = current_token_++;
    handlers_.emplace(token, EventHandler(std::forward<Arg>(handler)));
    return EventHandlerRevoker(EventHandlerRevokerImpl(event_resolver_, token));
  }

  template <typename... Args>
  void Raise(Args&&... args) {
    for (const auto& handler : handlers_)
      (handler.second)(std::forward<Args>(args)...);
  }

 private:
  void RemoveHandler(const long token) {
    auto find_result = handlers_.find(token);
    if (find_result != handlers_.cend()) handlers_.erase(find_result);
  }

 private:
  std::map<long, EventHandler> handlers_{};
  long current_token_ = 0;
  std::shared_ptr<EventResolver> event_resolver_;
};
}  // namespace cru
