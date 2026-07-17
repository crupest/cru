#include "cru/platform/gui/mock/Base.h"
#include "cru/platform/gui/mock/UiApplication.h"
#include "cru/platform/gui/mock/Window.h"
#include "cru/ui/controls/Window.h"
#include "cru/ui/render/CanvasRenderObject.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

namespace {
class CanvasControl : public cru::ui::controls::Control {
 public:
  CanvasControl() : Control("CanvasControl") {
    render_object_.SetAttachedControl(this);
  }

  cru::ui::render::RenderObject* GetRenderObject() override {
    return &render_object_;
  }

 private:
  cru::ui::render::CanvasRenderObject render_object_;
};
}  // namespace

using cru::platform::gui::mock::GetMockPlatformId;
using cru::platform::gui::mock::MockResource;
using cru::platform::gui::mock::MockUiApplication;
using cru::platform::gui::mock::MockWindow;

TEST_CASE("UI mock platform seam is compile visible", "[ui][mock]") {
  const MockResource resource;

  REQUIRE(std::string{MockResource::kPlatformId} == "Mock");
  REQUIRE(resource.GetPlatformId() == "Mock");
  REQUIRE(GetMockPlatformId() == "Mock");
}

TEST_CASE("Mock application mouse actions drive ControlHost mouse routing",
          "[ui][mock][MockMouseInput]") {
  MockUiApplication app;
  CanvasControl child;
  cru::ui::controls::Window window;
  std::vector<std::string> child_events;
  int root_focus_gain_count = 0;
  int root_focus_loss_count = 0;

  window.AddChild(&child);
  window.GetNativeWindow()->SetClientSize({100.f, 100.f});
  window.GetNativeWindow()->SetVisibility(
      cru::platform::gui::WindowVisibilityType::Show);
  window.GetControlHost()->RelayoutWithSize({100.f, 100.f});

  auto* mock_window = dynamic_cast<MockWindow*>(window.GetNativeWindow());
  REQUIRE(mock_window != nullptr);

  auto child_enter_revoker = child.MouseEnterEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseEventArgs&) {
        child_events.push_back("enter");
      });
  auto child_leave_revoker = child.MouseLeaveEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseEventArgs&) {
        child_events.push_back("leave");
      });
  auto child_down_revoker = child.MouseDownEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseButtonEventArgs& args) {
        REQUIRE(args.GetPoint() == cru::platform::Point{10.f, 10.f});
        REQUIRE(args.GetButton() == cru::platform::gui::MouseButtons::Left);
        child_events.push_back("down");
      });
  auto child_up_revoker = child.MouseUpEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseButtonEventArgs& args) {
        REQUIRE(args.GetPoint() == cru::platform::Point{150.f, 150.f});
        REQUIRE(args.GetButton() == cru::platform::gui::MouseButtons::Left);
        child_events.push_back("captured-up");
      });
  auto child_wheel_revoker = child.MouseWheelEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseWheelEventArgs& args) {
        REQUIRE(args.GetDelta() == 2.f);
        REQUIRE(args.GetKeyModifier() ==
                cru::platform::gui::KeyModifiers::Ctrl);
        child_events.push_back("wheel");
      });
  auto root_gain_revoker = window.GainFocusEvent()->Direct()->AddHandler(
      [&](cru::ui::events::FocusChangeEventArgs&) { ++root_focus_gain_count; });
  auto root_loss_revoker = window.LoseFocusEvent()->Direct()->AddHandler(
      [&](cru::ui::events::FocusChangeEventArgs&) { ++root_focus_loss_count; });

  REQUIRE(app.FocusWindow(mock_window));
  REQUIRE(root_focus_gain_count == 1);
  REQUIRE(app.MoveMouse({10.f, 10.f}));
  REQUIRE(child.IsMouseOver());
  REQUIRE(app.MouseDown(cru::platform::gui::MouseButtons::Left));
  REQUIRE_FALSE(app.MoveMouse({150.f, 150.f}));
  REQUIRE_FALSE(app.MouseDown(cru::platform::gui::MouseButtons::Left));
  REQUIRE(child.CaptureMouse());
  REQUIRE(app.MouseUp(cru::platform::gui::MouseButtons::Left));
  REQUIRE(app.MoveMouse({10.f, 10.f}));
  REQUIRE(app.MouseWheel(2.f, cru::platform::gui::KeyModifiers::Ctrl, false));
  REQUIRE(child.ReleaseMouse());
  REQUIRE_FALSE(app.MoveMouse({150.f, 150.f}));
  REQUIRE_FALSE(child.IsMouseOver());
  mock_window->SetVisibility(cru::platform::gui::WindowVisibilityType::Hide);
  REQUIRE(root_focus_loss_count == 1);

  REQUIRE(child_events == std::vector<std::string>{"enter", "down", "leave",
                                                   "captured-up", "enter",
                                                   "wheel", "leave"});
}

