#include "cru/platform/gui/mock/UiApplication.h"
#include "cru/platform/gui/mock/Window.h"

#include <catch2/catch_test_macros.hpp>

#include <cru/base/Base.h>
#include <cru/base/io/MemoryStream.h>
#include <cru/platform/graphics/Brush.h>
#include <cru/platform/graphics/Factory.h>
#include <cru/platform/graphics/Geometry.h>
#include <cru/platform/graphics/Image.h>
#include <cru/platform/graphics/ImageFactory.h>
#include <cru/platform/graphics/TextLayout.h>

#include <array>
#include <cstddef>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace {
constexpr std::array<std::byte, 8> kPngSignature{
    std::byte{0x89}, std::byte{0x50}, std::byte{0x4E}, std::byte{0x47},
    std::byte{0x0D}, std::byte{0x0A}, std::byte{0x1A}, std::byte{0x0A}};

class FakePainter : public virtual cru::platform::graphics::IPainter {
 public:
  FakePainter(int* clear_count, int* end_draw_count)
      : clear_count_(clear_count), end_draw_count_(end_draw_count) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }

  cru::platform::Matrix GetTransform() override { return transform_; }
  void SetTransform(const cru::platform::Matrix& matrix) override {
    transform_ = matrix;
  }
  void ConcatTransform(const cru::platform::Matrix& matrix) override {
    transform_ *= matrix;
  }

  void Clear(const cru::platform::Color&) override { ++*clear_count_; }
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
  void EndDraw() override { ++*end_draw_count_; }

 private:
  cru::platform::Matrix transform_ = cru::platform::Matrix::Identity();
  int* clear_count_;
  int* end_draw_count_;
};

class FakeImage : public virtual cru::platform::graphics::IImage {
 public:
  FakeImage(cru::platform::graphics::IGraphicsFactory* graphics_factory,
            int width, int height, int* painter_count, int* clear_count,
            int* end_draw_count)
      : graphics_factory_(graphics_factory),
        width_(width),
        height_(height),
        painter_count_(painter_count),
        clear_count_(clear_count),
        end_draw_count_(end_draw_count) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() override {
    return graphics_factory_;
  }
  float GetWidth() override { return static_cast<float>(width_); }
  float GetHeight() override { return static_cast<float>(height_); }
  std::unique_ptr<cru::platform::graphics::IImage> CreateWithRect(
      const cru::platform::Rect& rect) override {
    return std::make_unique<FakeImage>(
        graphics_factory_, static_cast<int>(rect.width),
        static_cast<int>(rect.height), painter_count_, clear_count_,
        end_draw_count_);
  }
  std::unique_ptr<cru::platform::graphics::IPainter> CreatePainter() override {
    ++*painter_count_;
    return std::make_unique<FakePainter>(clear_count_, end_draw_count_);
  }

 private:
  cru::platform::graphics::IGraphicsFactory* graphics_factory_;
  int width_;
  int height_;
  int* painter_count_;
  int* clear_count_;
  int* end_draw_count_;
};

class FakeImageFactory : public virtual cru::platform::graphics::IImageFactory {
 public:
  FakeImageFactory(cru::platform::graphics::IGraphicsFactory* graphics_factory,
                   std::vector<cru::platform::Size>* bitmap_sizes,
                   int* painter_count, int* clear_count, int* end_draw_count)
      : graphics_factory_(graphics_factory),
        bitmap_sizes_(bitmap_sizes),
        painter_count_(painter_count),
        clear_count_(clear_count),
        end_draw_count_(end_draw_count) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() override {
    return graphics_factory_;
  }
  std::unique_ptr<cru::platform::graphics::IImage> DecodeFromStream(
      cru::io::Stream*) override;
  void EncodeToStream(cru::platform::graphics::IImage*, cru::io::Stream*,
                      cru::platform::graphics::ImageFormat, float) override;
  std::unique_ptr<cru::platform::graphics::IImage> CreateBitmap(
      int width, int height) override {
    bitmap_sizes_->push_back(
        {static_cast<float>(width), static_cast<float>(height)});
    return std::make_unique<FakeImage>(graphics_factory_, width, height,
                                       painter_count_, clear_count_,
                                       end_draw_count_);
  }
  int GetEncodeCount() const { return encode_count_; }
  cru::platform::graphics::IImage* GetLastEncodedImage() const {
    return last_encoded_image_;
  }
  cru::platform::graphics::ImageFormat GetLastFormat() const {
    return last_format_;
  }
  float GetLastQuality() const { return last_quality_; }

 private:
  cru::platform::graphics::IGraphicsFactory* graphics_factory_;
  std::vector<cru::platform::Size>* bitmap_sizes_;
  cru::platform::Size last_encoded_size_{};
  int* painter_count_;
  int* clear_count_;
  int* end_draw_count_;
  int encode_count_ = 0;
  cru::platform::graphics::IImage* last_encoded_image_ = nullptr;
  cru::platform::graphics::ImageFormat last_format_ =
      cru::platform::graphics::ImageFormat::Png;
  float last_quality_ = 0.f;
};

std::unique_ptr<cru::platform::graphics::IImage>
FakeImageFactory::DecodeFromStream(cru::io::Stream* stream) {
  std::array<std::byte, kPngSignature.size()> signature{};
  stream->Read(signature.data(), signature.size());
  if (signature != kPngSignature) {
    return nullptr;
  }

  return std::make_unique<FakeImage>(
      graphics_factory_, static_cast<int>(last_encoded_size_.width),
      static_cast<int>(last_encoded_size_.height), painter_count_, clear_count_,
      end_draw_count_);
}

void FakeImageFactory::EncodeToStream(
    cru::platform::graphics::IImage* image, cru::io::Stream* stream,
    cru::platform::graphics::ImageFormat format, float quality) {
  ++encode_count_;
  last_encoded_image_ = image;
  last_format_ = format;
  last_quality_ = quality;
  last_encoded_size_ = {image->GetWidth(), image->GetHeight()};
  stream->Write(kPngSignature.data(), kPngSignature.size());
}

