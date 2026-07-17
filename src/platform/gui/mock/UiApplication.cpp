#include "cru/platform/gui/mock/UiApplication.h"

#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/mock/Window.h"

#include <cru/base/Base.h>

#include <algorithm>
#include <cstddef>
#include <format>
#include <sstream>
#include <string_view>
#include <utility>

namespace cru::platform::gui::mock {
namespace {
MockUiApplication* RequireApplication(MockUiApplication* application,
                                      std::string_view owner) {
  if (application == nullptr) {
    throw Exception(
        std::format("{} requires a non-null MockUiApplication.", owner));
  }
  return application;
}

bool IsMouseVisibleWindow(MockWindow* window) {
  return window != nullptr && window->IsCreated() &&
         window->GetVisibility() == WindowVisibilityType::Show;
}
}  // namespace

MockUiApplication::MockUiApplication(
    graphics::IGraphicsFactory* graphics_factory, bool release_graphics_factory)
    : graphics_factory_(graphics_factory),
      release_graphics_factory_(release_graphics_factory),
      cursor_manager_(std::make_unique<MockCursorManager>()),
      clipboard_(std::make_unique<MockClipboard>()) {}

MockUiApplication::~MockUiApplication() {
  FlushDeleteLater();
  if (release_graphics_factory_) {
    delete graphics_factory_;
  }
}

int MockUiApplication::Run() {
  std::size_t iterations = 0;
  while (!quit_requested_ &&
         (PumpOnce() || !action_queue_.empty() || FindNextDueTimerIndex())) {
    ++iterations;
    if (iterations >= kDefaultMaxPumpIterations) {
      last_diagnostic_ = BuildEventLoopDiagnostic(
          "Run reached max pump iterations before quit",
          kDefaultMaxPumpIterations, iterations);
      throw Exception(last_diagnostic_);
    }
  }

  if (quit_requested_) {
    InvokePendingQuitHandlers();
  }
  FlushDeleteLater();
  return quit_requested_ ? quit_code_ : 0;
}

void MockUiApplication::RequestQuit(int quit_code) {
  quit_requested_ = true;
  quit_code_ = quit_code;
}

void MockUiApplication::AddOnQuitHandler(std::function<void()> handler) {
  quit_handlers_.push_back(std::move(handler));
}

bool MockUiApplication::IsQuitOnAllWindowClosed() {
  return is_quit_on_all_window_closed_;
}

void MockUiApplication::SetQuitOnAllWindowClosed(
    bool quit_on_all_window_closed) {
  is_quit_on_all_window_closed_ = quit_on_all_window_closed;
}

long long MockUiApplication::SetImmediate(std::function<void()> action) {
  auto id = TakeNextTimerId();
  action_queue_.push_back(QueuedAction{id, std::move(action)});
  return id;
}

long long MockUiApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                        std::function<void()> action) {
  if (milliseconds < std::chrono::milliseconds::zero()) {
    throw Exception("Timer interval can't be negative.");
  }

  auto id = TakeNextTimerId();
  timers_.push_back(Timer{id, milliseconds, current_time_ + milliseconds, false,
                          next_timer_order_++, std::move(action)});
  return id;
}

long long MockUiApplication::SetInterval(std::chrono::milliseconds milliseconds,
                                         std::function<void()> action) {
  if (milliseconds < std::chrono::milliseconds::zero()) {
    throw Exception("Timer interval can't be negative.");
  }
  if (milliseconds == std::chrono::milliseconds::zero()) {
    throw Exception("Repeat timer interval can't be 0.");
  }

  auto id = TakeNextTimerId();
  timers_.push_back(Timer{id, milliseconds, current_time_ + milliseconds, true,
                          next_timer_order_++, std::move(action)});
  return id;
}

void MockUiApplication::CancelTimer(long long id) {
  if (id <= 0) return;

  std::erase_if(action_queue_,
                [id](const QueuedAction& action) { return action.id == id; });
  std::erase_if(timers_, [id](const Timer& timer) { return timer.id == id; });
}

