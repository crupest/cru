#pragma once
#include "base.hpp"

#include <functional>
#include <map>
#include <utility>

namespace cru {
// A non-copyable non-movable Event class.
// It stores a list of event handlers.
template <typename... TArgs>
class Event {
 public:
  using EventHandler = std::function<void(TArgs...)>;
  using EventHandlerToken = long;

  Event() = default;
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  Event(Event&&) = delete;
  Event& operator=(Event&&) = delete;
  ~Event() = default;

  EventHandlerToken AddHandler(const EventHandler& handler) {
    const auto token = current_token_++;
    handlers_.emplace(token, handler);
    return token;
  }

  void RemoveHandler(const EventHandlerToken token) {
    auto find_result = handlers_.find(token);
    if (find_result != handlers_.cend()) handlers_.erase(find_result);
  }

  template <typename... Args>
  void Raise(Args&&... args) {
    for (const auto& handler : handlers_)
      (handler.second)(std::forward<Args>(args)...);
  }

 private:
  std::map<EventHandlerToken, EventHandler> handlers_;

  EventHandlerToken current_token_ = 0;
};
}  // namespace cru
