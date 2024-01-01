#pragma once

#include <cru/common/Bitmask.h>
#include <cru/common/SelfResolvable.h>

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace cru {
class Event2Base {
 public:
  virtual ~Event2Base() = default;

  virtual void RevokeHandler(int token_value) = 0;
};

template <typename TArgument, typename TResult>
class EventContext {
 public:
  EventContext() : argument_(), result_() {}
  explicit EventContext(TArgument argument)
      : argument_(std::move(argument)), result_() {}
  EventContext(TArgument argument, TResult result)
      : argument_(std::move(argument)), result_(std::move(result)) {}

  TArgument& GetArgument() { return argument_; }
  const TArgument& GetArgument() const { return argument_; }

  TResult& GetResult() { return result_; }
  const TResult& GetResult() const { return result_; }
  void SetResult(const TResult& result) { result_ = result; }
  void SetResult(TResult&& result) { result_ = std::move(result); }
  TResult TakeResult() { return std::move(result_); }

  bool GetStopHandling() const { return stop_handling_; }
  void SetStopHandling(bool stop = true) { stop_handling_ = stop; }

 private:
  TArgument argument_;
  TResult result_;
  bool stop_handling_ = false;
};

template <typename TArgument, typename TResult>
class Event2;

template <typename T>
constexpr bool is_event2_v = false;

template <typename TArgument, typename TResult>
constexpr bool is_event2_v<Event2<TArgument, TResult>> = true;

class EventHandlerToken {
 public:
  EventHandlerToken(ObjectResolver<Event2Base> event_resolver, int token_value)
      : event_resolver_(std::move(event_resolver)), token_value_(token_value) {}

  ObjectResolver<Event2Base> GetEventResolver() const {
    return event_resolver_;
  }
  int GetTokenValue() const { return token_value_; }
  inline void RevokeHandler() const;

 private:
  ObjectResolver<Event2Base> event_resolver_;
  int token_value_;
};

namespace details {
struct Event2BehaviorFlagTag {};
}  // namespace details
using Event2BehaviorFlag = Bitmask<details::Event2BehaviorFlagTag>;

struct Event2BehaviorFlags {
  /**
   * @brief Make a copy of handler list before invoking handlers. So the event
   * object or its owner can be destroyed during running handlers.
   */
  static constexpr Event2BehaviorFlag CopyHandlers =
      Event2BehaviorFlag::FromOffset(1);
};

template <typename TArgument = std::nullptr_t,
          typename TResult = std::nullptr_t>
class Event2 : public Event2Base,
               public SelfResolvable<Event2<TArgument, TResult>> {
 public:
  using HandlerToken = EventHandlerToken;
  using Context = EventContext<TArgument, TResult>;
  using Handler = std::function<void(Context*)>;
  using SpyOnlyHandler = std::function<void()>;

  template <typename TFunc>
  static std::enable_if_t<std::invocable<TFunc>, Handler> WrapAsHandler(
      TFunc&& handler) {
    return Handler([h = std::forward<TFunc>(handler)](Context*) { h(); });
  }

  template <typename TFunc>
  static std::enable_if_t<std::invocable<TFunc, Context*>, Handler>
  WrapAsHandler(TFunc&& handler) {
    return Handler(std::forward<TFunc>(handler));
  }

 private:
  struct HandlerData {
    int token_value;
    Handler handler;
  };

 public:
  explicit Event2(Event2BehaviorFlag flags = {}) : flags_(flags) {}
  Event2(const Event2&) = delete;
  Event2(Event2&&) = delete;
  Event2& operator=(const Event2&) = delete;
  Event2& operator=(Event2&&) = delete;
  ~Event2() override = default;

 public:
  template <typename TFunc>
  HandlerToken AddHandler(TFunc&& handler) {
    auto token = this->current_token_value_++;
    auto real_handler = WrapAsHandler(std::forward<TFunc>(handler));
    HandlerData handler_data{token, std::move(real_handler)};
    this->handlers_.push_back(std::move(handler_data));
    return HandlerToken(this->CreateResolver(), token);
  }

  void RevokeHandler(int token_value) override {
    auto iter = this->handlers_.cbegin();
    auto end = this->handlers_.cend();
    for (; iter != end; ++iter) {
      if (iter->token_value == token_value) {
        this->handlers_.erase(iter);
        break;
      }
    }
  }

  void RevokeHandler(const HandlerToken& token) {
    return RevokeHandler(token.GetTokenValue());
  }

  TResult Raise() {
    Context context;
    RunInContext(&context);
    return context.TakeResult();
  }

  TResult Raise(TArgument argument) {
    Context context(std::move(argument));
    RunInContext(&context);
    return context.TakeResult();
  }

  TResult Raise(TArgument argument, TResult result) {
    Context context(std::move(argument), std::move(result));
    RunInContext(&context);
    return context.TakeResult();
  }

 private:
  void RunInContext(Context* context) {
    if (this->flags_ & Event2BehaviorFlags::CopyHandlers) {
      std::vector<Handler> handlers_copy;
      for (const auto& handler : this->handlers_) {
        handlers_copy.push_back(handler.handler);
      }
      for (const auto& handler : handlers_copy) {
        if (context->GetStopHandling()) {
          break;
        }
        handler(context);
      }
    } else {
      for (const auto& handler : this->handlers_) {
        if (context->GetStopHandling()) {
          break;
        }
        handler.handler(context);
      }
    }
  }

 private:
  int current_token_value_ = 1;
  std::vector<HandlerData> handlers_;
  Event2BehaviorFlag flags_;
};

inline void EventHandlerToken::RevokeHandler() const {
  auto event = this->event_resolver_.Resolve();
  if (event) {
    event->RevokeHandler(this->token_value_);
  }
}
}  // namespace cru
