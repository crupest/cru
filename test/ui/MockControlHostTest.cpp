#include "MockTestGraphics.h"

#include "cru/base/io/MemoryStream.h"
#include "cru/platform/gui/mock/UiApplication.h"
#include "cru/platform/gui/mock/Window.h"
#include "cru/ui/controls/Window.h"
#include "cru/ui/render/CanvasRenderObject.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
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
  cru::ui::render::CanvasRenderObject* GetCanvasRenderObject() {
    return &render_object_;
  }

 private:
  cru::ui::render::CanvasRenderObject render_object_;
};

cru::platform::gui::mock::MockWindow* GetMockWindow(
    cru::ui::controls::Window* window) {
  return dynamic_cast<cru::platform::gui::mock::MockWindow*>(
      window->GetNativeWindow());
}
}  // namespace

using cru::platform::gui::mock::MockUiApplicationFixture;
using cru::test::ui::mock::FakeGraphicsFactory;
using cru::test::ui::mock::kPngSignature;

TEST_CASE("Mock ControlHost relayout repaints hosted canvas into snapshot",
          "[ui][mock][MockControlHost][MockRepaint][MockSnapshot]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplicationFixture fixture(&graphics_factory);
  auto* app = fixture.GetApplication();
  CanvasControl canvas;
  cru::ui::controls::Window window;
  int after_layout_count = 0;
  int canvas_paint_count = 0;

  window.AddChild(&canvas);
  window.GetNativeWindow()->SetClientSize({120.f, 80.f});
  window.GetNativeWindow()->SetVisibility(
      cru::platform::gui::WindowVisibilityType::Show);
  auto* mock_window = GetMockWindow(&window);
  REQUIRE(mock_window != nullptr);
  REQUIRE(mock_window->IsCreated());
  REQUIRE(app->GetAllWindow() ==
          std::vector<cru::platform::gui::INativeWindow*>{mock_window});

  auto after_layout_revoker =
      window.GetControlHost()->AfterLayoutEvent()->AddSpyOnlyHandler(
          [&] { ++after_layout_count; });
  auto paint_revoker = canvas.GetCanvasRenderObject()->PaintEvent()->AddHandler(
      [&](const cru::ui::render::CanvasPaintEventArgs& args) {
        REQUIRE(args.GetPainter() != nullptr);
        REQUIRE(args.GetPaintSize() == cru::platform::Size{120.f, 80.f});
        args.GetPainter()->FillRectangle({0.f, 0.f, 10.f, 10.f}, nullptr);
        ++canvas_paint_count;
      });

  window.GetControlHost()->ScheduleRelayout();
  REQUIRE_FALSE(mock_window->HasPendingRepaint());
  REQUIRE(after_layout_count == 0);
  REQUIRE(app->PumpOnce());
  REQUIRE(after_layout_count == 1);
  REQUIRE(mock_window->HasPendingRepaint());
  REQUIRE(canvas_paint_count == 0);

  app->Settle();

  REQUIRE_FALSE(mock_window->HasPendingRepaint());
  REQUIRE(canvas_paint_count == 1);
  REQUIRE(graphics_factory.GetPainterCount() == 1);
  REQUIRE(graphics_factory.GetClearCount() == 1);
  REQUIRE(graphics_factory.GetFillCount() == 1);
  REQUIRE(graphics_factory.GetEndDrawCount() == 1);
  REQUIRE(mock_window->GetSnapshotImage()->GetWidth() == 120.f);
  REQUIRE(mock_window->GetSnapshotImage()->GetHeight() == 80.f);

  std::array<std::byte, 16> buffer{};
  cru::io::MemoryStream stream(buffer.data(), buffer.size());
  mock_window->EncodeSnapshotToStream(&stream);
  REQUIRE(std::equal(kPngSignature.cbegin(), kPngSignature.cend(),
                     buffer.cbegin()));
  REQUIRE(graphics_factory.GetFakeImageFactory()->GetLastFormat() ==
          cru::platform::graphics::ImageFormat::Png);
  REQUIRE(graphics_factory.GetFakeImageFactory()->GetLastQuality() == 1.f);
}

TEST_CASE("Mock ControlHost routes input and settle drains timer invalidation",
          "[ui][mock][MockControlHost][MockInput][MockTextInput][MockSettle]") {
  using namespace std::chrono_literals;

  FakeGraphicsFactory graphics_factory;
  MockUiApplicationFixture fixture(&graphics_factory);
  auto* app = fixture.GetApplication();
  CanvasControl canvas;
  cru::ui::controls::Window window;
  std::vector<std::string> events;
  int paint_count = 0;
  int timer_count = 0;

  window.AddChild(&canvas);
  window.GetNativeWindow()->SetClientSize({100.f, 60.f});
  window.GetNativeWindow()->SetVisibility(
      cru::platform::gui::WindowVisibilityType::Show);
  window.GetControlHost()->RelayoutWithSize({100.f, 60.f});
  auto* mock_window = GetMockWindow(&window);
  REQUIRE(mock_window != nullptr);
  app->Settle();

  auto mouse_down_revoker = canvas.MouseDownEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseButtonEventArgs& args) {
        REQUIRE(args.GetPoint() == cru::platform::Point{5.f, 5.f});
        events.push_back("mouse-down");
      });
  auto key_down_revoker = canvas.KeyDownEvent()->Direct()->AddHandler(
      [&](cru::ui::events::KeyEventArgs& args) {
        REQUIRE(args.GetKeyCode() == cru::platform::gui::KeyCode::A);
        events.push_back("key-down");
      });
  auto text_revoker = canvas.TextInputEvent()->AddHandler(
      [&](const std::string& text) { events.push_back("text:" + text); });
  auto paint_revoker = canvas.GetCanvasRenderObject()->PaintEvent()->AddHandler(
      [&](const cru::ui::render::CanvasPaintEventArgs&) { ++paint_count; });

  canvas.SetFocus();
  REQUIRE(canvas.HasFocus());
  REQUIRE(mock_window->InjectMouseMove({5.f, 5.f}));
  REQUIRE(mock_window->InjectMouseDown(cru::platform::gui::MouseButtons::Left,
                                       {5.f, 5.f}));
  mock_window->InjectKeyDown(cru::platform::gui::KeyCode::A);
  mock_window->InjectTextInput("typed");

  REQUIRE(events ==
          std::vector<std::string>{"mouse-down", "key-down", "text:typed"});

  app->SetTimeout(0ms, [&] {
    ++timer_count;
    canvas.GetCanvasRenderObject()->InvalidatePaint();
  });
  REQUIRE(timer_count == 0);
  REQUIRE_FALSE(mock_window->HasPendingRepaint());

  app->Settle();

  REQUIRE(timer_count == 1);
  REQUIRE(paint_count == 1);
  REQUIRE_FALSE(mock_window->HasPendingRepaint());
}
