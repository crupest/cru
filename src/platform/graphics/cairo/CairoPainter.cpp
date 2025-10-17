#include "cru/platform/graphics/cairo/CairoPainter.h"
#include "cru/base/Exception.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/Check.h"
#include "cru/platform/Exception.h"
#include "cru/platform/graphics/cairo/Base.h"
#include "cru/platform/graphics/cairo/CairoBrush.h"
#include "cru/platform/graphics/cairo/CairoGeometry.h"
#include "cru/platform/graphics/cairo/CairoImage.h"
#include "cru/platform/graphics/cairo/CairoResource.h"
#include "cru/platform/graphics/cairo/PangoTextLayout.h"

#include <cairo.h>
#include <pango/pangocairo.h>

namespace cru::platform::graphics::cairo {
CairoPainter::CairoPainter(CairoGraphicsFactory* factory, cairo_t* cairo,
                           bool auto_release, cairo_surface_t* cairo_surface)
    : CairoResource(factory),
      cairo_(cairo),
      auto_release_(auto_release),
      cairo_surface_(cairo_surface) {}

CairoPainter::~CairoPainter() {
  if (auto_release_) {
    cairo_destroy(cairo_);
  }
}

Matrix CairoPainter::GetTransform() {
  CheckValidation();
  cairo_matrix_t matrix;
  cairo_get_matrix(cairo_, &matrix);
  return Convert(matrix);
}

void CairoPainter::SetTransform(const Matrix& matrix) {
  CheckValidation();
  auto m = Convert(matrix);
  cairo_set_matrix(cairo_, &m);
}

void CairoPainter::ConcatTransform(const Matrix& matrix) {
  CheckValidation();
  auto m = Convert(matrix);
  cairo_transform(cairo_, &m);
}

void CairoPainter::Clear(const Color& color) {
  CheckValidation();
  cairo_set_source_rgba(cairo_, color.GetFloatRed(), color.GetFloatGreen(),
                        color.GetFloatBlue(), color.GetFloatAlpha());
  cairo_paint(cairo_);
}

void CairoPainter::DrawLine(const Point& start, const Point& end, IBrush* brush,
                            float width) {
  CheckValidation();
  auto cairo_brush = CheckPlatform<CairoBrush>(brush, GetPlatformId());
  auto cairo_pattern = cairo_brush->GetCairoPattern();
  cairo_save(cairo_);
  cairo_set_source(cairo_, cairo_pattern);
  cairo_set_line_width(cairo_, width);
  cairo_new_path(cairo_);
  cairo_move_to(cairo_, start.x, start.y);
  cairo_line_to(cairo_, end.x, end.y);
  cairo_stroke(cairo_);
  cairo_restore(cairo_);
}

void CairoPainter::StrokeRectangle(const Rect& rectangle, IBrush* brush,
                                   float width) {
  CheckValidation();
  auto cairo_brush = CheckPlatform<CairoBrush>(brush, GetPlatformId());
  auto cairo_pattern = cairo_brush->GetCairoPattern();
  cairo_save(cairo_);
  cairo_set_source(cairo_, cairo_pattern);
  cairo_set_line_width(cairo_, width);
  cairo_new_path(cairo_);
  cairo_rectangle(cairo_, rectangle.left, rectangle.top, rectangle.width,
                  rectangle.height);
  cairo_stroke(cairo_);
  cairo_restore(cairo_);
}

void CairoPainter::FillRectangle(const Rect& rectangle, IBrush* brush) {
  CheckValidation();
  auto cairo_brush = CheckPlatform<CairoBrush>(brush, GetPlatformId());
  auto cairo_pattern = cairo_brush->GetCairoPattern();
  cairo_save(cairo_);
  cairo_set_source(cairo_, cairo_pattern);
  cairo_new_path(cairo_);
  cairo_rectangle(cairo_, rectangle.left, rectangle.top, rectangle.width,
                  rectangle.height);
  cairo_fill(cairo_);
  cairo_restore(cairo_);
}

void CairoPainter::StrokeEllipse(const Rect& outline_rect, IBrush* brush,
                                 float width) {
  CheckValidation();
  auto cairo_brush = CheckPlatform<CairoBrush>(brush, GetPlatformId());
  auto cairo_pattern = cairo_brush->GetCairoPattern();
  cairo_save(cairo_);
  cairo_set_source(cairo_, cairo_pattern);
  cairo_set_line_width(cairo_, width);
  {
    auto center = outline_rect.GetCenter();
    cairo_matrix_t save_matrix;
    cairo_get_matrix(cairo_, &save_matrix);
    cairo_translate(cairo_, center.x, center.y);
    cairo_scale(cairo_, 1, outline_rect.height / outline_rect.width);
    cairo_translate(cairo_, -center.x, -center.y);
    cairo_new_path(cairo_);
    cairo_arc(cairo_, center.x, center.y, outline_rect.width / 2.0, 0,
              2 * M_PI);
    cairo_set_matrix(cairo_, &save_matrix);
  }
  cairo_stroke(cairo_);
  cairo_restore(cairo_);
}

void CairoPainter::FillEllipse(const Rect& outline_rect, IBrush* brush) {
  CheckValidation();
  auto cairo_brush = CheckPlatform<CairoBrush>(brush, GetPlatformId());
  auto cairo_pattern = cairo_brush->GetCairoPattern();
  cairo_save(cairo_);
  cairo_set_source(cairo_, cairo_pattern);
  {
    auto center = outline_rect.GetCenter();
    cairo_matrix_t save_matrix;
    cairo_get_matrix(cairo_, &save_matrix);
    cairo_translate(cairo_, center.x, center.y);
    cairo_scale(cairo_, 1, outline_rect.height / outline_rect.width);
    cairo_translate(cairo_, -center.x, -center.y);
    cairo_new_path(cairo_);
    cairo_arc(cairo_, center.x, center.y, outline_rect.width / 2.0, 0,
              2 * M_PI);
    cairo_set_matrix(cairo_, &save_matrix);
  }
  cairo_fill(cairo_);
  cairo_restore(cairo_);
}

void CairoPainter::StrokeGeometry(IGeometry* geometry, IBrush* brush,
                                  float width) {
  CheckValidation();
  auto cairo_geometry = CheckPlatform<CairoGeometry>(geometry, GetPlatformId());
  auto cairo_brush = CheckPlatform<CairoBrush>(brush, GetPlatformId());

  auto cairo_path = cairo_geometry->GetCairoPath();
  auto cairo_pattern = cairo_brush->GetCairoPattern();

  cairo_save(cairo_);
  cairo_set_source(cairo_, cairo_pattern);
  cairo_set_line_width(cairo_, width);
  cairo_new_path(cairo_);
  cairo_append_path(cairo_, cairo_path);
  cairo_stroke(cairo_);
  cairo_restore(cairo_);
}

void CairoPainter::FillGeometry(IGeometry* geometry, IBrush* brush) {
  CheckValidation();
  auto cairo_geometry = CheckPlatform<CairoGeometry>(geometry, GetPlatformId());
  auto cairo_brush = CheckPlatform<CairoBrush>(brush, GetPlatformId());

  auto cairo_path = cairo_geometry->GetCairoPath();
  auto cairo_pattern = cairo_brush->GetCairoPattern();

  cairo_save(cairo_);
  cairo_set_source(cairo_, cairo_pattern);
  cairo_new_path(cairo_);
  cairo_append_path(cairo_, cairo_path);
  cairo_fill(cairo_);
  cairo_restore(cairo_);
}

void CairoPainter::DrawText(const Point& offset, ITextLayout* text_layout,
                            IBrush* brush) {
  CheckValidation();

  auto pango_text_layout =
      CheckPlatform<PangoTextLayout>(text_layout, GetPlatformId());

  auto cairo_brush = CheckPlatform<CairoBrush>(brush, GetPlatformId());
  auto cairo_pattern = cairo_brush->GetCairoPattern();

  cairo_save(cairo_);
  cairo_set_source(cairo_, cairo_pattern);
  cairo_move_to(cairo_, offset.x, offset.y);
  pango_cairo_update_layout(cairo_, pango_text_layout->GetPangoLayout());
  pango_cairo_show_layout(cairo_, pango_text_layout->GetPangoLayout());
  cairo_restore(cairo_);
}

void CairoPainter::DrawImage(const Point& offset, IImage* image) {
  CheckValidation();
  auto cairo_image = CheckPlatform<CairoImage>(image, GetPlatformId());
  cairo_save(cairo_);
  cairo_set_source_surface(cairo_, cairo_image->GetCairoSurface(), 0, 0);
  cairo_new_path(cairo_);
  cairo_rectangle(cairo_, offset.x, offset.y, image->GetWidth(),
                  image->GetHeight());
  cairo_fill(cairo_);
  cairo_restore(cairo_);
}

void CairoPainter::PushLayer(const Rect& bounds) {
  CheckValidation();
  layer_stack_.push_back(bounds);
  cairo_reset_clip(cairo_);
  cairo_new_path(cairo_);
  cairo_rectangle(cairo_, bounds.left, bounds.top, bounds.width, bounds.height);
  cairo_clip(cairo_);
}

void CairoPainter::PopLayer() {
  CheckValidation();
  layer_stack_.pop_back();
  cairo_reset_clip(cairo_);
  if (!layer_stack_.empty()) {
    auto clip = layer_stack_.back();
    cairo_new_path(cairo_);
    cairo_rectangle(cairo_, clip.left, clip.top, clip.width, clip.height);
    cairo_clip(cairo_);
  }
}

void CairoPainter::PushState() {
  CheckValidation();
  cairo_save(cairo_);
}

void CairoPainter::PopState() {
  CheckValidation();
  cairo_restore(cairo_);
}

void CairoPainter::EndDraw() {
  if (cairo_surface_ != nullptr) {
    CRU_LOG_TAG_DEBUG("Flush cairo painter.");
    cairo_surface_flush(cairo_surface_);
    cairo_device_t* device = cairo_surface_get_device(cairo_surface_);
    if (device) {
      cairo_device_flush(device);
    }
  }
  valid_ = false;
}

void CairoPainter::CheckValidation() {
  if (!valid_) {
    throw ReuseException("Painter already ended drawing.");
  }
}
}  // namespace cru::platform::graphics::cairo