TEST_CASE("Mock key text and IME injection drive ControlHost focus routing",
          "[ui][mock][MockKeyboardInjection][MockTextInput][MockIme]") {
  MockUiApplication app;
  CanvasControl child;
  cru::ui::controls::Window window;
  std::vector<std::string> events;

  window.AddChild(&child);
  window.GetNativeWindow()->SetClientSize({100.f, 100.f});
  window.GetNativeWindow()->SetVisibility(
      cru::platform::gui::WindowVisibilityType::Show);
  window.GetControlHost()->RelayoutWithSize({100.f, 100.f});
  child.SetFocus();

  auto* mock_window = dynamic_cast<MockWindow*>(window.GetNativeWindow());
  REQUIRE(mock_window != nullptr);
  REQUIRE(child.HasFocus());
  REQUIRE(app.FocusWindow(mock_window));

  auto key_down_revoker = child.KeyDownEvent()->Direct()->AddHandler(
      [&](cru::ui::events::KeyEventArgs& args) {
        REQUIRE(args.GetKeyCode() == cru::platform::gui::KeyCode::A);
        REQUIRE(args.GetKeyModifier() ==
                (cru::platform::gui::KeyModifiers::Ctrl |
                 cru::platform::gui::KeyModifiers::Shift));
        events.push_back("key-down");
      });
  auto key_up_revoker = child.KeyUpEvent()->Direct()->AddHandler(
      [&](cru::ui::events::KeyEventArgs& args) {
        REQUIRE(args.GetKeyCode() == cru::platform::gui::KeyCode::A);
        REQUIRE(args.GetKeyModifier() == cru::platform::gui::KeyModifiers::Alt);
        events.push_back("key-up");
      });
  auto text_revoker = child.TextInputEvent()->AddHandler(
      [&](const std::string& text) { events.push_back("text:" + text); });
  auto composition_start_revoker =
      child.CompositionStartEvent()->AddSpyOnlyHandler(
          [&] { events.push_back("composition-start"); });
  auto composition_revoker = child.CompositionEvent()->AddHandler(
      [&](const cru::platform::gui::CompositionText& composition) {
        events.push_back("composition:" + composition.text);
      });
  auto composition_end_revoker = child.CompositionEndEvent()->AddSpyOnlyHandler(
      [&] { events.push_back("composition-end"); });

  REQUIRE(app.KeyDown(cru::platform::gui::KeyCode::A,
                      cru::platform::gui::KeyModifiers::Ctrl |
                          cru::platform::gui::KeyModifiers::Shift));
  REQUIRE(app.KeyUp(cru::platform::gui::KeyCode::A,
                    cru::platform::gui::KeyModifiers::Alt));
  REQUIRE(app.TextInput("plain"));
  mock_window->InjectCompositionStart();
  mock_window->InjectCompositionUpdate("draft", {{0, 5, true}}, {5});
  mock_window->InjectCompositionCommit("done");

  REQUIRE(events == std::vector<std::string>{
                        "key-down", "key-up", "text:plain", "composition-start",
                        "composition:draft", "composition:", "text:done",
                        "composition-end"});
}
