#pragma once
#include "../native_resource.hpp"

#include <chrono>
#include <functional>
#include <vector>

namespace cru::platform::native {
struct INativeWindow;
struct ICursorManager;

// The entry point of a ui application.
// It will call IGraphFactory::CreateInstance during its creation
// and set graph factory to be auto deleted. If you want to keep
// the graph factory then you should manually set it to false after
// creating the ui application.
struct IUiApplication : public virtual INativeResource {
 public:
  // Create a platform-specific instance and save it as the global instance.
  // Do not create the instance twice. Implements should assert for that.
  // After creating, get the instance by GetInstance.
  static IUiApplication* CreateInstance();

  // Get the global instance. If it is not created, then return nullptr.
  static IUiApplication* GetInstance();

 public:
  // Block current thread and run the message loop. Return the exit code when
  // message loop gets a quit message (possibly posted by method RequestQuit).
  virtual int Run() = 0;

  // Post a quit message with given quit code.
  virtual void RequestQuit(int quit_code) = 0;

  virtual void AddOnQuitHandler(const std::function<void()>& handler) = 0;

  virtual void InvokeLater(const std::function<void()>& action) = 0;
  virtual unsigned long SetTimeout(std::chrono::milliseconds milliseconds,
                                   const std::function<void()>& action) = 0;
  virtual unsigned long SetInterval(std::chrono::milliseconds milliseconds,
                                    const std::function<void()>& action) = 0;
  virtual void CancelTimer(unsigned long id) = 0;

  virtual std::vector<INativeWindow*> GetAllWindow() = 0;
  virtual INativeWindow* CreateWindow(INativeWindow* parent) = 0;

  virtual ICursorManager* GetCursorManager() = 0;
};
}  // namespace cru::platform::native