class FakeGraphicsFactory
    : public virtual cru::platform::graphics::IGraphicsFactory {
 public:
  FakeGraphicsFactory()
      : image_factory_(this, &bitmap_sizes_, &painter_count_, &clear_count_,
                       &end_draw_count_) {}

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

  const std::vector<cru::platform::Size>& GetBitmapSizes() const {
    return bitmap_sizes_;
  }
  int GetPainterCount() const { return painter_count_; }
  int GetClearCount() const { return clear_count_; }
  int GetEndDrawCount() const { return end_draw_count_; }
  FakeImageFactory* GetFakeImageFactory() { return &image_factory_; }

 private:
  std::vector<cru::platform::Size> bitmap_sizes_;
  int painter_count_ = 0;
  int clear_count_ = 0;
  int end_draw_count_ = 0;
  FakeImageFactory image_factory_;
};

class NoImageGraphicsFactory
    : public virtual cru::platform::graphics::IGraphicsFactory {
 public:
  std::string GetPlatformId() const override { return "NoImageGraphics"; }
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
    return nullptr;
  }
};
}  // namespace

using cru::Exception;
using cru::platform::Point;
using cru::platform::Rect;
using cru::platform::Size;
using cru::platform::Thickness;
using cru::platform::colors::white;
using cru::platform::gui::FocusChangeType;
using cru::platform::gui::INativeWindow;
using cru::platform::gui::KeyModifiers;
using cru::platform::gui::MouseButtons;
using cru::platform::gui::MouseEnterLeaveType;
using cru::platform::gui::NativeMouseButtonEventArgs;
using cru::platform::gui::NativeMouseWheelEventArgs;
using cru::platform::gui::WindowStyleFlags;
using cru::platform::gui::WindowVisibilityType;
using cru::platform::gui::mock::MockCursorManager;
using cru::platform::gui::mock::MockInputMethodContext;
using cru::platform::gui::mock::MockUiApplication;
using cru::platform::gui::mock::MockWindow;

TEST_CASE("Mock windows start hidden and uncreated",
          "[platform][gui][mock][window][MockWindowLifecycle]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());

  REQUIRE_FALSE(window->IsCreated());
  REQUIRE(window->GetVisibility() == WindowVisibilityType::Hide);
  REQUIRE(window->GetParent() == nullptr);
  REQUIRE(window->GetStyleFlag() == cru::platform::gui::WindowStyleFlag{});
  REQUIRE(window->GetTitle().empty());
  REQUIRE(window->GetClientRect() == Rect{});
  REQUIRE(window->GetWindowRect() == Rect{});
  REQUIRE(window->GetClientSize() == Size{});
  REQUIRE(window->GetMousePosition() == Point{});
  REQUIRE(window->GetCursor() == nullptr);
  REQUIRE(window->GetInputMethodContext() ==
          static_cast<MockInputMethodContext*>(
              window->GetMockInputMethodContext()));
  REQUIRE(app.GetCreatedWindows().empty());
  REQUIRE_FALSE(window->HasPendingRepaint());

  window->RequestRepaint();
  REQUIRE_FALSE(window->HasPendingRepaint());
}

TEST_CASE("Mock window show creates before visibility change",
          "[platform][gui][mock][window][MockWindowLifecycle]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<std::string> events;

  auto create_revoker = window->CreateEvent()->AddSpyOnlyHandler([&] {
    REQUIRE(window->IsCreated());
    REQUIRE(window->GetVisibility() == WindowVisibilityType::Hide);
    REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{window.get()});
    events.push_back("create");
  });
  auto visibility_revoker = window->VisibilityChangeEvent()->AddHandler(
      [&](WindowVisibilityType visibility) {
        REQUIRE(window->GetVisibility() == visibility);
        events.push_back(visibility == WindowVisibilityType::Show ? "show"
                                                                  : "other");
      });

  window->SetVisibility(WindowVisibilityType::Show);

  REQUIRE(events == std::vector<std::string>{"create", "show"});
  REQUIRE(window->IsCreated());
  REQUIRE(window->GetVisibility() == WindowVisibilityType::Show);
  REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{window.get()});

  window->SetVisibility(WindowVisibilityType::Show);
  REQUIRE(events == std::vector<std::string>{"create", "show"});
}

TEST_CASE("Mock window hide and minimize keep native state created",
          "[platform][gui][mock][window][MockWindowLifecycle]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<WindowVisibilityType> visibility_events;
  auto visibility_revoker = window->VisibilityChangeEvent()->AddHandler(
      [&](WindowVisibilityType visibility) {
        visibility_events.push_back(visibility);
      });

  window->SetVisibility(WindowVisibilityType::Show);
  visibility_events.clear();

  window->SetVisibility(WindowVisibilityType::Hide);
  REQUIRE(window->IsCreated());
  REQUIRE(window->GetVisibility() == WindowVisibilityType::Hide);
  REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{window.get()});

  window->SetVisibility(WindowVisibilityType::Minimize);
  REQUIRE(window->IsCreated());
  REQUIRE(window->GetVisibility() == WindowVisibilityType::Minimize);
  REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{window.get()});

  REQUIRE(visibility_events ==
          std::vector<WindowVisibilityType>{WindowVisibilityType::Hide,
                                            WindowVisibilityType::Minimize});
}

TEST_CASE(
    "Mock window minimize before show updates visibility without creating",
    "[platform][gui][mock][window][MockWindowLifecycle]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<WindowVisibilityType> visibility_events;
  std::vector<std::string> create_order;
  auto visibility_revoker = window->VisibilityChangeEvent()->AddHandler(
      [&](WindowVisibilityType visibility) {
        visibility_events.push_back(visibility);
      });
  auto create_revoker = window->CreateEvent()->AddSpyOnlyHandler(
      [&] { create_order.push_back("create"); });

  window->SetVisibility(WindowVisibilityType::Minimize);
  REQUIRE_FALSE(window->IsCreated());
  REQUIRE(window->GetVisibility() == WindowVisibilityType::Minimize);
  REQUIRE(app.GetCreatedWindows().empty());

  window->SetVisibility(WindowVisibilityType::Hide);
  REQUIRE_FALSE(window->IsCreated());
  REQUIRE(window->GetVisibility() == WindowVisibilityType::Hide);
  REQUIRE(app.GetCreatedWindows().empty());

  window->SetVisibility(WindowVisibilityType::Show);
  REQUIRE(window->IsCreated());
  REQUIRE(window->GetVisibility() == WindowVisibilityType::Show);
  REQUIRE(create_order == std::vector<std::string>{"create"});
  REQUIRE(visibility_events ==
          std::vector<WindowVisibilityType>{WindowVisibilityType::Minimize,
                                            WindowVisibilityType::Hide,
                                            WindowVisibilityType::Show});
}

