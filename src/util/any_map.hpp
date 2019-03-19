#pragma once
#include "pre.hpp"

#include <any>
#include <functional>
#include <optional>
#include <typeinfo>
#include <unordered_map>

#include "base.hpp"
#include "format.hpp"

namespace cru::util {
// A map with String as key and any type as value.
// It also has notification when value with specified key changed.
class AnyMap : public Object {
 public:
  using ListenerToken = long;
  using Listener = std::function<void(const std::any&)>;

  AnyMap() = default;
  AnyMap(const AnyMap& other) = delete;
  AnyMap(AnyMap&& other) = delete;
  AnyMap& operator=(const AnyMap& other) = delete;
  AnyMap& operator=(AnyMap&& other) = delete;
  ~AnyMap() override = default;

  // return the value if the value exists and the type of value is T.
  // return a null optional if value doesn't exists.
  // throw std::runtime_error if type is mismatch.
  template <typename T>
  std::optional<T> GetOptionalValue(const String& key) const {
    try {
      const auto find_result = map_.find(key);
      if (find_result != map_.cend()) {
        const auto& value = find_result->second.first;
        if (value.has_value()) return std::any_cast<T>(value);
        return std::nullopt;
      }
      return std::nullopt;
    } catch (const std::bad_any_cast&) {
      throw std::runtime_error(
          Format("Value of key \"{}\" in AnyMap is not of the type {}.",
                 ToUtf8String(key), typeid(T).name()));
    }
  }

  // return the value if the value exists and the type of value is T.
  // throw if value doesn't exists. (different from "GetOptionalValue").
  // throw std::runtime_error if type is mismatch.
  template <typename T>
  T GetValue(const String& key) const {
    const auto optional_value = GetOptionalValue<T>(key);
    if (optional_value.has_value())
      return optional_value.value();
    else
      throw std::runtime_error(
          Format("Key \"{}\" does not exists in AnyMap.", ToUtf8String(key)));
  }

  // Set the value of key, and trigger all related listeners.
  template <typename T>
  void SetValue(const String& key, T&& value) {
    auto& p = map_[key];
    p.first = std::make_any<T>(std::forward<T>(value));
    InvokeListeners(p.second, p.first);
  }

  // Remove the value of the key.
  void ClearValue(const String& key) {
    auto& p = map_[key];
    p.first = std::any{};
    InvokeListeners(p.second, std::any{});
  }

  // Add a listener which is called when value of key is changed.
  // Return a token used to remove the listener.
  ListenerToken RegisterValueChangeListener(const String& key,
                                            const Listener& listener);

  // Remove a listener by token.
  void UnregisterValueChangeListener(ListenerToken token);

 private:
  void InvokeListeners(std::list<ListenerToken>& listener_list,
                       const std::any& value);

 private:
  std::unordered_map<String, std::pair<std::any, std::list<ListenerToken>>>
      map_{};
  std::unordered_map<ListenerToken, Listener> listeners_{};
  ListenerToken current_listener_token_ = 0;
};
}  // namespace cru::util
