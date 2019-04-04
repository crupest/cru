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
class EventBase {
  friend EventRevoker;

 protected:
  using EventHandlerToken = long;

  EventBase() : resolve_ptr_(new std::reference_wrapper(*this)) {}

  EventBase(const EventBase& other) = delete;
  EventBase(EventBase&& other) = delete;
  EventBase& operator=(const EventBase& other) = delete;
  EventBase& operator=(EventBase&& other) = delete;
  virtual ~EventBase() = default;

  virtual void RemoveHandler(EventHandlerToken token) = 0;

  inline EventRevoker CreateRevoker(EventHandlerToken token);

 private:
  std::shared_ptr<std::reference_wrapper<EventBase>> resolve_ptr_;
};
}  // namespace details

class EventRevoker {
  friend class ::cru::details::EventBase;

 private:
  EventRevoker(const std::shared_ptr<
                   std::reference_wrapper<details::EventBase>>& resolve_ptr,
               details::EventBase::EventHandlerToken token)
      : weak_ptr_(resolve_ptr), token_(token) {}

 public:
  EventRevoker(const EventRevoker& other) = default;
  EventRevoker(EventRevoker&& other) = default;
  EventRevoker& operator=(const EventRevoker& other) = default;
  EventRevoker& operator=(EventRevoker&& other) = default;
  ~EventRevoker() = default;

  void operator()() const {
    const auto true_resolver = weak_ptr_.lock();
    if (true_resolver) {
      (*true_resolver).get().RemoveHandler(token_);
    }
  }

 private:
  std::weak_ptr<std::reference_wrapper<details::EventBase>> weak_ptr_;
  details::EventBase::EventHandlerToken token_;
};

inline EventRevoker details::EventBase::CreateRevoker(EventHandlerToken token) {
  return EventRevoker(resolve_ptr_, token);
}

// A non-copyable non-movable Event class.
// It stores a list of event handlers.
template <typename... TArgs>
class Event : public details::EventBase {
 public:
  using EventHandler = std::function<void(TArgs...)>;

  Event() = default;
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  Event(Event&&) = delete;
  Event& operator=(Event&&) = delete;
  ~Event() = default;

  EventRevoker AddHandler(const EventHandler& handler) {
    const auto token = current_token_++;
    handlers_.emplace(token, handler);
    return CreateRevoker(token);
  }

  EventRevoker AddHandler(EventHandler&& handler) {
    const auto token = current_token_++;
    handlers_.emplace(token, std::move(handler));
    return CreateRevoker(token);
  }

  template <typename Arg>
  EventRevoker AddHandler(Arg&& handler) {
    static_assert(std::is_invocable_v<Arg, TArgs...>, "Handler not invocable.");
    const auto token = current_token_++;
    handlers_.emplace(token, EventHandler(std::forward<Arg>(handler)));
    return CreateRevoker(token);
  }

  template <typename... Args>
  void Raise(Args&&... args) {
    for (const auto& handler : handlers_)
      (handler.second)(std::forward<Args>(args)...);
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
  EventRevokerGuard() = default;
  EventRevokerGuard(const EventRevokerGuard& other) = delete;
  EventRevokerGuard(EventRevokerGuard&& other) = delete;
  EventRevokerGuard& operator=(const EventRevokerGuard& other) = delete;
  EventRevokerGuard& operator=(EventRevokerGuard&& other) = delete;
  ~EventRevokerGuard() {
    for (const auto revoker : revokers_) revoker();
  }

  void Add(EventRevoker revoker) { revokers_.push_back(std::move(revoker)); }

 private:
  std::list<EventRevoker> revokers_;
};
}  // namespace cru