TEST_CASE("Mock window resize events observe updated client size",
          "[platform][gui][mock][window][MockWindowState]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<Size> resize_args;
  std::vector<Size> observed_sizes;

  auto resize_revoker =
      window->ResizeEvent()->AddHandler([&](const Size& size) {
        resize_args.push_back(size);
        observed_sizes.push_back(window->GetClientSize());
      });

  window->SetClientRect(Rect{10.f, 20.f, 100.f, 50.f});
  window->SetClientSize(Size{120.f, 60.f});
  REQUIRE(resize_args.empty());
  REQUIRE(window->GetClientRect() == Rect{10.f, 20.f, 120.f, 60.f});

  window->SetVisibility(WindowVisibilityType::Show);
  REQUIRE(resize_args.empty());

  window->SetClientSize(Size{140.f, 70.f});
  window->SetClientSize(Size{140.f, 70.f});
  window->SetClientRect(Rect{30.f, 40.f, 140.f, 70.f});
  window->SetWindowRect(Rect{50.f, 60.f, 160.f, 90.f});

  REQUIRE(resize_args ==
          std::vector<Size>{Size{140.f, 70.f}, Size{144.f, 52.f}});
  REQUIRE(observed_sizes == resize_args);
  REQUIRE(window->GetClientRect() == Rect{58.f, 90.f, 144.f, 52.f});
  REQUIRE(window->GetWindowRect() == Rect{50.f, 60.f, 160.f, 90.f});

  window->Close();
  window->SetClientSize(Size{10.f, 20.f});
  REQUIRE(resize_args.size() == 2);
  REQUIRE(window->GetClientSize() == Size{10.f, 20.f});
}

TEST_CASE("Mock window derives client and window rects from border",
          "[platform][gui][mock][window][MockWindowState]"
          "[MockWindowBorder]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());

  REQUIRE(window->GetBorderSize() == Thickness{8.f, 30.f, 8.f, 8.f});
  REQUIRE(window->GetEffectiveBorderSize() == Thickness{8.f, 30.f, 8.f, 8.f});

  window->SetClientRect(Rect{100.f, 120.f, 300.f, 200.f});
  REQUIRE(window->GetClientRect() == Rect{100.f, 120.f, 300.f, 200.f});
  REQUIRE(window->GetWindowRect() == Rect{92.f, 90.f, 316.f, 238.f});

  window->SetBorderSize(Thickness{2.f, 3.f, 4.f, 5.f});
  REQUIRE(window->GetBorderSize() == Thickness{2.f, 3.f, 4.f, 5.f});
  REQUIRE(window->GetWindowRect() == Rect{98.f, 117.f, 306.f, 208.f});

  window->SetWindowRect(Rect{10.f, 20.f, 50.f, 60.f});
  REQUIRE(window->GetWindowRect() == Rect{10.f, 20.f, 50.f, 60.f});
  REQUIRE(window->GetClientRect() == Rect{12.f, 23.f, 44.f, 52.f});

  window->SetStyleFlag(WindowStyleFlags::NoCaptionAndBorder);
  REQUIRE(window->GetEffectiveBorderSize() == Thickness{});
  REQUIRE(window->GetWindowRect() == window->GetClientRect());

  window->SetClientRect(Rect{1.f, 2.f, 30.f, 40.f});
  REQUIRE(window->GetClientRect() == Rect{1.f, 2.f, 30.f, 40.f});
  REQUIRE(window->GetWindowRect() == Rect{1.f, 2.f, 30.f, 40.f});
}

TEST_CASE("Mock application tracks global mouse on a desktop canvas",
          "[platform][gui][mock][window][MockMouseInput]"
          "[MockDesktopCanvas]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> first(app.CreateMockWindow());
  std::unique_ptr<MockWindow> second(app.CreateMockWindow());

  app.SetDesktopSize(Size{500.f, 500.f});
  first->SetClientRect(Rect{10.f, 20.f, 100.f, 100.f});
  second->SetClientRect(Rect{200.f, 50.f, 100.f, 100.f});
  first->SetVisibility(WindowVisibilityType::Show);
  second->SetVisibility(WindowVisibilityType::Show);

  REQUIRE(app.MoveMouse(Point{15.f, 25.f}));
  REQUIRE(app.GetMousePosition() == Point{15.f, 25.f});
  REQUIRE(first->GetMousePosition() == Point{5.f, 5.f});
  REQUIRE(second->GetMousePosition() == Point{-185.f, -25.f});

  REQUIRE(app.MoveMouse(second->ClientToScreen(Point{7.f, 8.f})));
  REQUIRE(app.GetMousePosition() == Point{207.f, 58.f});
  REQUIRE(first->GetMousePosition() == Point{197.f, 38.f});

  app.MoveMouse(Point{1000.f, -5.f});
  REQUIRE(app.GetMousePosition() == Point{500.f, 0.f});
}

TEST_CASE("Mock application routes global mouse by z-order and border hit",
          "[platform][gui][mock][window][MockMouseInput]"
          "[MockDesktopCanvas]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> bottom(app.CreateMockWindow());
  std::unique_ptr<MockWindow> top(app.CreateMockWindow());
  std::vector<std::string> events;

  app.SetDesktopSize(Size{300.f, 300.f});
  bottom->SetClientRect(Rect{20.f, 20.f, 100.f, 100.f});
  top->SetClientRect(Rect{40.f, 40.f, 100.f, 100.f});
  bottom->SetVisibility(WindowVisibilityType::Show);
  top->SetVisibility(WindowVisibilityType::Show);

  auto bottom_enter_leave_revoker =
      bottom->MouseEnterLeaveEvent()->AddHandler([&](MouseEnterLeaveType type) {
        events.push_back(type == MouseEnterLeaveType::Enter ? "bottom-enter"
                                                            : "bottom-leave");
      });
  auto bottom_move_revoker =
      bottom->MouseMoveEvent()->AddHandler([&](const Point& point) {
        events.push_back(std::format("bottom-move:{}:{}", point.x, point.y));
      });
  auto top_enter_leave_revoker =
      top->MouseEnterLeaveEvent()->AddHandler([&](MouseEnterLeaveType type) {
        events.push_back(type == MouseEnterLeaveType::Enter ? "top-enter"
                                                            : "top-leave");
      });
  auto top_move_revoker =
      top->MouseMoveEvent()->AddHandler([&](const Point& point) {
        events.push_back(std::format("top-move:{}:{}", point.x, point.y));
      });

  REQUIRE(app.MoveMouse(Point{50.f, 50.f}));
  bottom->SetToForeground();
  REQUIRE(app.MoveMouse(Point{50.f, 50.f}));
  REQUIRE_FALSE(app.MoveMouse(Point{13.f, 0.f}));

  REQUIRE(events == std::vector<std::string>{
                        "top-enter", "top-move:10:10", "top-leave",
                        "bottom-enter", "bottom-move:30:30", "bottom-leave"});
  REQUIRE_FALSE(top->IsMouseInside());
  REQUIRE_FALSE(bottom->IsMouseInside());
}

