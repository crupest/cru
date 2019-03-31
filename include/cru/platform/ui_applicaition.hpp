#pragma once
#include "cru/common/base.hpp"

#include <chrono>
#include <functional>

namespace cru::platform {
struct NativeWindow;
struct GraphFactory;

struct UiApplication : public virtual Interface {
  static UiApplication* GetInstance();

  virtual int Run() = 0;
  virtual void Quit(int quite_code) = 0;

  virtual void InvokeLater(const std::function<void()>& action) = 0;
  virtual unsigned long SetTimeout(std::chrono::milliseconds milliseconds,
                          const std::function<void()>& action) = 0;
  virtual unsigned long SetInterval(std::chrono::milliseconds milliseconds,
                           const std::function<void()>& action) = 0;
  virtual void CancelTimer(unsigned long id) = 0;

  virtual NativeWindow* CreateWindow() = 0;

  virtual GraphFactory* GetGraphFactory() = 0;
};
}  // namespace cru::platform