void MockUiApplication::DeleteLater(Object* object) {
  delete_later_pending_ = true;
  delete_later_pool_.Add(object);
}

std::vector<INativeWindow*> MockUiApplication::GetAllWindow() {
  return windows_;
}

INativeWindow* MockUiApplication::CreateWindow() { return CreateMockWindow(); }

MockWindow* MockUiApplication::CreateMockWindow() {
  return new MockWindow(this);
}

graphics::IGraphicsFactory* MockUiApplication::GetGraphicsFactory() {
  return graphics_factory_;
}

ICursorManager* MockUiApplication::GetCursorManager() {
  return cursor_manager_.get();
}

IClipboard* MockUiApplication::GetClipboard() { return clipboard_.get(); }

MockCursorManager* MockUiApplication::GetMockCursorManager() {
  return cursor_manager_.get();
}

MockClipboard* MockUiApplication::GetMockClipboard() {
  return clipboard_.get();
}

void MockUiApplication::SetDesktopRect(const Rect& rect) {
  desktop_rect_ = rect.Normalize();
  global_mouse_position_ = ClampToDesktop(global_mouse_position_);
}

void MockUiApplication::SetDesktopSize(const Size& size) {
  SetDesktopRect(Rect{desktop_rect_.GetLeftTop(), size.AtLeast0()});
}

void MockUiApplication::SetGlobalMousePosition(const Point& point) {
  global_mouse_position_ = ClampToDesktop(point);
}

bool MockUiApplication::MoveMouse(const Point& global_point) {
  SetGlobalMousePosition(global_point);

  if (!IsMouseVisibleWindow(captured_window_)) {
    captured_window_ = nullptr;
  }

  if (captured_window_ != nullptr) {
    if (!captured_window_->IsMouseInside()) {
      UpdateHoveredWindow(captured_window_);
    }
    return captured_window_->InjectMouseMove(
        captured_window_->GlobalToClient(global_mouse_position_));
  }

  auto* client_window = FindClientWindowAt(global_mouse_position_);
  UpdateHoveredWindow(client_window);
  if (client_window == nullptr) return false;

  return client_window->InjectMouseMove(
      client_window->GlobalToClient(global_mouse_position_));
}

bool MockUiApplication::MouseDown(MouseButton button, KeyModifier modifier) {
  auto* target = GetMouseTargetWindow();
  if (target == nullptr) {
    if (captured_window_ == nullptr) UpdateHoveredWindow(nullptr);
    return false;
  }

  if (target != captured_window_) UpdateHoveredWindow(target);
  return target->InjectMouseDown(
      button, target->GlobalToClient(global_mouse_position_), modifier);
}

bool MockUiApplication::MouseUp(MouseButton button, KeyModifier modifier) {
  auto* target = GetMouseTargetWindow();
  if (target == nullptr) {
    if (captured_window_ == nullptr) UpdateHoveredWindow(nullptr);
    return false;
  }

  if (target != captured_window_) UpdateHoveredWindow(target);
  return target->InjectMouseUp(
      button, target->GlobalToClient(global_mouse_position_), modifier);
}

bool MockUiApplication::MouseWheel(float delta, KeyModifier modifier,
                                   bool horizontal) {
  auto* target = GetMouseTargetWindow();
  if (target == nullptr) {
    if (captured_window_ == nullptr) UpdateHoveredWindow(nullptr);
    return false;
  }

  if (target != captured_window_) UpdateHoveredWindow(target);
  return target->InjectMouseWheel(
      delta, target->GlobalToClient(global_mouse_position_), modifier,
      horizontal);
}

bool MockUiApplication::Click(const Point& global_point, MouseButton button,
                              KeyModifier modifier) {
  MoveMouse(global_point);
  const auto down = MouseDown(button, modifier);
  const auto up = MouseUp(button, modifier);
  return down && up;
}