TEST_CASE("Mock application routes captured mouse outside client",
          "[platform][gui][mock][window][MockMouseInput]"
          "[MockDesktopCanvas]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<Point> move_points;
  std::vector<MouseEnterLeaveType> enter_leave_events;

  app.SetDesktopSize(Size{100.f, 100.f});
  window->SetClientRect(Rect{10.f, 10.f, 30.f, 30.f});
  window->SetVisibility(WindowVisibilityType::Show);

  auto move_revoker = window->MouseMoveEvent()->AddHandler(
      [&](const Point& point) { move_points.push_back(point); });
  auto enter_leave_revoker = window->MouseEnterLeaveEvent()->AddHandler(
      [&](MouseEnterLeaveType type) { enter_leave_events.push_back(type); });

  REQUIRE(app.MoveMouse(Point{15.f, 15.f}));
  REQUIRE(window->CaptureMouse());
  REQUIRE(app.MoveMouse(Point{99.f, 99.f}));
  REQUIRE(window->GetMousePosition() == Point{89.f, 89.f});
  REQUIRE(app.GetCapturedWindow() == window.get());
  REQUIRE(window->HasMouseCapture());

  REQUIRE(window->ReleaseMouse());
  REQUIRE_FALSE(window->HasMouseCapture());
  REQUIRE(app.GetCapturedWindow() == nullptr);
  REQUIRE_FALSE(app.MoveMouse(Point{99.f, 99.f}));

  REQUIRE(move_points ==
          std::vector<Point>{Point{5.f, 5.f}, Point{89.f, 89.f}});
  REQUIRE(enter_leave_events ==
          std::vector<MouseEnterLeaveType>{MouseEnterLeaveType::Enter,
                                           MouseEnterLeaveType::Leave});
}

TEST_CASE("Mock window close destroys once and unregisters",
          "[platform][gui][mock][window][MockWindowLifecycle]"
          "[QuitOnAllWindowsClosed]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> first(app.CreateMockWindow());
  std::unique_ptr<MockWindow> second(app.CreateMockWindow());
  std::vector<std::string> events;

  auto first_destroy_revoker = first->DestroyEvent()->AddSpyOnlyHandler([&] {
    REQUIRE(first->IsCreated());
    REQUIRE(app.GetCreatedWindows().size() == 2);
    events.push_back("first-destroy");
    first->Close();
  });
  auto second_destroy_revoker = second->DestroyEvent()->AddSpyOnlyHandler([&] {
    REQUIRE(second->IsCreated());
    REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{second.get()});
    events.push_back("second-destroy");
  });

  first->SetVisibility(WindowVisibilityType::Show);
  second->SetVisibility(WindowVisibilityType::Show);
  REQUIRE(app.GetCreatedWindows() ==
          std::vector<MockWindow*>{first.get(), second.get()});

  REQUIRE(first->RequestFocus());
  REQUIRE(first->CaptureMouse());
  first->Close();

  REQUIRE(events == std::vector<std::string>{"first-destroy"});
  REQUIRE_FALSE(first->IsCreated());
  REQUIRE_FALSE(first->HasFocus());
  REQUIRE_FALSE(first->HasMouseCapture());
  REQUIRE(first->GetVisibility() == WindowVisibilityType::Hide);
  REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{second.get()});
  REQUIRE_FALSE(app.HasQuitRequest());

  first->Close();
  REQUIRE(events == std::vector<std::string>{"first-destroy"});
  REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{second.get()});

  second->Close();
  REQUIRE(events ==
          std::vector<std::string>{"first-destroy", "second-destroy"});
  REQUIRE(app.GetCreatedWindows().empty());
  REQUIRE(app.HasQuitRequest());
  REQUIRE(app.GetRequestedQuitCode() == 0);
}

TEST_CASE("Mock window state round-trips without native APIs",
          "[platform][gui][mock][window][MockWindowState]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> parent(app.CreateMockWindow());
  std::unique_ptr<MockWindow> child(app.CreateMockWindow());
  MockCursorManager cursor_manager;
  auto arrow = cursor_manager.GetSystemCursor(
      cru::platform::gui::SystemCursorType::Arrow);
  auto hand = cursor_manager.GetSystemCursor(
      cru::platform::gui::SystemCursorType::Hand);
  std::vector<FocusChangeType> focus_events;
  auto focus_revoker = child->FocusEvent()->AddHandler(
      [&](FocusChangeType change) { focus_events.push_back(change); });

  child->SetParent(parent.get());
  child->SetStyleFlag(WindowStyleFlags::NoCaptionAndBorder);
  child->SetTitle("mock child");
  child->SetClientRect(Rect{1.f, 2.f, 30.f, 40.f});
  child->SetWindowRect(Rect{3.f, 4.f, 50.f, 60.f});
  app.MoveMouse(child->ClientToScreen(Point{7.f, 8.f}));
  child->SetCursor(arrow);

  REQUIRE(child->GetParent() == parent.get());
  REQUIRE(child->GetStyleFlag() == WindowStyleFlags::NoCaptionAndBorder);
  REQUIRE(child->GetTitle() == "mock child");
  REQUIRE(child->GetClientRect() == Rect{3.f, 4.f, 50.f, 60.f});
  REQUIRE(child->GetWindowRect() == Rect{3.f, 4.f, 50.f, 60.f});
  REQUIRE(child->GetMousePosition() == Point{7.f, 8.f});
  REQUIRE(child->GetCursor() == arrow);
  REQUIRE_FALSE(child->RequestFocus());
  REQUIRE_FALSE(child->CaptureMouse());
  REQUIRE_FALSE(child->ReleaseMouse());

  child->SetVisibility(WindowVisibilityType::Show);
  REQUIRE(child->RequestFocus());
  REQUIRE(child->RequestFocus());
  REQUIRE(child->HasFocus());
  REQUIRE(focus_events == std::vector<FocusChangeType>{FocusChangeType::Gain});
  REQUIRE(child->CaptureMouse());
  REQUIRE(child->HasMouseCapture());
  REQUIRE(child->ReleaseMouse());
  REQUIRE_FALSE(child->HasMouseCapture());
  REQUIRE(child->ReleaseMouse());
  REQUIRE_FALSE(child->HasMouseCapture());
  child->SetCursor(hand);
  REQUIRE(child->GetCursor() == hand);

  child->SetVisibility(WindowVisibilityType::Hide);
  REQUIRE(child->IsCreated());
  child->SetToForeground();
  REQUIRE(child->IsCreated());
  REQUIRE(child->GetVisibility() == WindowVisibilityType::Show);
  REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{child.get()});

  child->Close();
  REQUIRE(app.GetCreatedWindows().empty());
  child->SetToForeground();
  REQUIRE(child->IsCreated());
  REQUIRE(child->GetVisibility() == WindowVisibilityType::Show);
  REQUIRE(app.GetCreatedWindows() == std::vector<MockWindow*>{child.get()});
}

