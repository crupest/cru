#pragma once
#include "Base.h"
#include "Guard.h"
#include "SelfResolvable.h"

#include <algorithm>
#include <cassert>
#include <functional>
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
        : token(token), handler(std::move(handler)), pending_removal(false) {}
    EventHandlerToken token;
    Handler handler;
    bool pending_removal;
  };

 public:
  Event()
      : handler_data_list_(), current_token_(0), recursive_raise_count_(0) {}

  ~Event() override {
    if (recursive_raise_count_ > 0) {
      // You can't destroy the event during raising. So just terminate the
      // process to avoid undefined behavior.
      std::terminate();
    }
  }

  EventHandlerRevoker AddSpyOnlyHandler(SpyOnlyHandler handler) override {
    return AddHandler([handler = std::move(handler)](Args) { handler(); });
  }

  EventHandlerRevoker AddHandler(Handler handler) override {
    const auto token = current_token_++;
    this->handler_data_list_.emplace_back(token, std::move(handler));
    return CreateRevoker(token);
  }

  void Raise(Args args) {
    ++recursive_raise_count_;
    for (const auto& data : handler_data_list_) {
      if (!data.pending_removal) {
        data.handler(args);
      }
    }
    --recursive_raise_count_;
    if (recursive_raise_count_ == 0) {
      std::erase_if(handler_data_list_, [](const HandlerData& data) {
        return data.pending_removal;
      });
    }
  }

 protected:
  void RemoveHandler(EventHandlerToken token) override {
    const auto find_result = std::find_if(
        this->handler_data_list_.begin(), this->handler_data_list_.end(),
        [token](const HandlerData& data) { return data.token == token; });
    if (find_result != this->handler_data_list_.end()) {
      if (recursive_raise_count_ > 0) {
        find_result->pending_removal = true;
      } else {
        this->handler_data_list_.erase(find_result);
      }
    }
  }

 private:
  std::vector<HandlerData> handler_data_list_;
  EventHandlerToken current_token_ = 0;
  int recursive_raise_count_ = 0;
};

#define CRU_DEFINE_EVENT(name, arg_type) \
 private:                                \
  ::cru::Event<arg_type> name##Event_;   \
                                         \
 public:                                 \
  ::cru::IEvent<arg_type>* name##Event() { return &name##Event_; }

#define CRU_DEFINE_EVENT_OVERRIDE(name, arg_type) \
 private:                                         \
  ::cru::Event<arg_type> name##Event_;            \
                                                  \
 public:                                          \
  ::cru::IEvent<arg_type>* name##Event() override { return &name##Event_; }

namespace details {
struct EventHandlerRevokerCaller {
  void operator()(const EventHandlerRevoker& revoker) { revoker(); }
};
}  // namespace details

using EventHandlerRevokerGuard =
    AutoDestruct<EventHandlerRevoker, details::EventHandlerRevokerCaller>;

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