bool MockUiApplication::Drag(MockWindow& window, const Point& start_point,
                             const Point& end_point, MouseButton button,
                             KeyModifier modifier) {
  if (window.GetMockUiApplication() != this) {
    throw Exception(
        "MockUiApplication Drag received a window from a different "
        "MockUiApplication.");
  }
  if (!IsMouseVisibleWindow(&window)) return false;

  SetGlobalMousePosition(window.ClientToGlobal(start_point));
  UpdateHoveredWindow(&window);
  if (!window.IsMouseInside()) return false;
  const auto clamped_start_point = window.GetMousePosition();
  const auto move_start = window.InjectMouseMove(clamped_start_point);
  const auto down =
      window.InjectMouseDown(button, clamped_start_point, modifier);

  SetGlobalMousePosition(window.ClientToGlobal(end_point));
  const auto clamped_end_point = window.GetMousePosition();
  const auto move_end = window.InjectMouseMove(clamped_end_point);
  const auto up = window.InjectMouseUp(button, clamped_end_point, modifier);
  return move_start && down && move_end && up;
}

void MockUiApplication::AdvanceTimeBy(std::chrono::milliseconds milliseconds) {
  if (milliseconds < std::chrono::milliseconds::zero()) {
    throw Exception("Mock UI manual clock can't move backwards.");
  }

  current_time_ += milliseconds;
}

bool MockUiApplication::PumpOnce() {
  bool did_work = false;

  if (!action_queue_.empty()) {
    auto queued_action = std::move(action_queue_.front());
    action_queue_.pop_front();
    queued_action.action();
    did_work = true;
  } else if (auto timer_index = FindNextDueTimerIndex()) {
    auto action = timers_[*timer_index].action;

    if (timers_[*timer_index].repeat) {
      timers_[*timer_index].next_due += timers_[*timer_index].interval;
    } else {
      timers_.erase(timers_.begin() +
                    static_cast<std::ptrdiff_t>(*timer_index));
    }

    action();
    did_work = true;
  }

  return FlushDeleteLater() || did_work;
}

std::size_t MockUiApplication::PumpUntilIdle(std::size_t max_iterations) {
  std::size_t iterations = 0;

  while (!action_queue_.empty() || FindNextDueTimerIndex() ||
         delete_later_pending_) {
    if (iterations >= max_iterations) {
      last_diagnostic_ = BuildEventLoopDiagnostic(
          "PumpUntilIdle reached max iterations with ready work pending",
          max_iterations, iterations);
      throw Exception(last_diagnostic_);
    }

    PumpOnce();
    ++iterations;
  }

  if (HasRepeatingTimer()) {
    last_diagnostic_ = BuildEventLoopDiagnostic(
        "Settle stopped with pending repeating interval", max_iterations,
        iterations);
    throw Exception(last_diagnostic_);
  }

  last_diagnostic_.clear();
  return iterations;
}

void MockUiApplication::Settle(std::size_t max_iterations) {
  CRU_UNUSED(PumpUntilIdle(max_iterations));
}

bool MockUiApplication::WaitUntil(const std::function<bool()>& predicate,
                                  std::size_t max_iterations) {
  if (predicate()) {
    last_diagnostic_.clear();
    return true;
  }

  for (std::size_t iteration = 0; iteration < max_iterations; ++iteration) {
    if (!PumpOnce()) {
      if (predicate()) {
        last_diagnostic_.clear();
        return true;
      }
      last_diagnostic_ = BuildEventLoopDiagnostic(
          "WaitUntil stopped with predicate false and no ready work",
          max_iterations, iteration + 1);
      return false;
    }

    if (predicate()) {
      last_diagnostic_.clear();
      return true;
    }
  }

  last_diagnostic_ = BuildEventLoopDiagnostic(
      "WaitUntil reached max iterations with predicate false", max_iterations,
      max_iterations);
  return false;
}

std::string MockUiApplication::GetEventLoopDiagnostic() const {
  return BuildEventLoopDiagnostic("Mock UI event loop state");
}

long long MockUiApplication::TakeNextTimerId() { return next_timer_id_++; }

