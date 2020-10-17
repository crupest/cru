#pragma once
#include "Base.hpp"

#include "SelfResolvable.hpp"

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

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
  ~IEvent() = default;  // Note that user can't destroy a Event via IEvent. So
                        // destructor should be protected.

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
    this->handler_data_list_.emplace_back(token, handler);
    return CreateRevoker(token);
  }

  EventRevoker AddHandler(EventHandler&& handler) override {
    const auto token = current_token_++;
    this->handler_data_list_.emplace_back(token, std::move(handler));
    return CreateRevoker(token);
  }

  // This method will make a copy of all handlers. Because user might delete a
  // handler in a handler, which may lead to seg fault as the handler is deleted
  // while being executed.
  // Thanks to this behavior, all handlers will be taken a snapshot when Raise
  // is called, so even if you delete a handler during this period, all handlers
  // in the snapshot will be executed.
  void Raise(typename IEvent<TEventArgs>::EventArgs args) {
    std::vector<EventHandler> handlers;
    handlers.reserve(this->handler_data_list_.size());
    for (const auto& data : this->handler_data_list_) {
      handlers.push_back(data.handler);
    }
    for (const auto& handler : handlers) {
      handler(args);
    }
  }

 protected:
  void RemoveHandler(EventHandlerToken token) override {
    const auto find_result = std::find_if(
        this->handler_data_list_.cbegin(), this->handler_data_list_.cend(),
        [token](const HandlerData& data) { return data.token == token; });
    if (find_result != this->handler_data_list_.cend()) {
      this->handler_data_list_.erase(find_result);
    }
  }

 private:
  std::vector<HandlerData> handler_data_list_;
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

// A guard class for event revoker. Automatically revoke it when destroyed.
class EventRevokerGuard {
 public:
  EventRevokerGuard() = default;
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
    Expects(revoker_);
    return *revoker_;
  }

  EventRevoker Release() { return std::move(*revoker_.release()); }

  void Reset(EventRevoker&& revoker) {
    revoker_.reset(new EventRevoker(std::move(revoker)));
  }

 private:
  std::unique_ptr<EventRevoker, details::EventRevokerDestroyer> revoker_;
};

class EventRevokerListGuard {
 public:
  EventRevokerListGuard() = default;
  EventRevokerListGuard(const EventRevokerListGuard& other) = delete;
  EventRevokerListGuard(EventRevokerListGuard&& other) = default;
  EventRevokerListGuard& operator=(const EventRevokerListGuard& other) = delete;
  EventRevokerListGuard& operator=(EventRevokerListGuard&& other) = default;
  ~EventRevokerListGuard() = default;

 public:
  void Add(EventRevoker&& revoker) {
    event_revoker_guard_list_.push_back(EventRevokerGuard(std::move(revoker)));
  }

  EventRevokerListGuard& operator+=(EventRevoker&& revoker) {
    this->Add(std::move(revoker));
    return *this;
  }

 private:
  std::vector<EventRevokerGuard> event_revoker_guard_list_;
};
}  // namespace cru
