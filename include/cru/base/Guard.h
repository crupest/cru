#pragma once

#include <cstdlib>
#include <functional>

namespace cru {
struct Guard {
  using ExitFunc = std::function<void()>;

  Guard() = default;
  explicit Guard(const ExitFunc& f) : on_exit(f) {}
  explicit Guard(ExitFunc&& f) : on_exit(std::move(f)) {}
  Guard(const Guard&) = delete;
  Guard(Guard&&) = default;
  Guard& operator=(const Guard&) = delete;
  Guard& operator=(Guard&&) = default;
  ~Guard() {
    if (on_exit) {
      on_exit();
    }
  }

  void Drop() { on_exit = {}; }

  ExitFunc on_exit;
};

template <typename T>
struct FreeLater {
  FreeLater(T* ptr) : ptr(ptr) {}
  ~FreeLater() { ::free(ptr); }

  FreeLater(const FreeLater& other) = delete;
  FreeLater& operator=(const FreeLater& other) = delete;

  FreeLater(FreeLater&& other) : ptr(other.ptr) { other.ptr = nullptr; }
  FreeLater& operator=(FreeLater&& other) {
    if (this != &other) {
      ::free(ptr);
      ptr = other.ptr;
      other.ptr = nullptr;
    }
    return *this;
  }

  operator T*() const { return ptr; }
  T* operator->() { return ptr; }

  T* ptr;
};

}  // namespace cru
