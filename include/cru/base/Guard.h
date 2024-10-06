#pragma once

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
}  // namespace cru