std::optional<std::size_t> MockUiApplication::FindNextDueTimerIndex() const {
  std::optional<std::size_t> result;
  for (std::size_t index = 0; index < timers_.size(); ++index) {
    const auto& timer = timers_[index];
    if (timer.next_due > current_time_) continue;

    if (!result || timer.next_due < timers_[*result].next_due ||
        (timer.next_due == timers_[*result].next_due &&
         timer.order < timers_[*result].order)) {
      result = index;
    }
  }

  return result;
}

bool MockUiApplication::FlushDeleteLater() {
  if (!delete_later_pending_) return false;

  delete_later_pending_ = false;
  delete_later_pool_.Clean();
  delete_later_pending_ = false;
  return true;
}

bool MockUiApplication::HasRepeatingTimer() const {
  return std::ranges::any_of(timers_,
                             [](const Timer& timer) { return timer.repeat; });
}

Point MockUiApplication::ClampToDesktop(const Point& point) const {
  const auto right = desktop_rect_.GetRight();
  const auto bottom = desktop_rect_.GetBottom();
  return {std::clamp(point.x, desktop_rect_.left, right),
          std::clamp(point.y, desktop_rect_.top, bottom)};
}

MockWindow* MockUiApplication::FindTopmostWindowAt(
    const Point& global_point) const {
  for (auto iterator = windows_.rbegin(); iterator != windows_.rend();
       ++iterator) {
    auto* window = FindKnownWindow(*iterator);
    if (!IsMouseVisibleWindow(window)) continue;
    if (window->IsGlobalPointInWindow(global_point)) return window;
  }
  return nullptr;
}

MockWindow* MockUiApplication::FindClientWindowAt(
    const Point& global_point) const {
  auto* window = FindTopmostWindowAt(global_point);
  if (window == nullptr || !window->IsGlobalPointInClient(global_point)) {
    return nullptr;
  }
  return window;
}

MockWindow* MockUiApplication::GetMouseTargetWindow() const {
  if (IsMouseVisibleWindow(captured_window_)) return captured_window_;
  return FindClientWindowAt(global_mouse_position_);
}

void MockUiApplication::RegisterWindow(MockWindow* window) {
  if (window == nullptr) return;
  if (!std::ranges::contains(window_instances_, window)) {
    window_instances_.push_back(window);
  }
}

void MockUiApplication::UnregisterWindow(MockWindow* window) {
  ClearMouseStateForWindow(window);
  std::erase(window_instances_, window);
  std::erase(windows_, static_cast<INativeWindow*>(window));
}

void MockUiApplication::RemoveCreatedWindow(MockWindow* window) {
  ClearMouseStateForWindow(window);
  std::erase(windows_, static_cast<INativeWindow*>(window));
}

MockWindow* MockUiApplication::FindKnownWindow(INativeWindow* window) const {
  auto iterator =
      std::ranges::find_if(window_instances_, [window](MockWindow* instance) {
        return static_cast<INativeWindow*>(instance) == window;
      });
  return iterator == window_instances_.end() ? nullptr : *iterator;
}

void MockUiApplication::UpdateHoveredWindow(MockWindow* window) {
  if (hovered_window_ == window) return;

  auto* old_hovered_window = hovered_window_;
  hovered_window_ = nullptr;
  if (old_hovered_window != nullptr && old_hovered_window->IsCreated()) {
    old_hovered_window->InjectMouseLeave();
  }

  if (window != nullptr && window->IsCreated()) {
    hovered_window_ = window;
    window->InjectMouseEnter();
  }
}

void MockUiApplication::SetHoveredWindowFromInjection(MockWindow* window) {
  hovered_window_ = window;
}

void MockUiApplication::ClearHoveredWindowFromInjection(MockWindow* window) {
  if (hovered_window_ == window) hovered_window_ = nullptr;
}

bool MockUiApplication::CaptureMouse(MockWindow* window) {
  if (!IsMouseVisibleWindow(window)) return false;

  if (captured_window_ != nullptr && captured_window_ != window) {
    captured_window_->has_mouse_capture_ = false;
  }
  captured_window_ = window;
  window->has_mouse_capture_ = true;
  return true;
}

