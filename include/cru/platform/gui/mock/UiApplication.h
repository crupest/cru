#pragma once

#include "Base.h"
#include "Clipboard.h"
#include "Cursor.h"
#include "Window.h"

#include <cru/platform/gui/UiApplication.h>

#include <chrono>
#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cru::platform::gui::mock {
class MockWindow;

/**
 * @brief Test-only GUI application for code that needs an IUiApplication
 * thread-local singleton without starting a native backend.
 *
 * Direct construction installs this object as the current thread's
 * IUiApplication singleton for its lifetime, matching native application
 * backends without leaking state across test threads.
 *
 * Create MockUiApplication before code that needs
 * IUiApplication::GetInstance(). Native-window-only tests can call
 * CreateMockWindow directly. Typical native window flow:
 *
 * @code
 * FakeGraphicsFactory graphics_factory;
 * MockUiApplication app(&graphics_factory);
 * auto* window = app.CreateMockWindow();
 * window->SetClientSize({100.f, 60.f});
 * window->SetVisibility(WindowVisibilityType::Show);
 * window->InjectMouseDown(MouseButtons::Left, {5.f, 5.f});
 * app.Settle();
 * @endcode
 *
 * Pass a real or test double graphics::IGraphicsFactory when painting,
 * snapshots, or EncodeSnapshotToStream are needed. The mock owns the factory
 * only when release_graphics_factory is true; otherwise the caller must keep it
 * alive for the mock lifetime. A test can omit the factory only for paths that
 * do not paint or snapshot.
 *
 * PumpOnce executes at most one ready queued action/timer and flushes
 * DeleteLater work. AdvanceTimeBy moves the manual clock for timeouts. Settle
 * drains ready work with a bounded iteration count and throws if work cannot
 * become idle, including uncanceled repeating intervals. WaitUntil also uses a
 * bounded pump loop, returns false when no ready work can satisfy the
 * predicate, and records GetLastDiagnostic/GetEventLoopDiagnostic state for
 * assertions.
 *
 * Non-goals: this mock is not a native backend replacement, production
 * bootstrap selector, screenshot baseline manager, image-diff/update workflow,
 * or browser/DOM-style locator/actionability layer.
 */
