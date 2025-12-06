#pragma once

#include "Base.h"

#include <cstdlib>
#include <functional>
#include <optional>

namespace cru {
struct Guard {
  using ExitFunc = std::function<void()>;

  Guard() = default;
  explicit Guard(const ExitFunc& f) : on_exit(f) {}
  explicit Guard(ExitFunc&& f) : on_exit(std::move(f)) {}

  CRU_DELETE_COPY(Guard)
  CRU_DEFAULT_MOVE(Guard)

  ~Guard() {
    if (on_exit) {
      on_exit();
    }
  }

  void Drop() { on_exit = {}; }

  ExitFunc on_exit;
};

namespace details {
template <typename T>
struct AutoFreePtrDeleter {
  void operator()(T* ptr) const noexcept { ::free(ptr); }
};
}  // namespace details

template <typename T>
using AutoFreePtr = std::unique_ptr<T, details::AutoFreePtrDeleter<T>>;

template <typename T>
inline AutoFreePtr<T> MakeAutoFree(T* ptr) {
  return AutoFreePtr<T>(ptr);
}

template <typename T, typename Destructor>
class AutoDestruct {
 public:
  AutoDestruct() : value_(std::nullopt) {}

  explicit AutoDestruct(T value) : value_(std::move(value)) {}

  CRU_DELETE_COPY(AutoDestruct)

  AutoDestruct(AutoDestruct&& other) noexcept
      : value_(std::move(other.value_)) {
    other.value_ = std::nullopt;
  }

  AutoDestruct& operator=(AutoDestruct&& other) noexcept {
    if (this != &other) {
      DoDestruct();
      value_.swap(other.value_);
    }
    return *this;
  }

  ~AutoDestruct() { DoDestruct(); }

 public:
  bool IsValid() const { return value_.has_value(); }

  void CheckValid(
      std::optional<std::string_view> additional_message = std::nullopt) const {
    if (!IsValid()) {
      std::string message("AutoDestruct contains no object.");
      if (additional_message) {
        message += " ";
        message += *additional_message;
      }
      throw Exception(std::move(message));
    }
  }

  const T& Get() const {
    CheckValid();
    return *value_;
  }

  T Release() {
    CheckValid();
    auto value = std::move(*value_);
    value_ = std::nullopt;
    return value;
  }

  void Reset(std::optional<T> value = std::nullopt) {
    DoDestruct();
    value_ = std::move(value);
  }

  explicit operator bool() const { return value_.has_value(); }

 private:
  void DoDestruct() {
    if (value_) {
      Destructor{}(*value_);
      value_ = std::nullopt;
    }
  }

 private:
  std::optional<T> value_;
};
}  // namespace cru