bool MockUiApplication::ReleaseMouse(MockWindow* window) {
  if (window == nullptr || !window->IsCreated()) return false;

  if (captured_window_ == window) captured_window_ = nullptr;
  window->has_mouse_capture_ = false;
  return true;
}

void MockUiApplication::ClearMouseStateForWindow(MockWindow* window) {
  if (window == nullptr) return;
  if (hovered_window_ == window) hovered_window_ = nullptr;
  if (captured_window_ == window) captured_window_ = nullptr;
  window->has_mouse_capture_ = false;
  window->is_mouse_inside_ = false;
}

void MockUiApplication::BringWindowToForeground(MockWindow* window) {
  if (window == nullptr) return;
  std::erase(windows_, static_cast<INativeWindow*>(window));
  windows_.push_back(window);
}

std::string MockUiApplication::BuildEventLoopDiagnostic(
    std::string_view reason, std::optional<std::size_t> max_iterations,
    std::optional<std::size_t> iterations) const {
  std::ostringstream stream;
  stream << reason << ": current_time_ms=" << current_time_.count()
         << ", max_iterations=";
  if (max_iterations) {
    stream << *max_iterations;
  } else {
    stream << "(n/a)";
  }
  stream << ", iterations=";
  if (iterations) {
    stream << *iterations;
  } else {
    stream << "(n/a)";
  }
  stream << ", graphics_factory="
         << (graphics_factory_ == nullptr ? "null" : "present");
  if (graphics_factory_ != nullptr) {
    stream << ", image_factory="
           << (graphics_factory_->GetImageFactory() == nullptr ? "null"
                                                               : "present");
  }
  stream << ", windows=" << windows_.size() << ", desktop_rect=("
         << desktop_rect_.left << ", " << desktop_rect_.top << ", "
         << desktop_rect_.width << ", " << desktop_rect_.height << ")"
         << ", global_mouse_position=(" << global_mouse_position_.x << ", "
         << global_mouse_position_.y << ")"
         << ", hovered_window=" << hovered_window_
         << ", captured_window=" << captured_window_
         << ", queued_actions=" << action_queue_.size()
         << ", timers=" << timers_.size() << ", delete_later_pending="
         << (delete_later_pending_ ? "true" : "false")
         << ", quit_requested=" << (quit_requested_ ? "true" : "false");

  if (!action_queue_.empty()) {
    stream << ", first_action_id=" << action_queue_.front().id;
  }

  if (!timers_.empty()) {
    stream << ", timer_details=[";
    for (std::size_t index = 0; index < timers_.size(); ++index) {
      const auto& timer = timers_[index];
      if (index != 0) stream << "; ";
      stream << "id=" << timer.id << ",due_ms=" << timer.next_due.count()
             << ",interval_ms=" << timer.interval.count()
             << ",repeat=" << (timer.repeat ? "true" : "false");
    }
    stream << "]";
  }

  if (!windows_.empty()) {
    stream << ", window_details=[";
    for (std::size_t index = 0; index < windows_.size(); ++index) {
      if (index != 0) stream << "; ";
      auto* window = FindKnownWindow(windows_[index]);
      if (window == nullptr) {
        stream << "non_mock_window=" << windows_[index];
      } else {
        stream << "{" << window->GetDiagnostic() << "}";
      }
    }
    stream << "]";
  }

  return stream.str();
}

void MockUiApplication::InvokePendingQuitHandlers() {
  const auto handler_count = quit_handlers_.size();
  for (auto handler_index = invoked_quit_handler_count_;
       handler_index < handler_count; ++handler_index) {
    auto handler = quit_handlers_[handler_index];
    handler();
  }
  invoked_quit_handler_count_ = handler_count;
}

MockUser::MockUser(MockUiApplication* application)
    : application_(RequireApplication(application, "MockUser")) {}

