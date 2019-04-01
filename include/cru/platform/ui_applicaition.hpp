#pragma once
#include "cru/common/base.hpp"

#include "basic_types.hpp"

#include <chrono>
#include <functional>
#include <vector>

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

  virtual std::vector<NativeWindow*> GetAllWindow() = 0;
  virtual NativeWindow* CreateWindow(NativeWindow* parent) = 0;

  virtual GraphFactory* GetGraphFactory() = 0;
};
}  // namespace cru::platform
