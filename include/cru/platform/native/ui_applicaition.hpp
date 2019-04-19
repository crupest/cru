#pragma once
#include "cru/common/base.hpp"

#include <chrono>
#include <functional>
#include <vector>

namespace cru::platform::graph {
struct GraphFactory;
}

namespace cru::platform::native {
struct NativeWindow;

struct UiApplication : public virtual Interface {
  static UiApplication* GetInstance();

  virtual int Run() = 0;
  virtual void Quit(int quite_code) = 0;

  virtual void AddOnQuitHandler(const std::function<void()>& handler) = 0;

  virtual void InvokeLater(const std::function<void()>& action) = 0;
  virtual unsigned long SetTimeout(std::chrono::milliseconds milliseconds,
                                   const std::function<void()>& action) = 0;
  virtual unsigned long SetInterval(std::chrono::milliseconds milliseconds,
                                    const std::function<void()>& action) = 0;
  virtual void CancelTimer(unsigned long id) = 0;

  virtual std::vector<NativeWindow*> GetAllWindow() = 0;
  virtual NativeWindow* CreateWindow(NativeWindow* parent) = 0;
};
}  // namespace cru::platform::ui
