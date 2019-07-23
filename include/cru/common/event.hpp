#pragma once
#include "base.hpp"

#include "self_resolvable.hpp"

#include <forward_list>
#include <functional>
#include <memory>
#include <utility>

namespace cru {
class EventRevoker;

namespace details {
// Base class of all Event<T...>.
// It erases event args types and provides a
// unified form to create event revoker and
// revoke(remove) handler.
class EventBase : public SelfResolvable<EventBase> {
  friend EventRevoker;

 protected:
  using EventHandlerToken = long;

  EventBase() {}
  EventBase(const EventBase& other) = delete;
  EventBase(EventBase&& other) = delete;
  EventBase& operator=(const EventBase& other) = delete;
  EventBase& operator=(EventBase&& other) = delete;
  virtual ~EventBase() = default;

  // Remove the handler with the given token. If the token
  // corresponds to no handler (which might have be revoked
  // before), then nothing will be done.
  virtual void RemoveHandler(EventHandlerToken token) = 0;

  // Create a revoker with the given token.
  inline EventRevoker CreateRevoker(EventHandlerToken token);
};
}  // namespace details

// A non-copyable and movable event revoker.
// Call function call operator to revoke the handler.
class EventRevoker {
  friend details::EventBase;

 private:
  EventRevoker(ObjectResolver<details::EventBase>&& resolver,
               details::EventBase::EventHandlerToken token)
      : resolver_(std::move(resolver)), token_(token) {}

 public:
  EventRevoker(const EventRevoker& other) = default;
  EventRevoker(EventRevoker&& other) = default;
  EventRevoker& operator=(const EventRevoker& other) = default;
  EventRevoker& operator=(EventRevoker&& other) = default;
  ~EventRevoker() = default;

  // Revoke the registered handler. If the event has already
  // been destroyed, then nothing will be done. If one of the
  // copies calls this, then other copies's calls will have no
  // effect. (They have the same token.)
  void operator()() const {
    if (const auto event = resolver_.Resolve()) {
      event->RemoveHandler(token_);
    }
  }

 private:
  ObjectResolver<details::EventBase> resolver_;
  details::EventBase::EventHandlerToken token_;
};

inline EventRevoker details::EventBase::CreateRevoker(EventHandlerToken token) {
  return EventRevoker(CreateResolver(), token);
}

// int -> int
// Point -> const Point&
// int& -> int&
template <typename TRaw>
using DeducedEventArgs = std::conditional_t<
    std::is_lvalue_reference_v<TRaw>, TRaw,
    std::conditional_t<std::is_scalar_v<TRaw>, TRaw, const TRaw&>>;

// Provides an interface of event.
// IEvent only allow to add handler but not to raise the event. You may
// want to create an Event object and expose IEvent only so users won't
// be able to emit the event.
template <typename TEventArgs>
struct IEvent {
 public:
  using EventArgs = DeducedEventArgs<TEventArgs>;
  using EventHandler = std::function<void(EventArgs)>;

 protected:
  IEvent() = default;
  IEvent(const IEvent& other) = delete;
  IEvent(IEvent&& other) = delete;
  IEvent& operator=(const IEvent& other) = delete;
  IEvent& operator=(IEvent&& other) = delete;
  ~IEvent() = default;

 public:
  virtual EventRevoker AddHandler(const EventHandler& handler) = 0;
  virtual EventRevoker AddHandler(EventHandler&& handler) = 0;
};

// A non-copyable non-movable Event class.
// It stores a list of event handlers.
template <typename TEventArgs>
class Event : public details::EventBase, public IEvent<TEventArgs> {
  using typename IEvent<TEventArgs>::EventHandler;

 private:
  struct HandlerData {
    HandlerData(EventHandlerToken token, EventHandler handler)
        : token(token), handler(handler) {}
    EventHandlerToken token;
    EventHandler handler;
  };

 public:
  Event() = default;
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  Event(Event&&) = delete;
  Event& operator=(Event&&) = delete;
  ~Event() = default;

  EventRevoker AddHandler(const EventHandler& handler) override {
    const auto token = current_token_++;
    this->handler_data_list_.emplace_after(this->last_handler_iterator_ ,token, handler);
    ++(this->last_handler_iterator_);
    return CreateRevoker(token);
  }

  EventRevoker AddHandler(EventHandler&& handler) override {
    const auto token = current_token_++;
    this->handler_data_list_.emplace_after(this->last_handler_iterator_ ,token, std::move(handler));
    ++(this->last_handler_iterator_);
    return CreateRevoker(token);
  }

  // This method will make a copy of all handlers. Because user might delete a
  // handler in a handler, which may lead to seg fault as the handler is deleted
  // while being executed.
  // Thanks to this behavior, all handlers will be taken a snapshot when Raise
  // is called, so even if you delete a handler during this period, all handlers
  // in the snapshot will be executed.
  void Raise(EventArgs args) {
    std::forward_list<EventHandler> handlers;
    auto iter = handlers.cbefore_begin();
    for (const auto& data : this->handler_data_list_) {
      handlers.insert_after(iter, data.handler);
      iter++;
    }
    for (const auto& handler : handlers) {
      handler(args);
    }
  }

 protected:
  void RemoveHandler(EventHandlerToken token) override {
    this->handler_data_list_.remove_if(
				       [token](const HandlerData& data) { return data.token == token; });
  }

 private:
  std::forward_list<HandlerData> handler_data_list_{};
  typename std::forward_list<HandlerData>::const_iterator last_handler_iterator_ = this->handler_data_list_.cbefore_begin(); // remember the last handler to make push back O(1)
  EventHandlerToken current_token_ = 0;
};

namespace details {
struct EventRevokerDestroyer {
  void operator()(EventRevoker* p) {
    (*p)();
    delete p;
  }
};
}  // namespace details

class EventRevokerGuard {
 public:
  explicit EventRevokerGuard(EventRevoker&& revoker)
      : revoker_(new EventRevoker(std::move(revoker))) {}
  EventRevokerGuard(const EventRevokerGuard& other) = delete;
  EventRevokerGuard(EventRevokerGuard&& other) = default;
  EventRevokerGuard& operator=(const EventRevokerGuard& other) = delete;
  EventRevokerGuard& operator=(EventRevokerGuard&& other) = default;
  ~EventRevokerGuard() = default;

  EventRevoker Get() {
    // revoker is only null when this is moved
    // you shouldn't use a moved instance
    assert(revoker_);
    return *revoker_;
  }

  void Release() { revoker_.release(); }

 private:
  std::unique_ptr<EventRevoker, details::EventRevokerDestroyer> revoker_;
};  // namespace cru
}  // namespace cru