TEST_CASE("Mock quit-on-all-windows-closed flag controls last-close quit",
          "[platform][gui][mock][window][QuitOnAllWindowsClosed]") {
  MockUiApplication app;
  app.SetQuitOnAllWindowClosed(false);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());

  window->SetVisibility(WindowVisibilityType::Show);
  window->Close();

  REQUIRE(app.GetCreatedWindows().empty());
  REQUIRE_FALSE(app.HasQuitRequest());
}

TEST_CASE("Mock low-level injection is ignored before native creation",
          "[platform][gui][mock][window]"
          "[MockFocusInput][MockResizeInput]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  int event_count = 0;

  auto resize_revoker =
      window->ResizeEvent()->AddHandler([&](const Size&) { ++event_count; });
  auto visibility_revoker = window->VisibilityChangeEvent()->AddHandler(
      [&](WindowVisibilityType) { ++event_count; });
  auto focus_revoker =
      window->FocusEvent()->AddHandler([&](FocusChangeType) { ++event_count; });
  auto enter_leave_revoker = window->MouseEnterLeaveEvent()->AddHandler(
      [&](MouseEnterLeaveType) { ++event_count; });
  auto move_revoker = window->MouseMoveEvent()->AddHandler(
      [&](const Point&) { ++event_count; });
  auto down_revoker = window->MouseDownEvent()->AddHandler(
      [&](const NativeMouseButtonEventArgs&) { ++event_count; });
  auto up_revoker = window->MouseUpEvent()->AddHandler(
      [&](const NativeMouseButtonEventArgs&) { ++event_count; });
  auto wheel_revoker = window->MouseWheelEvent()->AddHandler(
      [&](const NativeMouseWheelEventArgs&) { ++event_count; });

  REQUIRE_FALSE(app.ResizeWindow(window.get(), Size{10.f, 20.f}));
  REQUIRE_FALSE(app.ShowWindow(window.get()));
  REQUIRE_FALSE(app.FocusWindow(window.get()));
  REQUIRE_FALSE(app.KeyDown(cru::platform::gui::KeyCode::A));
  REQUIRE(event_count == 0);
  REQUIRE(window->GetClientSize() == Size{});
  REQUIRE(window->GetVisibility() == WindowVisibilityType::Hide);
  REQUIRE_FALSE(window->HasFocus());
  REQUIRE_FALSE(window->IsMouseInside());
  REQUIRE(window->GetMousePosition() == Point{});
}

TEST_CASE("Mock low-level resize visibility and app focus update state first",
          "[platform][gui][mock][window][MockFocusInput]"
          "[MockResizeInput]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::unique_ptr<MockWindow> other_window(app.CreateMockWindow());
  std::vector<std::string> events;

  window->SetClientSize(Size{10.f, 20.f});
  window->SetVisibility(WindowVisibilityType::Show);
  other_window->SetVisibility(WindowVisibilityType::Show);
  REQUIRE(window->GetBackingImage() != nullptr);

  auto resize_revoker =
      window->ResizeEvent()->AddHandler([&](const Size& size) {
        REQUIRE(window->GetClientSize() == size);
        REQUIRE(window->GetWindowRect().GetSize() ==
                Size{size.width + 16.f, size.height + 38.f});
        REQUIRE(window->GetBackingImage() != nullptr);
        REQUIRE(window->GetBackingImage()->GetWidth() == size.width);
        REQUIRE(window->GetBackingImage()->GetHeight() == size.height);
        events.push_back("resize");
      });
  auto visibility_revoker = window->VisibilityChangeEvent()->AddHandler(
      [&](WindowVisibilityType visibility) {
        REQUIRE(window->GetVisibility() == visibility);
        events.push_back(visibility == WindowVisibilityType::Hide   ? "hide"
                         : visibility == WindowVisibilityType::Show ? "show"
                         : visibility == WindowVisibilityType::Minimize
                             ? "minimize"
                             : "other");
      });
  auto focus_revoker =
      window->FocusEvent()->AddHandler([&](FocusChangeType focus) {
        REQUIRE(window->HasFocus() == (focus == FocusChangeType::Gain));
        events.push_back(focus == FocusChangeType::Gain ? "focus-gain"
                                                        : "focus-lose");
      });

  REQUIRE(app.ResizeWindow(window.get(), Size{30.f, 40.f}));
  REQUIRE_FALSE(app.ResizeWindow(window.get(), Size{30.f, 40.f}));
  REQUIRE(app.FocusWindow(window.get()));
  REQUIRE(app.GetFocusedWindow() == window.get());
  REQUIRE(app.FocusWindow(window.get()));
  REQUIRE(app.FocusWindow(other_window.get()));
  REQUIRE(app.GetFocusedWindow() == other_window.get());
  REQUIRE(app.HideWindow(window.get()));
  REQUIRE_FALSE(app.HideWindow(window.get()));
  REQUIRE(app.ShowWindow(window.get()));
  REQUIRE(app.MinimizeWindow(window.get()));

  REQUIRE(events == std::vector<std::string>{"resize", "focus-gain",
                                             "focus-lose", "hide", "show",
                                             "minimize"});
  REQUIRE(graphics_factory.GetBitmapSizes() ==
          std::vector<Size>{Size{10.f, 20.f}, Size{}, Size{30.f, 40.f}});
}

