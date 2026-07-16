#pragma once

#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Image.h"
#include "cru/platform/graphics/Painter.h"

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace cru::test::ui::mock {
inline constexpr std::array<std::byte, 8> kPngSignature{
    std::byte{0x89}, std::byte{0x50}, std::byte{0x4E}, std::byte{0x47},
    std::byte{0x0D}, std::byte{0x0A}, std::byte{0x1A}, std::byte{0x0A}};

class FakePainter : public virtual platform::graphics::IPainter {
 public:
  FakePainter(int* clear_count, int* end_draw_count, int* fill_count)
      : clear_count_(clear_count),
        end_draw_count_(end_draw_count),
        fill_count_(fill_count) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  platform::Matrix GetTransform() override { return transform_; }
  void SetTransform(const platform::Matrix& matrix) override {
    transform_ = matrix;
  }
  void ConcatTransform(const platform::Matrix& matrix) override {
    transform_ *= matrix;
  }
  void Clear(const platform::Color&) override { ++*clear_count_; }
  void DrawLine(const platform::Point&, const platform::Point&,
                platform::graphics::IBrush*, float) override {}
  void StrokeRectangle(const platform::Rect&, platform::graphics::IBrush*,
                       float) override {}
  void FillRectangle(const platform::Rect&,
                     platform::graphics::IBrush*) override {
    ++*fill_count_;
  }
  void StrokeEllipse(const platform::Rect&, platform::graphics::IBrush*,
                     float) override {}
  void FillEllipse(const platform::Rect&,
                   platform::graphics::IBrush*) override {}
  void StrokeGeometry(platform::graphics::IGeometry*,
                      platform::graphics::IBrush*, float) override {}
  void FillGeometry(platform::graphics::IGeometry*,
                    platform::graphics::IBrush*) override {}
  void DrawText(const platform::Point&, platform::graphics::ITextLayout*,
                platform::graphics::IBrush*) override {}
  void DrawImage(const platform::Point&, platform::graphics::IImage*) override {
  }
  void PushLayer(const platform::Rect&) override {}
  void PopLayer() override {}
  void PushState() override {}
  void PopState() override {}
  void EndDraw() override { ++*end_draw_count_; }

 private:
  platform::Matrix transform_ = platform::Matrix::Identity();
  int* clear_count_;
  int* end_draw_count_;
  int* fill_count_;
};

class FakeImage : public virtual platform::graphics::IImage {
 public:
  FakeImage(platform::graphics::IGraphicsFactory* graphics_factory, int width,
            int height, int* painter_count, int* clear_count,
            int* end_draw_count, int* fill_count)
      : graphics_factory_(graphics_factory),
        width_(width),
        height_(height),
        painter_count_(painter_count),
        clear_count_(clear_count),
        end_draw_count_(end_draw_count),
        fill_count_(fill_count) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  platform::graphics::IGraphicsFactory* GetGraphicsFactory() override {
    return graphics_factory_;
  }
  float GetWidth() override { return static_cast<float>(width_); }
  float GetHeight() override { return static_cast<float>(height_); }
  std::unique_ptr<platform::graphics::IImage> CreateWithRect(
      const platform::Rect& rect) override {
    return std::make_unique<FakeImage>(
        graphics_factory_, static_cast<int>(rect.width),
        static_cast<int>(rect.height), painter_count_, clear_count_,
        end_draw_count_, fill_count_);
  }
  std::unique_ptr<platform::graphics::IPainter> CreatePainter() override {
    ++*painter_count_;
    return std::make_unique<FakePainter>(clear_count_, end_draw_count_,
                                         fill_count_);
  }

 private:
  platform::graphics::IGraphicsFactory* graphics_factory_;
  int width_;
  int height_;
  int* painter_count_;
  int* clear_count_;
  int* end_draw_count_;
  int* fill_count_;
};

class FakeImageFactory : public virtual platform::graphics::IImageFactory {
 public:
  FakeImageFactory(platform::graphics::IGraphicsFactory* graphics_factory,
                   int* painter_count, int* clear_count, int* end_draw_count,
                   int* fill_count)
      : graphics_factory_(graphics_factory),
        painter_count_(painter_count),
        clear_count_(clear_count),
        end_draw_count_(end_draw_count),
        fill_count_(fill_count) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  platform::graphics::IGraphicsFactory* GetGraphicsFactory() override {
    return graphics_factory_;
  }
  std::unique_ptr<platform::graphics::IImage> DecodeFromStream(
      io::Stream*) override {
    return nullptr;
  }
  void EncodeToStream(platform::graphics::IImage* image, io::Stream* stream,
                      platform::graphics::ImageFormat format,
                      float quality) override {
    CRU_UNUSED(image)
    last_format_ = format;
    last_quality_ = quality;
    stream->Write(kPngSignature.data(), kPngSignature.size());
  }
  std::unique_ptr<platform::graphics::IImage> CreateBitmap(
      int width, int height) override {
    bitmap_sizes_.push_back(
        {static_cast<float>(width), static_cast<float>(height)});
    return std::make_unique<FakeImage>(graphics_factory_, width, height,
                                       painter_count_, clear_count_,
                                       end_draw_count_, fill_count_);
  }
  const std::vector<platform::Size>& GetBitmapSizes() const {
    return bitmap_sizes_;
  }
  platform::graphics::ImageFormat GetLastFormat() const { return last_format_; }
  float GetLastQuality() const { return last_quality_; }

 private:
  platform::graphics::IGraphicsFactory* graphics_factory_;
  int* painter_count_;
  int* clear_count_;
  int* end_draw_count_;
  int* fill_count_;
  std::vector<platform::Size> bitmap_sizes_;
  platform::graphics::ImageFormat last_format_ =
      platform::graphics::ImageFormat::Png;
  float last_quality_ = 0.f;
};

class FakeGraphicsFactory
    : public virtual platform::graphics::IGraphicsFactory {
 public:
  FakeGraphicsFactory()
      : image_factory_(this, &painter_count_, &clear_count_, &end_draw_count_,
                       &fill_count_) {}

  std::string GetPlatformId() const override { return "FakeGraphics"; }
  std::unique_ptr<platform::graphics::ISolidColorBrush> CreateSolidColorBrush()
      override {
    return nullptr;
  }
  std::unique_ptr<platform::graphics::IGeometryBuilder> CreateGeometryBuilder()
      override {
    return nullptr;
  }
  std::unique_ptr<platform::graphics::IFont> CreateFont(std::string,
                                                        float) override {
    return nullptr;
  }
  std::unique_ptr<platform::graphics::ITextLayout> CreateTextLayout(
      std::shared_ptr<platform::graphics::IFont>, std::string) override {
    return nullptr;
  }
  platform::graphics::IImageFactory* GetImageFactory() override {
    return &image_factory_;
  }
  int GetPainterCount() const { return painter_count_; }
  int GetClearCount() const { return clear_count_; }
  int GetEndDrawCount() const { return end_draw_count_; }
  int GetFillCount() const { return fill_count_; }
  FakeImageFactory* GetFakeImageFactory() { return &image_factory_; }

 private:
  int painter_count_ = 0;
  int clear_count_ = 0;
  int end_draw_count_ = 0;
  int fill_count_ = 0;
  FakeImageFactory image_factory_;
};
}  // namespace cru::test::ui::mock
