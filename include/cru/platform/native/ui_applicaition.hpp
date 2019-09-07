#pragma once
#include "../native_resource.hpp"

#include "cursor.hpp"

#include <chrono>
#include <functional>
#include <vector>

namespace cru::platform::native {
class NativeWindow;

// The entry point of a ui application.
// It will call IGraphFactory::CreateInstance during its creation
// and set graph factory to be auto deleted. If you want to keep
// the graph factory then you should manually set it to false after
// creating the ui application.
class UiApplication : public NativeResource {
 public:
  // Create a platform-specific instance and save it as the global instance.
  // Do not create the instance twice. Implements should assert for that.
  // After creating, get the instance by GetInstance.
  static UiApplication* CreateInstance();

  // Get the global instance. If it is not created, then return nullptr.
  static UiApplication* GetInstance();

 protected:
  UiApplication() = default;

 public:
  UiApplication(const UiApplication& other) = delete;
  UiApplication& operator=(const UiApplication& other) = delete;

  UiApplication(UiApplication&& other) = delete;
  UiApplication& operator=(UiApplication&& other) = delete;

  ~UiApplication() override = default;

 public:
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

  virtual CursorManager* GetCursorManager() = 0;
};
}  // namespace cru::platform::native
