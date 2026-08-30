#pragma once
#include "Base.h"
#include "Menu.h"
#include "SaveOpenDialogOptions.h"

#include <cru/base/Guard.h>
#include <cru/platform/graphics/Factory.h>

#include <chrono>
#include <functional>
#include <optional>
#include <thread>
#include <vector>

namespace cru::platform::gui {
struct ICursorManager;
struct INativeWindow;
struct IInputMethodContext;
struct IClipboard;

class CRU_PLATFORM_GUI_API DeleteLaterPool : public Object {
 public:
  void Add(Object* object);

  void Clean();

 private:
  // May contain duplicate object pointer. When performing deleting, use a set
  // to record deleted objects to avoid double delete.
  std::vector<Object*> pool_;
};

/**
 * @brief The entry point of a ui application.
 *
 * Native UI application implementations install themselves as the current
 * thread instance during construction. MockUiApplication is intentionally
 * different:
 * direct construction does not install it unless its constructor is asked to do
 * so. Use ScopedRegistration for temporary overrides and specialized tests.
 */
struct CRU_PLATFORM_GUI_API IUiApplication : public virtual IPlatformResource {
 public:
  class CRU_PLATFORM_GUI_API ScopedRegistration {
   public:
    explicit ScopedRegistration(IUiApplication* application);
    explicit ScopedRegistration(IUiApplication& application);

   private:
    IUiApplication* application_;
    IUiApplication* previous_;
    Guard guard_;
  };

  /**
   * @brief Get global IUiApplication instance associated with current thread.
   * @return The instance. May be nullptr if there is no one.
   */
  static IUiApplication* GetInstance();

  /**
   * @brief Get global IUiApplication instance associated with given thread.
   * @param thread_id The id of the thread.
   * @return The instance. May be nullptr if there is no one.
   *
   * If thread_id is invalid, nullptr will be returned also.
   */
  static IUiApplication* GetInstance(std::thread::id thread_id);

  /**
   * @brief Set global IUiApplication instance associated with current thread.
   * @param instance The instance to set. Can be nullptr.
   *
   * This api should be called carefully to avoid interleave with
   * IUiApplication::ScopedRegistration.
   */
  static void SetInstance(IUiApplication* instance);

 public:
  explicit IUiApplication(bool register_instance = true);

  // Block current thread and run the message loop. Return the exit code when
  // message loop gets a quit message (possibly posted by method RequestQuit).
  virtual int Run() = 0;

  // Post a quit message with given quit code.
  virtual void RequestQuit(int quit_code) = 0;

  virtual void AddOnQuitHandler(std::function<void()> handler) = 0;

  virtual bool IsQuitOnAllWindowClosed() = 0;
  virtual void SetQuitOnAllWindowClosed(bool quit_on_all_window_closed) = 0;

  // Timer id should always be positive (not 0) and never the same. So it's ok
  // to use negative value (or 0) to represent no timer.
  virtual long long SetImmediate(std::function<void()> action) = 0;
  virtual long long SetTimeout(std::chrono::milliseconds milliseconds,
                               std::function<void()> action) = 0;
  virtual long long SetInterval(std::chrono::milliseconds milliseconds,
                                std::function<void()> action) = 0;
  // Implementation should guarantee calls on timer id already canceled have no
  // effects and do not crash. Also canceling negative id or 0 should always
  // result in no-op.
  virtual void CancelTimer(long long id) = 0;

  virtual void DeleteLater(Object* object) = 0;

  virtual std::vector<INativeWindow*> GetAllWindow() = 0;

  virtual INativeWindow* CreateWindow() = 0;

  virtual cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() = 0;

  virtual ICursorManager* GetCursorManager() = 0;

  virtual IClipboard* GetClipboard() = 0;

  // If return nullptr, it means the menu is not supported.
  virtual IMenu* GetApplicationMenu();

  /**
   * \todo Implement on X11.
   */
  virtual std::optional<std::string> ShowSaveDialog(SaveDialogOptions options);

  /**
   * \todo Implement on X11.
   */
  virtual std::optional<std::vector<std::string>> ShowOpenDialog(
      OpenDialogOptions options);

 private:
  std::optional<ScopedRegistration> instance_registration_;
};

namespace details {
struct TimerCanceler {
  void operator()(long long id) {
    if (auto* application = IUiApplication::GetInstance()) {
      application->CancelTimer(id);
    }
  }
};
}  // namespace details

using TimerAutoCanceler = AutoDestruct<long long, details::TimerCanceler>;
}  // namespace cru::platform::gui
