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
using cru::platform::gui::mock::MockUiApplicationFixture;
using cru::platform::gui::mock::MockWindow;

TEST_CASE("UI mock platform seam is compile visible", "[ui][mock]") {
  const MockResource resource;

  REQUIRE(std::string{MockResource::kPlatformId} == "Mock");
  REQUIRE(resource.GetPlatformId() == "Mock");
  REQUIRE(GetMockPlatformId() == "Mock");
}

TEST_CASE("Mock window injection drives ControlHost mouse routing",
          "[ui][mock][MockMouseInjection]") {
  MockUiApplicationFixture fixture;
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

  REQUIRE(mock_window->InjectFocus(cru::platform::gui::FocusChangeType::Gain));
  REQUIRE(root_focus_gain_count == 1);
  REQUIRE(mock_window->InjectMouseMove({10.f, 10.f}));
  REQUIRE(child.IsMouseOver());
  REQUIRE(mock_window->InjectMouseDown(cru::platform::gui::MouseButtons::Left,
                                       {10.f, 10.f}));
  REQUIRE(mock_window->InjectMouseDown(cru::platform::gui::MouseButtons::Left,
                                       {150.f, 150.f}));
  REQUIRE(child.CaptureMouse());
  REQUIRE(mock_window->InjectMouseUp(cru::platform::gui::MouseButtons::Left,
                                     {150.f, 150.f}));
  REQUIRE(mock_window->InjectMouseWheel(
      2.f, {10.f, 10.f}, cru::platform::gui::KeyModifiers::Ctrl, false));
  REQUIRE(mock_window->InjectMouseLeave());
  REQUIRE_FALSE(child.IsMouseOver());
  REQUIRE(mock_window->InjectFocus(cru::platform::gui::FocusChangeType::Lose));
  REQUIRE(root_focus_loss_count == 1);

  REQUIRE(child_events == std::vector<std::string>{"enter", "down",
                                                   "captured-up", "wheel",
                                                   "leave"});
}

TEST_CASE("Mock key text and IME injection drive ControlHost focus routing",
          "[ui][mock][MockKeyboardInjection][MockTextInput][MockIme]") {
  MockUiApplicationFixture fixture;
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

  mock_window->InjectKeyDown(cru::platform::gui::KeyCode::A,
                             cru::platform::gui::KeyModifiers::Ctrl |
                                 cru::platform::gui::KeyModifiers::Shift);
  mock_window->InjectKeyUp(cru::platform::gui::KeyCode::A,
                           cru::platform::gui::KeyModifiers::Alt);
  mock_window->InjectTextInput("plain");
  mock_window->InjectCompositionStart();
  mock_window->InjectCompositionUpdate("draft", {{0, 5, true}}, {5});
  mock_window->InjectCompositionCommit("done");

  REQUIRE(events == std::vector<std::string>{
                        "key-down", "key-up", "text:plain", "composition-start",
                        "composition:draft", "composition:", "text:done",
                        "composition-end"});
}
