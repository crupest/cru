#pragma once
#include "../Resource.hpp"
#include "Base.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace cru::platform::native {
// The entry point of a ui application.
struct IUiApplication : public virtual INativeResource {
 public:
  static IUiApplication* GetInstance() { return instance; }

 private:
  static IUiApplication* instance;

 protected:
  IUiApplication();

 public:
  ~IUiApplication() override;

  // Block current thread and run the message loop. Return the exit code when
  // message loop gets a quit message (possibly posted by method RequestQuit).
  virtual int Run() = 0;

  // Post a quit message with given quit code.
  virtual void RequestQuit(int quit_code) = 0;

  virtual void AddOnQuitHandler(std::function<void()> handler) = 0;

  virtual void InvokeLater(std::function<void()> action) = 0;
  // Timer id should always be positive and never the same. So it's ok to use
  // negative value to represent no timer.
  virtual long long SetTimeout(std::chrono::milliseconds milliseconds,
                               std::function<void()> action) = 0;
  virtual long long SetInterval(std::chrono::milliseconds milliseconds,
                                std::function<void()> action) = 0;
  // Implementation should guarantee calls on timer id already canceled have no
  // effects and do not crash. Also canceling negative id should always result
  // in no-op.
  virtual void CancelTimer(long long id) = 0;

  virtual std::vector<INativeWindow*> GetAllWindow() = 0;
  virtual std::shared_ptr<INativeWindowResolver> CreateWindow(
      INativeWindow* parent) = 0;

  virtual cru::platform::graph::IGraphFactory* GetGraphFactory() = 0;

  virtual ICursorManager* GetCursorManager() = 0;
  virtual IInputMethodManager* GetInputMethodManager() = 0;
};

// Bootstrap from this.
std::unique_ptr<IUiApplication> CreateUiApplication();
}  // namespace cru::platform::native
