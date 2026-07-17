#include "cru/platform/gui/mock/Tester.h"

#include <catch2/catch_test_macros.hpp>

#include <cru/base/Base.h>
#include <cru/platform/graphics/Brush.h>
#include <cru/platform/graphics/Factory.h>
#include <cru/platform/graphics/Geometry.h>
#include <cru/platform/graphics/Image.h>
#include <cru/platform/graphics/ImageFactory.h>
#include <cru/platform/graphics/TextLayout.h>
#include <cru/ui/controls/Window.h>
#include <cru/ui/render/CanvasRenderObject.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {
class FakePainter : public virtual cru::platform::graphics::IPainter {
 public:
  std::string GetPlatformId() const override { return "FakeGraphics"; }
  cru::platform::Matrix GetTransform() override { return transform_; }
  void SetTransform(const cru::platform::Matrix& matrix) override {
    transform_ = matrix;
  }
  void ConcatTransform(const cru::platform::Matrix& matrix) override {
    transform_ *= matrix;
  }
  void Clear(const cru::platform::Color&) override {}
  void DrawLine(const cru::platform::Point&, const cru::platform::Point&,
                cru::platform::graphics::IBrush*, float) override {}
  void StrokeRectangle(const cru::platform::Rect&,
                       cru::platform::graphics::IBrush*, float) override {}
  void FillRectangle(const cru::platform::Rect&,
                     cru::platform::graphics::IBrush*) override {}
  void StrokeEllipse(const cru::platform::Rect&,
                     cru::platform::graphics::IBrush*, float) override {}
  void FillEllipse(const cru::platform::Rect&,
                   cru::platform::graphics::IBrush*) override {}
  void StrokeGeometry(cru::platform::graphics::IGeometry*,
                      cru::platform::graphics::IBrush*, float) override {}
  void FillGeometry(cru::platform::graphics::IGeometry*,
                    cru::platform::graphics::IBrush*) override {}
  void DrawText(const cru::platform::Point&,
                cru::platform::graphics::ITextLayout*,
                cru::platform::graphics::IBrush*) override {}
  void DrawImage(const cru::platform::Point&,
                 cru::platform::graphics::IImage*) override {}
  void PushLayer(const cru::platform::Rect&) override {}
  void PopLayer() override {}
  void PushState() override {}
  void PopState() override {}
  void EndDraw() override {}

 private:
  cru::platform::Matrix transform_ = cru::platform::Matrix::Identity();
};

class FakeImage : public virtual cru::platform::graphics::IImage {
 public:
  FakeImage(cru::platform::graphics::IGraphicsFactory* graphics_factory,
            int width, int height)
      : graphics_factory_(graphics_factory), width_(width), height_(height) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() override {
    return graphics_factory_;
  }
  float GetWidth() override { return static_cast<float>(width_); }
  float GetHeight() override { return static_cast<float>(height_); }
  std::unique_ptr<cru::platform::graphics::IImage> CreateWithRect(
      const cru::platform::Rect& rect) override {
    return std::make_unique<FakeImage>(graphics_factory_,
                                       static_cast<int>(rect.width),
                                       static_cast<int>(rect.height));
  }
  std::unique_ptr<cru::platform::graphics::IPainter> CreatePainter() override {
    return std::make_unique<FakePainter>();
  }

 private:
  cru::platform::graphics::IGraphicsFactory* graphics_factory_;
  int width_;
  int height_;
};

class FakeImageFactory : public virtual cru::platform::graphics::IImageFactory {
 public:
  explicit FakeImageFactory(
      cru::platform::graphics::IGraphicsFactory* graphics_factory)
      : graphics_factory_(graphics_factory) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() override {
    return graphics_factory_;
  }
  std::unique_ptr<cru::platform::graphics::IImage> DecodeFromStream(
      cru::io::Stream*) override {
    return nullptr;
  }
  void EncodeToStream(cru::platform::graphics::IImage*, cru::io::Stream*,
                      cru::platform::graphics::ImageFormat, float) override {}
  std::unique_ptr<cru::platform::graphics::IImage> CreateBitmap(
      int width, int height) override {
    return std::make_unique<FakeImage>(graphics_factory_, width, height);
  }

 private:
  cru::platform::graphics::IGraphicsFactory* graphics_factory_;
};

class FakeGraphicsFactory
    : public virtual cru::platform::graphics::IGraphicsFactory {
 public:
  FakeGraphicsFactory() : image_factory_(this) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  std::unique_ptr<cru::platform::graphics::ISolidColorBrush>
  CreateSolidColorBrush() override {
    return nullptr;
  }
  std::unique_ptr<cru::platform::graphics::IGeometryBuilder>
  CreateGeometryBuilder() override {
    return nullptr;
  }
  std::unique_ptr<cru::platform::graphics::IFont> CreateFont(std::string,
                                                             float) override {
    return nullptr;
  }
  std::unique_ptr<cru::platform::graphics::ITextLayout> CreateTextLayout(
      std::shared_ptr<cru::platform::graphics::IFont>, std::string) override {
    return nullptr;
  }
  cru::platform::graphics::IImageFactory* GetImageFactory() override {
    return &image_factory_;
  }

 private:
  FakeImageFactory image_factory_;
};

class CanvasControl : public cru::ui::controls::Control {
 public:
  explicit CanvasControl(std::string name) : Control(std::move(name)) {
    render_object_.SetAttachedControl(this);
  }

  cru::ui::render::RenderObject* GetRenderObject() override {
    return &render_object_;
  }

 private:
  cru::ui::render::CanvasRenderObject render_object_;
};
}  // namespace

