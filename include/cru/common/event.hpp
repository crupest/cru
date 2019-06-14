#pragma once
#include "base.hpp"

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <utility>

namespace cru {
class EventRevoker;

namespace details {
// Base class of all Event<T...>.
// It erases event args types and provides a
// unified form to create event revoker and
// revoke(remove) handler.
class EventBase {
  friend EventRevoker;

 protected:
  using EventHandlerToken = long;

  EventBase() : resolver_(new EventBase*(this)) {}
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

 private:
  std::shared_ptr<EventBase*> resolver_;
};
}  // namespace details

// A non-copyable and movable event revoker.
// Call function call operator to revoke the handler.
class EventRevoker {
  friend class ::cru::details::EventBase;

 private:
  EventRevoker(const std::shared_ptr<details::EventBase*>& resolver,
               details::EventBase::EventHandlerToken token)
      : weak_resolver_(resolver), token_(token) {}

 public:
  EventRevoker(const EventRevoker& other) = delete;
  EventRevoker(EventRevoker&& other) = default;
  EventRevoker& operator=(const EventRevoker& other) = delete;
  EventRevoker& operator=(EventRevoker&& other) = default;
  ~EventRevoker() = default;

  // Revoke the registered handler. If the event has already
  // been destroyed, then nothing will be done. If one of the
  // copies calls this, then other copies's calls will have no
  // effect. (They have the same token.)
  void operator()() const {
    const auto true_resolver = weak_resolver_.lock();
    // if true_resolver is nullptr, then the event has been destroyed.
    if (true_resolver) {
      (*true_resolver)->RemoveHandler(token_);
    }
  }

 private:
  std::weak_ptr<details::EventBase*> weak_resolver_;
  details::EventBase::EventHandlerToken token_;
};

inline EventRevoker details::EventBase::CreateRevoker(EventHandlerToken token) {
  return EventRevoker(resolver_, token);
}

// int -> int
// Point -> const Point&
// int& -> int&
template <typename TRaw>
using DeducedEventArgs = std::conditional_t<
    std::is_lvalue_reference_v<TRaw>, TRaw,
    std::conditional_t<std::is_scalar_v<TRaw>, TRaw, const TRaw&>>;

// Provides an interface of event.
//
// Note that this class does not inherit Interface because Interface is
// public destructable, but I want to make this class not public
// destructable to prevent user from destructing it.
//
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
 public:
  Event() = default;
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  Event(Event&&) = delete;
  Event& operator=(Event&&) = delete;
  ~Event() = default;

  EventRevoker AddHandler(const EventHandler& handler) override {
    const auto token = current_token_++;
    handlers_.emplace(token, handler);
    return CreateRevoker(token);
  }

  EventRevoker AddHandler(EventHandler&& handler) override {
    const auto token = current_token_++;
    handlers_.emplace(token, std::move(handler));
    return CreateRevoker(token);
  }

  void Raise(EventArgs args) {
    // copy the handlers to a list, because the handler might be removed
    // during executing, and the handler with its data will be destroyed.
    // if the handler is a lambda with member data, then the member data
    // will be destroyed and result in seg fault.
    std::list<EventHandler> handlers;
    for (const auto& [key, handler] : handlers_) handlers.push_back(handler);
    for (const auto& handler : handlers) handler(args);
  }

 protected:
  void RemoveHandler(EventHandlerToken token) override {
    auto find_result = handlers_.find(token);
    if (find_result != handlers_.cend()) handlers_.erase(find_result);
  }

 private:
  std::map<EventHandlerToken, EventHandler> handlers_{};
  EventHandlerToken current_token_ = 0;
};

class EventRevokerGuard {
 public:
  explicit EventRevokerGuard(EventRevoker&& revoker)
      : revoker_(std::move(revoker)) {}
  EventRevokerGuard(const EventRevokerGuard& other) = delete;
  EventRevokerGuard(EventRevokerGuard&& other) = default;
  EventRevokerGuard& operator=(const EventRevokerGuard& other) = delete;
  EventRevokerGuard& operator=(EventRevokerGuard&& other) = default;
  ~EventRevokerGuard() { revoker_(); }

  EventRevoker ReleaseAndGet() { return std::move(revoker_); }

 private:
  EventRevoker revoker_;
};
}  // namespace cru