class CRU_PLATFORM_GUI_MOCK_API MockUiApplication
    : public MockResource,
      public virtual IUiApplication {
 public:
  explicit MockUiApplication(
      graphics::IGraphicsFactory* graphics_factory = nullptr,
      bool release_graphics_factory = false);
  ~MockUiApplication() override;

  static constexpr std::size_t kDefaultMaxPumpIterations = 1000;

  int Run() override;

  void RequestQuit(int quit_code) override;

  void AddOnQuitHandler(std::function<void()> handler) override;

  bool IsQuitOnAllWindowClosed() override;
  void SetQuitOnAllWindowClosed(bool quit_on_all_window_closed) override;

  long long SetImmediate(std::function<void()> action) override;
  long long SetTimeout(std::chrono::milliseconds milliseconds,
                       std::function<void()> action) override;
  long long SetInterval(std::chrono::milliseconds milliseconds,
                        std::function<void()> action) override;
  void CancelTimer(long long id) override;

  void DeleteLater(Object* object) override;

  std::vector<INativeWindow*> GetAllWindow() override;
  INativeWindow* CreateWindow() override;
  MockWindow* CreateMockWindow();

  graphics::IGraphicsFactory* GetGraphicsFactory() override;

  ICursorManager* GetCursorManager() override;
  IClipboard* GetClipboard() override;

  MockCursorManager* GetMockCursorManager();
  MockClipboard* GetMockClipboard();

  bool HasQuitRequest() const { return quit_requested_; }
  int GetRequestedQuitCode() const { return quit_code_; }

  std::chrono::milliseconds GetCurrentTime() const { return current_time_; }
  void AdvanceTimeBy(std::chrono::milliseconds milliseconds);

  bool PumpOnce();
  std::size_t PumpUntilIdle(
      std::size_t max_iterations = kDefaultMaxPumpIterations);
  void Settle(std::size_t max_iterations = kDefaultMaxPumpIterations);
  bool WaitUntil(const std::function<bool()>& predicate,
                 std::size_t max_iterations = kDefaultMaxPumpIterations);
  std::string GetEventLoopDiagnostic() const;
  const std::string& GetLastDiagnostic() const { return last_diagnostic_; }

  void RegisterWindow(INativeWindow* window);
  void UnregisterWindow(INativeWindow* window);

 private:
  struct QueuedAction {
    long long id;
    std::function<void()> action;
  };

  struct Timer {
    long long id;
    std::chrono::milliseconds interval;
    std::chrono::milliseconds next_due;
    bool repeat;
    std::size_t order;
    std::function<void()> action;
  };

  long long TakeNextTimerId();
  std::optional<std::size_t> FindNextDueTimerIndex() const;
  bool FlushDeleteLater();
  bool HasRepeatingTimer() const;
  std::string BuildEventLoopDiagnostic(
      std::string_view reason,
      std::optional<std::size_t> max_iterations = std::nullopt,
      std::optional<std::size_t> iterations = std::nullopt) const;
  void InvokePendingQuitHandlers();

 private:
  graphics::IGraphicsFactory* graphics_factory_;
  bool release_graphics_factory_;

  bool quit_requested_ = false;
  int quit_code_ = 0;
  std::vector<std::function<void()>> quit_handlers_;
  std::size_t invoked_quit_handler_count_ = 0;

  bool is_quit_on_all_window_closed_ = true;
  std::vector<INativeWindow*> windows_;

  long long next_timer_id_ = 1;
  std::size_t next_timer_order_ = 0;
  std::chrono::milliseconds current_time_ = std::chrono::milliseconds::zero();
  std::deque<QueuedAction> action_queue_;
  std::vector<Timer> timers_;
  std::string last_diagnostic_;

  bool delete_later_pending_ = false;
  DeleteLaterPool delete_later_pool_;
  std::unique_ptr<MockCursorManager> cursor_manager_;
  std::unique_ptr<MockClipboard> clipboard_;
};

/**
 * @brief GUI-native user facade for common mock-window actions.
 *
 * Each action settles first, checks that the window belongs to the same
 * MockUiApplication, verifies the window is created and visible, then delegates
 * to MockWindow injection APIs. This facade is GUI-native rather than a
 * browser/DOM locator or actionability model.
 */
class CRU_PLATFORM_GUI_MOCK_API MockUser : public Object {
 public:
  explicit MockUser(MockUiApplication* application);
  explicit MockUser(MockUiApplication& application);

  MockUiApplication* GetApplication() const { return application_; }

  bool Pump();
  void Settle(std::size_t max_iterations =
                  MockUiApplication::kDefaultMaxPumpIterations);
  bool WaitUntil(const std::function<bool()>& predicate,
                 std::size_t max_iterations =
                     MockUiApplication::kDefaultMaxPumpIterations);
  std::string GetLastDiagnostic() const;
  std::string GetEventLoopDiagnostic() const;

  void MoveMouse(MockWindow& window, const Point& point);
  void Click(MockWindow& window, const Point& point,
             MouseButton button = MouseButtons::Left,
             KeyModifier modifier = KeyModifiers::None);
  void TypeText(MockWindow& window, std::string text);
  void PressKey(MockWindow& window, KeyCode key,
                KeyModifier modifier = KeyModifiers::None);

 private:
  void EnsureReadyForAction(MockWindow& window, const Point* point,
                            std::string_view action);
  void EnsureWindowActionable(MockWindow& window, const Point* point,
                              std::string_view action) const;
  void MoveMouseAfterActionability(MockWindow& window, const Point& point);
  void RaiseMouseButtonAfterActionability(MockWindow& window,
                                          const Point& point,
                                          MouseButton button,
                                          KeyModifier modifier);
  static std::string DescribePoint(const Point& point);
  static std::string DescribeWindow(const MockWindow& window);

 private:
  MockUiApplication* application_;
};

}  // namespace cru::platform::gui::mock
