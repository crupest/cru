#include "cru/platform/gui/mock/UiApplication.h"

#include <catch2/catch_test_macros.hpp>

#include <cru/base/Base.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

using cru::Exception;
using cru::platform::Point;
using cru::platform::gui::KeyCode;
using cru::platform::gui::KeyModifiers;
using cru::platform::gui::MouseButtons;
using cru::platform::gui::MouseEnterLeaveType;
using cru::platform::gui::NativeMouseButtonEventArgs;
using cru::platform::gui::WindowVisibilityType;
using cru::platform::gui::mock::MockUiApplication;
using cru::platform::gui::mock::MockUser;
using cru::platform::gui::mock::MockWindow;

TEST_CASE("MockUser click emits native enter move down up in order",
          "[platform][gui][mock][user][MockUser]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  MockUser user(app);
  std::vector<std::string> events;

  auto enter_revoker =
      window->MouseEnterLeaveEvent()->AddHandler([&](MouseEnterLeaveType type) {
        if (type == MouseEnterLeaveType::Enter) events.push_back("enter");
      });
  auto move_revoker =
      window->MouseMoveEvent()->AddHandler([&](const Point& point) {
        REQUIRE(point == Point{10.f, 20.f});
        events.push_back("move");
      });
  auto down_revoker = window->MouseDownEvent()->AddHandler(
      [&](const NativeMouseButtonEventArgs& args) {
        REQUIRE(args.point == Point{10.f, 20.f});
        REQUIRE(args.button == MouseButtons::Left);
        REQUIRE(args.modifier == KeyModifiers::Ctrl);
        events.push_back("down");
      });
  auto up_revoker = window->MouseUpEvent()->AddHandler(
      [&](const NativeMouseButtonEventArgs& args) {
        REQUIRE(args.point == Point{10.f, 20.f});
        REQUIRE(args.button == MouseButtons::Left);
        REQUIRE(args.modifier == KeyModifiers::Ctrl);
        events.push_back("up");
      });

  window->SetClientSize({100.f, 100.f});
  window->SetVisibility(WindowVisibilityType::Show);

  user.Click(*window, Point{10.f, 20.f}, MouseButtons::Left,
             KeyModifiers::Ctrl);

  REQUIRE(events == std::vector<std::string>{"enter", "move", "down", "up"});
  REQUIRE(window->IsMouseInside());
  REQUIRE(window->GetMousePosition() == Point{10.f, 20.f});
}

TEST_CASE("MockUser keeps low-level injection available for uncovered cases",
          "[platform][gui][mock][user][MockUser]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  MockUser user(app);
  std::vector<std::string> events;

  auto down_revoker = window->MouseDownEvent()->AddHandler(
      [&](const NativeMouseButtonEventArgs& args) {
        events.push_back(args.button == MouseButtons::Right ? "right-down"
                                                            : "other-down");
      });
  auto key_down_revoker = window->KeyDownEvent()->AddHandler(
      [&](const cru::platform::gui::NativeKeyEventArgs& args) {
        events.push_back(args.key == KeyCode::A ? "key-a" : "other-key");
      });

  window->SetVisibility(WindowVisibilityType::Show);

  user.PressKey(*window, KeyCode::A, KeyModifiers::Shift);
  REQUIRE(window->InjectMouseDown(MouseButtons::Right, Point{1.f, 2.f}));

  REQUIRE(events == std::vector<std::string>{"key-a", "right-down"});
}

TEST_CASE(
    "MockDiagnostics ActionabilityDiagnostics report hidden and unsettled "
    "windows",
    "[platform][gui][mock][user][MockUser][WaitUntil]"
    "[MockDiagnostics][ActionabilityDiagnostics][SettleDiagnostics]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  MockUser user(app);

  try {
    user.Click(*window, Point{1.f, 2.f});
    FAIL("Clicking an uncreated window should fail actionability.");
  } catch (const Exception& exception) {
    auto message = std::string(exception.what());
    REQUIRE(message.find("created=false") != std::string::npos);
    REQUIRE(message.find("visibility=Hide") != std::string::npos);
    REQUIRE(message.find("pending_repaint=false") != std::string::npos);
    REQUIRE(message.find("paint_count=0") != std::string::npos);
    REQUIRE(message.find("last_injected_event=(none)") != std::string::npos);
    REQUIRE(message.find("point=(1, 2)") != std::string::npos);
    REQUIRE(message.find("app_diagnostic={Mock UI event loop state") !=
            std::string::npos);
  }

  window->SetVisibility(WindowVisibilityType::Show);
  app.SetInterval(std::chrono::milliseconds(1), [] {});

  try {
    user.MoveMouse(*window, Point{3.f, 4.f});
    FAIL("Actions should fail before input when settle reports pending work.");
  } catch (const Exception& exception) {
    auto message = std::string(exception.what());
    REQUIRE(message.find("did not settle") != std::string::npos);
    REQUIRE(message.find("pending repeating interval") != std::string::npos);
    REQUIRE(message.find("timer_details=[") != std::string::npos);
    REQUIRE(message.find("repeat=true") != std::string::npos);
  }
}
