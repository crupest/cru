#pragma once
#include "base.hpp"

#include "self_resolvable.hpp"

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <utility>

namespace cru {
class EventRevoker;

namespace details {
// Base class of all Event<T...>.
// It erases event args types and provides a
// unified form to create event revoker and
// revoke(remove) handler.
class EventBase : private SelfResovable<EventBase> {
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
  EventRevoker(ObjectResovler<details::EventBase>&& resolver,
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
  ObjectResovler<details::EventBase> resolver_;
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
    handlers_->emplace_back(token, handler);
    return CreateRevoker(token);
  }

  EventRevoker AddHandler(EventHandler&& handler) override {
    const auto token = current_token_++;
    handlers_->emplace_back(token, std::move(handler));
    return CreateRevoker(token);
  }

  void Raise(EventArgs args) {
    // Make a copy of the shared_ptr to retain the handlers in case the event is
    // deleted during executing. if the handler is a lambda with member data,
    // then the member data will be destroyed and result in seg fault.
    const auto handlers = handlers_;
    // do not use range for because it is not safe when current node is deleted.
    auto i = handlers->cbegin();
    while (i != handlers->cend()) {
      // first move the i forward.
      const auto current = i++;
      // The same as above but in case that the RemoveHandler is called and
      // the handler is not valid then.
      const auto handler = current->handler;
      handler(args);
    }
  }

 protected:
  void RemoveHandler(EventHandlerToken token) override {
    auto find_result = std::find_if(handlers_->cbegin(), handlers_->cend(),
                                    [token](const HandlerData& handler_data) {
                                      return handler_data.token == token;
                                    });
    if (find_result != handlers_->cend()) handlers_->erase(find_result);
  }

 private:
  std::shared_ptr<std::list<HandlerData>> handlers_ =
      std::make_shared<std::list<HandlerData>>();
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