using cru::Exception;
using cru::platform::Point;
using cru::platform::gui::KeyCode;
using cru::platform::gui::KeyModifiers;
using cru::platform::gui::MouseButtons;
using cru::platform::gui::WindowVisibilityType;
using cru::platform::gui::mock::MockUiApplication;
using cru::platform::gui::mock::MockUser;
using cru::platform::gui::mock::MockWindow;

TEST_CASE("MockUserActions target click routes through ControlHost hit testing",
          "[ui][mock][MockUserActions][MockGuiTester]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  MockUser user(app);
  CanvasControl child("ClickableChild");
  cru::ui::controls::Window window;
  std::vector<std::string> child_events;

  window.AddChild(&child);
  window.GetNativeWindow()->SetClientSize({100.f, 100.f});
  window.GetNativeWindow()->SetVisibility(WindowVisibilityType::Show);
  window.GetControlHost()->RelayoutWithSize({100.f, 100.f});

  auto* mock_window = dynamic_cast<MockWindow*>(window.GetNativeWindow());
  REQUIRE(mock_window != nullptr);

  auto child_enter_revoker = child.MouseEnterEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseEventArgs&) {
        child_events.push_back("enter");
      });
  auto child_move_revoker = child.MouseMoveEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseEventArgs& args) {
        REQUIRE(args.GetPoint() == Point{10.f, 10.f});
        child_events.push_back("move");
      });
  auto child_down_revoker = child.MouseDownEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseButtonEventArgs& args) {
        REQUIRE(args.GetPoint() == Point{10.f, 10.f});
        REQUIRE(args.GetButton() == MouseButtons::Left);
        child_events.push_back("down");
      });
  auto child_up_revoker = child.MouseUpEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseButtonEventArgs& args) {
        REQUIRE(args.GetPoint() == Point{10.f, 10.f});
        REQUIRE(args.GetButton() == MouseButtons::Left);
        child_events.push_back("up");
      });

  user.Click(*mock_window, Point{10.f, 10.f}, &child);

  REQUIRE(child_events ==
          std::vector<std::string>{"enter", "move", "down", "up"});
}

TEST_CASE(
    "MockUserActions target mismatch reports point expected hit and window "
    "state",
    "[ui][mock][MockUserActions]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  MockUser user(app);
  CanvasControl child("actual-child");
  cru::ui::controls::Window window;
  int down_count = 0;

  window.AddChild(&child);
  window.GetNativeWindow()->SetClientSize({100.f, 100.f});
  window.GetNativeWindow()->SetVisibility(WindowVisibilityType::Show);
  window.GetControlHost()->RelayoutWithSize({100.f, 100.f});

  auto* mock_window = dynamic_cast<MockWindow*>(window.GetNativeWindow());
  REQUIRE(mock_window != nullptr);

  auto child_down_revoker = child.MouseDownEvent()->Direct()->AddHandler(
      [&](cru::ui::events::MouseButtonEventArgs&) { ++down_count; });

  try {
    user.Click(*mock_window, Point{10.f, 10.f}, &window);
    FAIL("Clicking with a mismatched expected target should fail.");
  } catch (const Exception& exception) {
    auto message = std::string(exception.what());
    REQUIRE(message.find("target mismatch") != std::string::npos);
    REQUIRE(message.find("point=(10, 10)") != std::string::npos);
    REQUIRE(message.find("expected=Window(") != std::string::npos);
    REQUIRE(message.find("hit=actual-child(") != std::string::npos);
    REQUIRE(message.find("visibility=Show") != std::string::npos);
  }

  REQUIRE(down_count == 0);
}

TEST_CASE(
    "MockUserActions type and key actions use focused native text and key "
    "route",
    "[ui][mock][MockUserActions][MockTextInput]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  MockUser user(app);
  CanvasControl child("TextTarget");
  cru::ui::controls::Window window;
  std::vector<std::string> events;

  window.AddChild(&child);
  window.GetNativeWindow()->SetClientSize({100.f, 100.f});
  window.GetNativeWindow()->SetVisibility(WindowVisibilityType::Show);
  window.GetControlHost()->RelayoutWithSize({100.f, 100.f});

  auto* mock_window = dynamic_cast<MockWindow*>(window.GetNativeWindow());
  REQUIRE(mock_window != nullptr);

  auto focus_revoker = child.GainFocusEvent()->Direct()->AddHandler(
      [&](cru::ui::events::FocusChangeEventArgs&) {
        events.push_back("focus");
      });
  auto text_revoker = child.TextInputEvent()->AddHandler(
      [&](const std::string& text) { events.push_back("text:" + text); });
  auto key_down_revoker = child.KeyDownEvent()->Direct()->AddHandler(
      [&](cru::ui::events::KeyEventArgs& args) {
        REQUIRE(args.GetKeyCode() == KeyCode::A);
        REQUIRE(args.GetKeyModifier() == KeyModifiers::Ctrl);
        events.push_back("key-down");
      });
  auto key_up_revoker = child.KeyUpEvent()->Direct()->AddHandler(
      [&](cru::ui::events::KeyEventArgs& args) {
        REQUIRE(args.GetKeyCode() == KeyCode::A);
        REQUIRE(args.GetKeyModifier() == KeyModifiers::Ctrl);
        events.push_back("key-up");
      });

  user.TypeText(*mock_window, &child, "typed");
  user.PressKey(*mock_window, KeyCode::A, KeyModifiers::Ctrl);

  REQUIRE(events == std::vector<std::string>{"focus", "text:typed", "key-down",
                                             "key-up"});
  REQUIRE(child.HasFocus());
}