TEST_CASE("Mock application mouse actions preserve native args and state",
          "[platform][gui][mock][window][MockMouseInput]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<MouseEnterLeaveType> enter_leave_events;
  std::vector<Point> move_points;
  std::vector<Point> observed_positions;
  std::vector<std::string> button_events;
  std::vector<float> wheel_deltas;
  std::vector<bool> wheel_horizontal;

  auto enter_leave_revoker =
      window->MouseEnterLeaveEvent()->AddHandler([&](MouseEnterLeaveType type) {
        REQUIRE(window->IsMouseInside() ==
                (type == MouseEnterLeaveType::Enter));
        enter_leave_events.push_back(type);
      });
  auto move_revoker =
      window->MouseMoveEvent()->AddHandler([&](const Point& point) {
        move_points.push_back(point);
        observed_positions.push_back(window->GetMousePosition());
      });
  auto down_revoker = window->MouseDownEvent()->AddHandler(
      [&](const NativeMouseButtonEventArgs& args) {
        REQUIRE(window->GetMousePosition() == args.point);
        REQUIRE(args.button == MouseButtons::Right);
        REQUIRE(args.modifier == KeyModifiers::Alt);
        button_events.push_back("down");
      });
  auto up_revoker = window->MouseUpEvent()->AddHandler(
      [&](const NativeMouseButtonEventArgs& args) {
        REQUIRE(window->GetMousePosition() == args.point);
        REQUIRE(args.button == MouseButtons::Right);
        REQUIRE(args.modifier == KeyModifiers::Ctrl);
        button_events.push_back("up");
      });
  auto wheel_revoker = window->MouseWheelEvent()->AddHandler(
      [&](const NativeMouseWheelEventArgs& args) {
        REQUIRE(window->GetMousePosition() == args.point);
        REQUIRE(args.modifier == KeyModifiers::Shift);
        wheel_deltas.push_back(args.delta);
        wheel_horizontal.push_back(args.horizontal);
      });

  window->SetClientSize(Size{200.f, 200.f});
  window->SetVisibility(WindowVisibilityType::Show);

  REQUIRE(app.MoveMouse(Point{10.f, 20.f}));
  REQUIRE(window->CaptureMouse());
  REQUIRE(app.MoveMouse(Point{30.f, 40.f}));
  REQUIRE(app.MouseDown(MouseButtons::Right, KeyModifiers::Alt));
  REQUIRE(app.GetFocusedWindow() == window.get());
  REQUIRE(window->HasFocus());
  REQUIRE(window->HasMouseCapture());
  REQUIRE(app.MoveMouse(Point{50.f, 60.f}));
  REQUIRE(app.MouseUp(MouseButtons::Right, KeyModifiers::Ctrl));
  REQUIRE(window->HasMouseCapture());
  REQUIRE(app.MoveMouse(Point{70.f, 80.f}));
  REQUIRE(app.MouseWheel(-3.f, KeyModifiers::Shift, false));
  REQUIRE(app.MoveMouse(Point{90.f, 100.f}));
  REQUIRE(app.MouseWheel(4.f, KeyModifiers::Shift, true));
  REQUIRE(window->HasMouseCapture());

  REQUIRE(enter_leave_events ==
          std::vector<MouseEnterLeaveType>{MouseEnterLeaveType::Enter});
  REQUIRE(move_points ==
          std::vector<Point>{Point{10.f, 20.f}, Point{30.f, 40.f},
                             Point{50.f, 60.f}, Point{70.f, 80.f},
                             Point{90.f, 100.f}});
  REQUIRE(observed_positions == move_points);
  REQUIRE(button_events == std::vector<std::string>{"down", "up"});
  REQUIRE(wheel_deltas == std::vector<float>{-3.f, 4.f});
  REQUIRE(wheel_horizontal == std::vector<bool>{false, true});
  REQUIRE(window->GetMousePosition() == Point{90.f, 100.f});
}

TEST_CASE("Mock repaint requests are async coalesced and ordered",
          "[platform][gui][mock][window][repaint][MockRepaint]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<std::string> events;

  auto paint_revoker = window->PaintEvent()->AddSpyOnlyHandler([&] {
    events.push_back("paint");
    REQUIRE(window->HasPendingRepaint() == false);
  });
  auto paint1_revoker = window->Paint1Event()->AddHandler(
      [&](const cru::platform::gui::NativePaintEventArgs& args) {
        events.push_back("paint1");
        REQUIRE(args.repaint_area == Rect{{}, window->GetClientSize()});
      });

  window->SetClientSize(Size{123.f, 45.f});
  window->SetVisibility(WindowVisibilityType::Show);
  window->RequestRepaint();
  window->RequestRepaint();

  REQUIRE(window->HasPendingRepaint());
  REQUIRE(events.empty());

  REQUIRE(app.PumpOnce());
  REQUIRE(events == std::vector<std::string>{"paint", "paint1"});
  REQUIRE_FALSE(window->HasPendingRepaint());
  REQUIRE_FALSE(app.PumpOnce());
}

TEST_CASE("Mock repaint requested before close is a pump no-op",
          "[platform][gui][mock][window][repaint][MockRepaint]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  app.SetQuitOnAllWindowClosed(false);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  int paint_count = 0;
  auto paint_revoker = window->PaintEvent()->AddSpyOnlyHandler(
      [&paint_count] { ++paint_count; });
  auto destroy_revoker = window->DestroyEvent()->AddSpyOnlyHandler([&] {
    window->RequestRepaint();
    REQUIRE_FALSE(window->HasPendingRepaint());
  });

  window->SetVisibility(WindowVisibilityType::Show);
  window->RequestRepaint();
  REQUIRE(window->HasPendingRepaint());

  window->Close();
  REQUIRE_FALSE(window->IsCreated());
  REQUIRE_FALSE(window->HasPendingRepaint());
  REQUIRE_FALSE(app.PumpOnce());
  REQUIRE(paint_count == 0);
}

TEST_CASE("Mock repaint stops if paint event closes the window",
          "[platform][gui][mock][window][repaint][MockRepaint]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  app.SetQuitOnAllWindowClosed(false);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<std::string> events;
  auto paint_revoker = window->PaintEvent()->AddSpyOnlyHandler([&] {
    events.push_back("paint");
    window->Close();
  });
  auto paint1_revoker = window->Paint1Event()->AddHandler(
      [&](const cru::platform::gui::NativePaintEventArgs&) {
        events.push_back("paint1");
      });

  window->SetVisibility(WindowVisibilityType::Show);
  window->RequestRepaint();

  REQUIRE(app.PumpOnce());
  REQUIRE(events == std::vector<std::string>{"paint"});
  REQUIRE_FALSE(window->IsCreated());
}

