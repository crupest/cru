#include "cru/platform/gui/mock/Tester.h"

#include <cru/base/Base.h>

#include <sstream>
#include <string>
#include <string_view>

namespace cru::platform::gui::mock {
namespace {
const char* ToString(WindowVisibilityType visibility) {
  switch (visibility) {
    case WindowVisibilityType::Hide:
      return "Hide";
    case WindowVisibilityType::Show:
      return "Show";
    case WindowVisibilityType::Minimize:
      return "Minimize";
    default:
      return "Unknown";
  }
}

MockUiApplication* RequireApplication(MockUiApplication* application,
                                      std::string_view owner) {
  if (application == nullptr) {
    throw Exception(std::string(owner) +
                    " requires a non-null MockUiApplication.");
  }
  return application;
}
}  // namespace

MockGuiTester::MockGuiTester(MockUiApplication* application)
    : application_(RequireApplication(application, "MockGuiTester")) {}

MockGuiTester::MockGuiTester(MockUiApplication& application)
    : MockGuiTester(&application) {}

bool MockGuiTester::Pump() { return application_->PumpOnce(); }

void MockGuiTester::Settle(std::size_t max_iterations) {
  application_->Settle(max_iterations);
}

bool MockGuiTester::WaitUntil(const std::function<bool()>& predicate,
                              std::size_t max_iterations) {
  return application_->WaitUntil(predicate, max_iterations);
}

std::string MockGuiTester::GetLastDiagnostic() const {
  return application_->GetLastDiagnostic();
}

std::string MockGuiTester::GetEventLoopDiagnostic() const {
  return application_->GetEventLoopDiagnostic();
}

MockUser::MockUser(MockUiApplication* application) : tester_(application) {}

MockUser::MockUser(MockUiApplication& application) : MockUser(&application) {}

bool MockUser::Pump() { return tester_.Pump(); }

void MockUser::Settle(std::size_t max_iterations) {
  tester_.Settle(max_iterations);
}

bool MockUser::WaitUntil(const std::function<bool()>& predicate,
                         std::size_t max_iterations) {
  return tester_.WaitUntil(predicate, max_iterations);
}

std::string MockUser::GetLastDiagnostic() const {
  return tester_.GetLastDiagnostic();
}

std::string MockUser::GetEventLoopDiagnostic() const {
  return tester_.GetEventLoopDiagnostic();
}

void MockUser::MoveMouse(MockWindow& window, const Point& point) {
  EnsureReadyForAction(window, &point, "MoveMouse");
  MoveMouseAfterActionability(window, point);
}

void MockUser::Click(MockWindow& window, const Point& point, MouseButton button,
                     KeyModifier modifier) {
  EnsureReadyForAction(window, &point, "Click");
  MoveMouseAfterActionability(window, point);
  RaiseMouseButtonAfterActionability(window, point, button, modifier);
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
    tester_.Settle();
  } catch (const Exception& exception) {
    throw Exception(std::string("MockUser ") + std::string(action) +
                    " failed before action because the mock UI did not "
                    "settle: " +
                    exception.what());
  }

  EnsureWindowActionable(window, point, action);
}

void MockUser::EnsureWindowActionable(MockWindow& window, const Point* point,
                                      std::string_view action) const {
  if (window.GetMockUiApplication() != tester_.GetApplication()) {
    throw Exception(std::string("MockUser ") + std::string(action) +
                    " received a window from a different MockUiApplication. "
                    "window={" +
                    DescribeWindow(window) + "}");
  }

  if (!window.IsCreated() ||
      window.GetVisibility() != WindowVisibilityType::Show) {
    auto message = std::string("MockUser ") + std::string(action) +
                   " requires a created visible mock window. window={" +
                   DescribeWindow(window) + "}";
    if (point != nullptr) message += ", point=" + DescribePoint(*point);
    throw Exception(std::move(message));
  }
}

void MockUser::MoveMouseAfterActionability(MockWindow& window,
                                           const Point& point) {
  if (!window.IsMouseInside() && !window.InjectMouseEnter()) {
    throw Exception(
        "MockUser MoveMouse failed to inject mouse enter. window={" +
        DescribeWindow(window) + "}, point=" + DescribePoint(point));
  }

  if (!window.InjectMouseMove(point)) {
    throw Exception("MockUser MoveMouse failed to inject mouse move. window={" +
                    DescribeWindow(window) +
                    "}, point=" + DescribePoint(point));
  }
}

void MockUser::RaiseMouseButtonAfterActionability(MockWindow& window,
                                                  const Point& point,
                                                  MouseButton button,
                                                  KeyModifier modifier) {
  if (!window.InjectMouseDown(button, point, modifier)) {
    throw Exception("MockUser Click failed to inject mouse down. window={" +
                    DescribeWindow(window) +
                    "}, point=" + DescribePoint(point));
  }
  if (!window.InjectMouseUp(button, point, modifier)) {
    throw Exception("MockUser Click failed to inject mouse up. window={" +
                    DescribeWindow(window) +
                    "}, point=" + DescribePoint(point));
  }
}

std::string MockUser::DescribePoint(const Point& point) {
  std::ostringstream stream;
  stream << "(" << point.x << ", " << point.y << ")";
  return stream.str();
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
