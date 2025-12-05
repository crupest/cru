#pragma once
#include "Base.h"

#include <cru/base/Base.h>
#include <cru/platform/graphics/Painter.h>

#include <functional>
#include <vector>

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoPainter : public CairoResource,
                                                     public virtual IPainter {
 private:
  constexpr static auto kLogTag =
      "cru::platform::graphics::cairo::CairoPainter";

 public:
  CairoPainter(CairoGraphicsFactory* factory, cairo_t* cairo, bool auto_release,
               cairo_surface_t* cairo_surface = nullptr);
  ~CairoPainter();

 public:
  Matrix GetTransform() override;
  void SetTransform(const Matrix& matrix) override;

  void ConcatTransform(const Matrix& matrix) override;

  void Clear(const Color& color) override;

  void DrawLine(const Point& start, const Point& end, IBrush* brush,
                float width) override;
  void StrokeRectangle(const Rect& rectangle, IBrush* brush,
                       float width) override;
  void FillRectangle(const Rect& rectangle, IBrush* brush) override;
  void StrokeEllipse(const Rect& outline_rect, IBrush* brush,
                     float width) override;
  void FillEllipse(const Rect& outline_rect, IBrush* brush) override;

  void StrokeGeometry(IGeometry* geometry, IBrush* brush, float width) override;
  void FillGeometry(IGeometry* geometry, IBrush* brush) override;

  void DrawText(const Point& offset, ITextLayout* text_layout,
                IBrush* brush) override;

  void DrawImage(const Point& offset, IImage* image) override;

  void PushLayer(const Rect& bounds) override;
  void PopLayer() override;

  void PushState() override;
  void PopState() override;

  void EndDraw() override;

  void SetEndDrawCallback(std::function<void()> action);

 private:
  void CheckValidation();

  bool valid_ = true;

  cairo_t* cairo_;
  bool auto_release_;
  std::function<void()> end_draw_callback_;

  cairo_surface_t* cairo_surface_;

  std::vector<Rect> layer_stack_;
};
}  // namespace cru::platform::graphics::cairo