TEST_CASE("Mock begin paint returns a backing image painter",
          "[platform][gui][mock][window][paint][MockBeginPaint]"
          "[BackingBitmap]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());

  window->SetClientSize(Size{10.f, 20.f});
  window->SetVisibility(WindowVisibilityType::Show);
  REQUIRE(window->GetBackingImage() != nullptr);
  REQUIRE(window->GetBackingImage()->GetWidth() == 10.f);
  REQUIRE(window->GetBackingImage()->GetHeight() == 20.f);

  auto painter = window->BeginPaint();
  REQUIRE(painter != nullptr);
  REQUIRE(graphics_factory.GetPainterCount() == 1);
  painter->Clear(white);
  REQUIRE(graphics_factory.GetClearCount() == 1);
  REQUIRE(graphics_factory.GetEndDrawCount() == 0);
  painter->EndDraw();
  REQUIRE(graphics_factory.GetEndDrawCount() == 1);
}

TEST_CASE("Mock paint event can drive a repaint handler into the backing image",
          "[platform][gui][mock][window][paint][MockRepaint]"
          "[MockBeginPaint][BackingBitmap]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  int repaint_handler_count = 0;
  auto paint1_revoker = window->Paint1Event()->AddHandler(
      [&](const cru::platform::gui::NativePaintEventArgs& args) {
        REQUIRE(args.repaint_area == Rect{{}, window->GetClientSize()});
        auto painter = window->BeginPaint();
        painter->Clear(white);
        painter->EndDraw();
        ++repaint_handler_count;
      });

  window->SetClientSize(Size{80.f, 30.f});
  window->SetVisibility(WindowVisibilityType::Show);
  window->RequestRepaint();

  REQUIRE(app.PumpOnce());
  REQUIRE(repaint_handler_count == 1);
  REQUIRE(graphics_factory.GetPainterCount() == 1);
  REQUIRE(graphics_factory.GetClearCount() == 1);
  REQUIRE(graphics_factory.GetEndDrawCount() == 1);
}

TEST_CASE("Mock backing bitmap dimensions follow client resize",
          "[platform][gui][mock][window][paint][BackingBitmap]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());

  window->SetClientSize(Size{12.2f, 34.1f});
  REQUIRE(window->GetBackingImage() == nullptr);

  window->SetVisibility(WindowVisibilityType::Show);
  REQUIRE(window->GetBackingImage() != nullptr);
  REQUIRE(window->GetBackingImage()->GetWidth() == 13.f);
  REQUIRE(window->GetBackingImage()->GetHeight() == 35.f);

  window->SetClientSize(Size{50.f, 60.f});
  REQUIRE(window->GetBackingImage()->GetWidth() == 50.f);
  REQUIRE(window->GetBackingImage()->GetHeight() == 60.f);
  REQUIRE(graphics_factory.GetBitmapSizes() ==
          std::vector<Size>{Size{13.f, 35.f}, Size{50.f, 60.f}});
}

TEST_CASE("Mock painting reports missing graphics factories only at paint time",
          "[platform][gui][mock][window][paint][MockBeginPaint]") {
  {
    MockUiApplication app;
    std::unique_ptr<MockWindow> window(app.CreateMockWindow());

    window->SetClientSize(Size{10.f, 10.f});
    window->SetVisibility(WindowVisibilityType::Show);
    window->RequestRepaint();
    REQUIRE(app.PumpOnce());

    try {
      CRU_UNUSED(window->BeginPaint());
      FAIL("BeginPaint should require an injected graphics factory.");
    } catch (const Exception& exception) {
      REQUIRE(std::string_view(exception.what()).find("graphics factory") !=
              std::string_view::npos);
    }
  }

  {
    NoImageGraphicsFactory graphics_factory;
    MockUiApplication app(&graphics_factory);
    std::unique_ptr<MockWindow> window(app.CreateMockWindow());

    window->SetClientSize(Size{10.f, 10.f});
    window->SetVisibility(WindowVisibilityType::Show);

    try {
      CRU_UNUSED(window->BeginPaint());
      FAIL("BeginPaint should require image factory support.");
    } catch (const Exception& exception) {
      REQUIRE(std::string_view(exception.what()).find("image factory") !=
              std::string_view::npos);
    }
  }
}

TEST_CASE("Mock snapshot image requires paint and reports backing dimensions",
          "[platform][gui][mock][window][snapshot][MockSnapshot]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());

  window->SetClientSize(Size{22.f, 10.f});
  REQUIRE(window->GetBackingImage() == nullptr);

  try {
    CRU_UNUSED(window->GetSnapshotImage());
    FAIL("GetSnapshotImage should require a paint before snapshot access.");
  } catch (const Exception& exception) {
    auto message = std::string(exception.what());
    REQUIRE(message.find("requires at least one BeginPaint") !=
            std::string::npos);
    REQUIRE(message.find("snapshot=22x10") != std::string::npos);
    REQUIRE(message.find("paint_count=0") != std::string::npos);
  }

  auto painter = window->BeginPaint();
  painter->EndDraw();

  auto* snapshot = window->GetSnapshotImage();

  REQUIRE(snapshot != nullptr);
  REQUIRE(window->GetBackingImage() == snapshot);
  REQUIRE(snapshot->GetWidth() == 22.f);
  REQUIRE(snapshot->GetHeight() == 10.f);
  REQUIRE(window->GetPaintCount() == 1);
  REQUIRE(graphics_factory.GetBitmapSizes() ==
          std::vector<Size>{Size{22.f, 10.f}});
}

TEST_CASE("Mock snapshot clone creates an independent bitmap",
          "[platform][gui][mock][window][snapshot][MockSnapshot]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());

  window->SetClientSize(Size{32.f, 16.f});
  auto painter = window->BeginPaint();
  painter->EndDraw();
  auto* snapshot = window->GetSnapshotImage();
  auto clone = window->CloneSnapshotToBitmap();

  REQUIRE(clone != nullptr);
  REQUIRE(clone.get() != snapshot);
  REQUIRE(clone->GetWidth() == 32.f);
  REQUIRE(clone->GetHeight() == 16.f);
  REQUIRE(graphics_factory.GetPainterCount() == 2);
  REQUIRE(graphics_factory.GetBitmapSizes() ==
          std::vector<Size>{Size{32.f, 16.f}, Size{32.f, 16.f}});
}