MockUser::MockUser(MockUiApplication& application) : MockUser(&application) {}

bool MockUser::Pump() { return application_->PumpOnce(); }

void MockUser::Settle(std::size_t max_iterations) {
  application_->Settle(max_iterations);
}

bool MockUser::WaitUntil(const std::function<bool()>& predicate,
                         std::size_t max_iterations) {
  return application_->WaitUntil(predicate, max_iterations);
}

std::string MockUser::GetLastDiagnostic() const {
  return application_->GetLastDiagnostic();
}

std::string MockUser::GetEventLoopDiagnostic() const {
  return application_->GetEventLoopDiagnostic();
}

void MockUser::MoveMouse(MockWindow& window, const Point& point) {
  EnsureReadyForAction(window, &point, "MoveMouse");
  MoveMouseAfterActionability(window, point);
}

void MockUser::Click(MockWindow& window, const Point& point, MouseButton button,
                     KeyModifier modifier) {
  EnsureReadyForAction(window, &point, "Click");
  if (!application_->Click(window.ClientToGlobal(point), button, modifier)) {
    throw Exception(std::format(
        "MockUser Click failed to route global mouse input. window={{{}}}, "
        "point={}",
        DescribeWindow(window), point));
  }
}

void MockUser::Drag(MockWindow& window, const Point& start_point,
                    const Point& end_point, MouseButton button,
                    KeyModifier modifier) {
  EnsureReadyForAction(window, &start_point, "Drag");
  if (!application_->Drag(window, start_point, end_point, button, modifier)) {
    throw Exception(
        std::format("MockUser Drag failed to route window-scoped drag input. "
                    "window={{{}}}, start_point={}, end_point={}",
                    DescribeWindow(window), start_point, end_point));
  }
}

void MockUser::TypeText(MockWindow& window, std::string text) {
  EnsureReadyForAction(window, nullptr, "TypeText");
  window.InjectTextInput(std::move(text));
}

void MockUser::PressKey(MockWindow& window, KeyCode key, KeyModifier modifier) {
  EnsureReadyForAction(window, nullptr, "PressKey");
  window.InjectKeyDown(key, modifier);
  window.InjectKeyUp(key, modifier);
}

void MockUser::EnsureReadyForAction(MockWindow& window, const Point* point,
                                    std::string_view action) {
  try {
    application_->Settle();
  } catch (const Exception& exception) {
    throw Exception(std::format(
        "MockUser {} failed before action because the mock UI did not settle: "
        "{}",
        action, exception.what()));
  }

  EnsureWindowActionable(window, point, action);
}

void MockUser::EnsureWindowActionable(MockWindow& window, const Point* point,
                                      std::string_view action) const {
  if (window.GetMockUiApplication() != application_) {
    throw Exception(std::format(
        "MockUser {} received a window from a different MockUiApplication. "
        "window={{{}}}",
        action, DescribeWindow(window)));
  }

  if (!window.IsCreated() ||
      window.GetVisibility() != WindowVisibilityType::Show) {
    auto message = std::format(
        "MockUser {} requires a created visible mock window. window={{{}}}",
        action, DescribeWindow(window));
    if (point != nullptr) {
      message += std::format(", point={}", *point);
    }
    throw Exception(std::move(message));
  }
}

void MockUser::MoveMouseAfterActionability(MockWindow& window,
                                           const Point& point) {
  if (!application_->MoveMouse(window.ClientToGlobal(point))) {
    throw Exception(
        std::format("MockUser MoveMouse failed to route global mouse input. "
                    "window={{{}}}, point={}",
                    DescribeWindow(window), point));
  }
}

std::string MockUser::DescribeWindow(const MockWindow& window) {
  auto& mutable_window = const_cast<MockWindow&>(window);

  std::ostringstream stream;
  stream << mutable_window.GetDiagnostic() << ", app_diagnostic={"
         << mutable_window.GetMockUiApplication()->GetEventLoopDiagnostic()
         << "}";
  return stream.str();
}

}  // namespace cru::platform::gui::mock
