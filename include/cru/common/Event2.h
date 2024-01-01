#pragma once

#include <cru/common/SelfResolvable.h>

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace cru {
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
  void SetResult(TResult&& result) { result_ = std::forward<TResult>(result); }

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

template <typename TEvent2>
class EventHandlerToken {
  static_assert(is_event2_v<TEvent2>, "TEvent2 must be Event2 class.");

 public:
  EventHandlerToken(ObjectResolver<TEvent2> event_resolver, int token_value);

  ObjectResolver<TEvent2> GetEventResolver() const { return event_resolver_; }
  int GetTokenValue() const { return token_value_; }
  void RevokeHandler() const;

 private:
  ObjectResolver<TEvent2> event_resolver_;
  int token_value_;
};

template <typename TArgument = std::nullptr_t,
          typename TResult = std::nullptr_t>
class Event2 : public SelfResolvable<Event2<TArgument, TResult>> {
 public:
  using HandlerToken = EventHandlerToken<Event2>;
  using Context = EventContext<TArgument, TResult>;
  using Handler = std::function<void(Context&)>;
  using SpyOnlyHandler = std::function<void()>;

  template <typename TFunc>
  static std::enable_if_t<std::invocable<TFunc>, Handler> WrapAsHandler(
      TFunc&& handler) {
    return Handler([h = std::forward<TFunc>(handler)](Context&) { h(); });
  }

  template <typename TFunc>
  static std::enable_if_t<std::invocable<TFunc, Context&>, Handler>
  WrapAsHandler(TFunc&& handler) {
    return Handler(std::forward<TFunc>(handler));
  }

 private:
  struct HandlerData {
    int token_value_;
    Handler handler_;
  };

 public:
  Event2(const Event2&) = delete;
  Event2(Event2&&) = delete;
  Event2& operator=(const Event2&) = delete;
  Event2& operator=(Event2&&) = delete;
  ~Event2() override = default;

 public:
  

  void RevokeHandler(int token_value);
  void RevokeHandler(const HandlerToken& token) {
    return RevokeHandler(token.GetTokenValue());
  }

 private:
  int current_token_value_ = 1;
  std::vector<HandlerData> handlers_;
};

template <typename TEvent2>
void EventHandlerToken<TEvent2>::RevokeHandler() const {
  auto event = this->event_resolver_.Resolve();
  if (event) {
    event->RevokeHandler(this->token_value_);
  }
}
}  // namespace cru