TEST_CASE("Mock snapshot encodes PNG bytes through image factory stream",
          "[platform][gui][mock][window][snapshot][EncodeSnapshot]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::array<std::byte, 64> png_buffer{};
  cru::io::MemoryStream stream(png_buffer.data(), png_buffer.size());

  window->SetClientSize(Size{40.f, 12.f});
  auto painter = window->BeginPaint();
  painter->Clear(white);
  painter->EndDraw();

  auto* snapshot = window->GetSnapshotImage();
  window->EncodeSnapshotToStream(
      &stream, cru::platform::graphics::ImageFormat::Png, 0.75f);

  REQUIRE(stream.Tell() == static_cast<cru::Index>(kPngSignature.size()));
  for (std::size_t index = 0; index < kPngSignature.size(); ++index) {
    REQUIRE(png_buffer[index] == kPngSignature[index]);
  }
  auto* image_factory = graphics_factory.GetFakeImageFactory();
  REQUIRE(image_factory->GetEncodeCount() == 1);
  REQUIRE(image_factory->GetLastEncodedImage() == snapshot);
  REQUIRE(image_factory->GetLastFormat() ==
          cru::platform::graphics::ImageFormat::Png);
  REQUIRE(image_factory->GetLastQuality() == 0.75f);

  stream.Seek(0, cru::io::Stream::SeekOrigin::Begin);
  auto decoded = image_factory->DecodeFromStream(&stream);
  REQUIRE(decoded != nullptr);
  REQUIRE(decoded->GetWidth() == snapshot->GetWidth());
  REQUIRE(decoded->GetHeight() == snapshot->GetHeight());
}

TEST_CASE("MockDiagnostics snapshot encode reports before-paint state",
          "[platform][gui][mock][window][snapshot][EncodeSnapshot]"
          "[MockDiagnostics]") {
  FakeGraphicsFactory graphics_factory;
  MockUiApplication app(&graphics_factory);
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::array<std::byte, 16> png_buffer{};
  cru::io::MemoryStream stream(png_buffer.data(), png_buffer.size());

  window->SetClientSize(Size{11.f, 7.f});
  REQUIRE(window->GetBackingImage() == nullptr);

  try {
    window->EncodeSnapshotToStream(&stream);
    FAIL("EncodeSnapshotToStream should require a paint before encoding.");
  } catch (const Exception& exception) {
    auto message = std::string(exception.what());
    REQUIRE(message.find("requires at least one BeginPaint") !=
            std::string::npos);
    REQUIRE(message.find("snapshot=11x7") != std::string::npos);
    REQUIRE(message.find("paint_count=0") != std::string::npos);
    REQUIRE(message.find("last_injected_event=(none)") != std::string::npos);
  }

  REQUIRE(window->GetBackingImage() != nullptr);
  REQUIRE(window->GetBackingImage()->GetWidth() == 11.f);
  REQUIRE(window->GetBackingImage()->GetHeight() == 7.f);
  REQUIRE(graphics_factory.GetPainterCount() == 0);
  REQUIRE(graphics_factory.GetFakeImageFactory()->GetEncodeCount() == 0);
  REQUIRE(stream.Tell() == 0);
}

TEST_CASE("MockDiagnostics snapshot reports missing stream and factories",
          "[platform][gui][mock][window][snapshot][EncodeSnapshot]"
          "[MockDiagnostics]") {
  {
    FakeGraphicsFactory graphics_factory;
    MockUiApplication app(&graphics_factory);
    std::unique_ptr<MockWindow> window(app.CreateMockWindow());

    try {
      window->EncodeSnapshotToStream(nullptr);
      FAIL("EncodeSnapshotToStream should require a stream.");
    } catch (const Exception& exception) {
      auto message = std::string(exception.what());
      REQUIRE(message.find("stream") != std::string::npos);
      REQUIRE(message.find("window={created=false") != std::string::npos);
      REQUIRE(message.find("paint_count=0") != std::string::npos);
    }
  }

  {
    MockUiApplication app;
    std::unique_ptr<MockWindow> window(app.CreateMockWindow());
    std::array<std::byte, 16> png_buffer{};
    cru::io::MemoryStream stream(png_buffer.data(), png_buffer.size());

    try {
      window->GetSnapshotImage();
      FAIL("GetSnapshotImage should require an injected graphics factory.");
    } catch (const Exception& exception) {
      auto message = std::string(exception.what());
      REQUIRE(message.find("graphics factory") != std::string::npos);
      REQUIRE(message.find("window={") != std::string::npos);
      REQUIRE(message.find("snapshot=(none)") != std::string::npos);
    }

    try {
      window->EncodeSnapshotToStream(&stream);
      FAIL(
          "EncodeSnapshotToStream should require an injected graphics "
          "factory.");
    } catch (const Exception& exception) {
      auto message = std::string(exception.what());
      REQUIRE(message.find("graphics factory") != std::string::npos);
      REQUIRE(message.find("window={") != std::string::npos);
      REQUIRE(message.find("snapshot=(none)") != std::string::npos);
    }
  }

  {
    NoImageGraphicsFactory graphics_factory;
    MockUiApplication app(&graphics_factory);
    std::unique_ptr<MockWindow> window(app.CreateMockWindow());
    std::array<std::byte, 16> png_buffer{};
    cru::io::MemoryStream stream(png_buffer.data(), png_buffer.size());

    try {
      window->GetSnapshotImage();
      FAIL("GetSnapshotImage should require image factory support.");
    } catch (const Exception& exception) {
      auto message = std::string(exception.what());
      REQUIRE(message.find("image factory") != std::string::npos);
      REQUIRE(message.find("window={") != std::string::npos);
      REQUIRE(message.find("snapshot=(none)") != std::string::npos);
    }

    try {
      window->EncodeSnapshotToStream(&stream);
      FAIL("EncodeSnapshotToStream should require image factory support.");
    } catch (const Exception& exception) {
      auto message = std::string(exception.what());
      REQUIRE(message.find("image factory") != std::string::npos);
      REQUIRE(message.find("window={") != std::string::npos);
      REQUIRE(message.find("snapshot=(none)") != std::string::npos);
    }
  }
}
