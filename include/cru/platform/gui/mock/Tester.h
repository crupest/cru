#pragma once

#include "UiApplication.h"
#include "Window.h"

#include <cru/base/Base.h>

#include <cstddef>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace cru::platform::gui::mock {
// Thin pump/settle wrapper around MockUiApplication for tests that want a small
// facade but no input synthesis.
class CRU_PLATFORM_GUI_MOCK_API MockGuiTester : public Object {
 public:
  explicit MockGuiTester(MockUiApplication* application);
  explicit MockGuiTester(MockUiApplication& application);

  MockUiApplication* GetApplication() const { return application_; }

  bool Pump();
  void Settle(std::size_t max_iterations =
                  MockUiApplication::kDefaultMaxPumpIterations);
  bool WaitUntil(const std::function<bool()>& predicate,
                 std::size_t max_iterations =
                     MockUiApplication::kDefaultMaxPumpIterations);
  std::string GetLastDiagnostic() const;
  std::string GetEventLoopDiagnostic() const;

 private:
  MockUiApplication* application_;
};

// GUI-native user facade for common mock-window actions. Each action settles
// first, checks that the window belongs to the same MockUiApplication, verifies
// the window is created and visible, then delegates to MockWindow injection
// APIs. Control overloads use existing ControlHost focus/hover state for
// diagnostics; this facade is GUI-native rather than a browser/DOM locator or
// actionability model.
class CRU_PLATFORM_GUI_MOCK_API MockUser : public Object {
 public:
  explicit MockUser(MockUiApplication* application);
  explicit MockUser(MockUiApplication& application);

  MockUiApplication* GetApplication() const { return tester_.GetApplication(); }

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

  template <typename TControl>
  void Click(MockWindow& window, const Point& point, TControl* expected_target,
             MouseButton button = MouseButtons::Left,
             KeyModifier modifier = KeyModifiers::None);

  template <typename TControl>
  void TypeText(MockWindow& window, TControl* focus_target, std::string text);

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

  template <typename TControl>
  static std::string DescribeControl(TControl* control);
  template <typename TControl>
  static std::string DescribeControlHost(TControl* control, const Point* point);

  template <typename TControl>
  void EnsureTargetWindow(MockWindow& window, TControl* target,
                          std::string_view action) const;

  template <typename TControl>
  void AssertHoverTarget(MockWindow& window, const Point& point,
                         TControl* expected_target) const;

 private:
  MockGuiTester tester_;
};

template <typename TControl>
void MockUser::Click(MockWindow& window, const Point& point,
                     TControl* expected_target, MouseButton button,
                     KeyModifier modifier) {
  EnsureReadyForAction(window, &point, "Click");
  EnsureTargetWindow(window, expected_target, "Click");
  MoveMouseAfterActionability(window, point);
  AssertHoverTarget(window, point, expected_target);
  RaiseMouseButtonAfterActionability(window, point, button, modifier);
}

template <typename TControl>
void MockUser::TypeText(MockWindow& window, TControl* focus_target,
                        std::string text) {
  EnsureReadyForAction(window, nullptr, "TypeText");
  EnsureTargetWindow(window, focus_target, "TypeText");
  focus_target->SetFocus();
  window.InjectTextInput(std::move(text));
}

template <typename TControl>
std::string MockUser::DescribeControl(TControl* control) {
  if (control == nullptr) return "(null)";
  std::ostringstream stream;
  stream << control->GetDebugId();
  return stream.str();
}

template <typename TControl>
std::string MockUser::DescribeControlHost(TControl* control,
                                          const Point* point) {
  CRU_UNUSED(point)
  if (control == nullptr) return "target=(null)";
  auto* host = control->GetControlHost();
  if (host == nullptr) return "target_host=(null)";

  std::ostringstream stream;
  stream << "focus=" << DescribeControl(host->GetFocusControl())
         << ", hover=" << DescribeControl(host->GetMouseHoverControl())
         << ", capture=" << DescribeControl(host->GetMouseCaptureControl());
  return stream.str();
}

template <typename TControl>
void MockUser::EnsureTargetWindow(MockWindow& window, TControl* target,
                                  std::string_view action) const {
  if (target == nullptr) {
    throw Exception(std::string("MockUser ") + std::string(action) +
                    " requires a non-null target control. window={" +
                    DescribeWindow(window) + "}");
  }

  auto* host = target->GetControlHost();
  if (host == nullptr) {
    throw Exception(
        std::string("MockUser ") + std::string(action) +
        " target has no ControlHost. target=" + DescribeControl(target) +
        ", window={" + DescribeWindow(window) + "}");
  }

  if (host->GetNativeWindow() != &window) {
    throw Exception(std::string("MockUser ") + std::string(action) +
                    " target belongs to a different native window. target=" +
                    DescribeControl(target) + ", target_diagnostic={" +
                    DescribeControlHost(target, nullptr) + "}, window={" +
                    DescribeWindow(window) + "}");
  }
}

template <typename TControl>
void MockUser::AssertHoverTarget(MockWindow& window, const Point& point,
                                 TControl* expected_target) const {
  auto* hit_target = expected_target->GetControlHost()->GetMouseHoverControl();
  if (hit_target == expected_target) return;

  throw Exception(
      "MockUser Click target mismatch: point=" + DescribePoint(point) +
      ", expected=" + DescribeControl(expected_target) +
      ", hit=" + DescribeControl(hit_target) + ", target_diagnostic={" +
      DescribeControlHost(expected_target, &point) + "}, window={" +
      DescribeWindow(window) + "}");
}
}  // namespace cru::platform::gui::mock
