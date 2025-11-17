#pragma once
#include "Base.h"
#include "SelfResolvable.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace cru {
class EventHandlerRevoker;

class EventBase : public Object, public SelfResolvable<EventBase> {
  friend EventHandlerRevoker;

 public:
  CRU_DELETE_COPY(EventBase)

 protected:
  EventBase() = default;
  using EventHandlerToken = int;

  /**
   * Remove the handler with the given token. If the token corresponds to no
   * handler (which might have be revoked before), then nothing will be done.
   */
  virtual void RemoveHandler(EventHandlerToken token) = 0;

  inline EventHandlerRevoker CreateRevoker(EventHandlerToken token);
};

class EventHandlerRevoker {
  friend EventBase;

 private:
  EventHandlerRevoker(ObjectResolver<EventBase>&& resolver,
                      EventBase::EventHandlerToken token)
      : resolver_(std::move(resolver)), token_(token) {}

 public:
  /**
   * Revoke the registered handler. If the event has already been destroyed or
   * the handler is already revoked, nothing will be done.
   */
  void operator()() const {
    if (const auto event = resolver_.Resolve()) {
      event->RemoveHandler(token_);
    }
  }

 private:
  ObjectResolver<EventBase> resolver_;
  EventBase::EventHandlerToken token_;
};

inline EventHandlerRevoker EventBase::CreateRevoker(EventHandlerToken token) {
  return EventHandlerRevoker(CreateResolver(), token);
}

struct IBaseEvent : public virtual Interface {
  using SpyOnlyHandler = std::function<void()>;
  virtual EventHandlerRevoker AddSpyOnlyHandler(SpyOnlyHandler handler) = 0;
};

// Provides an interface of event.
// IEvent only allow to add handler but not to raise the event. You may
// want to create an Event object and expose IEvent only so users won't
// be able to emit the event.
template <typename TEventArgs>
struct IEvent : virtual IBaseEvent {
  using Args = TEventArgs;
  using Handler = std::function<void(Args)>;
  virtual EventHandlerRevoker AddHandler(Handler handler) = 0;
};

// A non-copyable non-movable Event class.
// It stores a list of event handlers.
template <typename TEventArgs>
class Event : public EventBase, public IEvent<TEventArgs> {
  using typename IEvent<TEventArgs>::Args;

  using typename IBaseEvent::SpyOnlyHandler;
  using typename IEvent<TEventArgs>::Handler;

 private:
  struct HandlerData {
    HandlerData(EventHandlerToken token, Handler handler)
        : token(token), handler(std::move(handler)) {}
    EventHandlerToken token;
    Handler handler;
  };

 public:
  EventHandlerRevoker AddSpyOnlyHandler(SpyOnlyHandler handler) override {
    return AddHandler([handler = std::move(handler)](Args) { handler(); });
  }

  EventHandlerRevoker AddHandler(Handler handler) override {
    const auto token = current_token_++;
    this->handler_data_list_.emplace_back(token, std::move(handler));
    return CreateRevoker(token);
  }

  // This method will make a copy of all handlers. Because user might delete a
  // handler in a handler, which may lead to seg fault as the handler is
  // deleted while being executed. Thanks to this behavior, all handlers will
  // be taken a snapshot when Raise is called, so even if you delete a handler
  // during this period, all handlers in the snapshot will be executed.
  void Raise(Args args) {
    std::vector<Handler> handlers;
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

#define CRU_DEFINE_EVENT(name, arg_type) \
 private:                                \
  ::cru::Event<arg_type> name##Event_;   \
                                         \
 public:                                 \
  ::cru::IEvent<arg_type>* name##Event() { return &name##Event_; }

namespace details {
struct EventHandlerRevokerDestroyer {
  void operator()(EventHandlerRevoker* p) {
    (*p)();
    delete p;
  }
};
}  // namespace details

// A guard class for event revoker. Automatically revoke it when destroyed.
class EventHandlerRevokerGuard {
 public:
  EventHandlerRevokerGuard() = default;
  explicit EventHandlerRevokerGuard(EventHandlerRevoker&& revoker)
      : revoker_(new EventHandlerRevoker(std::move(revoker))) {}

  EventHandlerRevoker Get() {
    // revoker is only null when this is moved
    // you shouldn't use a moved instance
    assert(revoker_);
    return *revoker_;
  }

  EventHandlerRevoker Release() { return std::move(*revoker_.release()); }

  void Reset() { revoker_.reset(); }

  void Reset(EventHandlerRevoker&& revoker) {
    revoker_.reset(new EventHandlerRevoker(std::move(revoker)));
  }

 private:
  std::unique_ptr<EventHandlerRevoker, details::EventHandlerRevokerDestroyer>
      revoker_;
};

class EventHandlerRevokerListGuard {
 public:
  void Add(EventHandlerRevoker&& revoker) {
    event_revoker_guard_list_.push_back(
        EventHandlerRevokerGuard(std::move(revoker)));
  }

  EventHandlerRevokerListGuard& operator+=(EventHandlerRevoker&& revoker) {
    this->Add(std::move(revoker));
    return *this;
  }

  void Clear() { event_revoker_guard_list_.clear(); }

  bool IsEmpty() const { return event_revoker_guard_list_.empty(); }

 private:
  std::vector<EventHandlerRevokerGuard> event_revoker_guard_list_;
};
}  // namespace cru
