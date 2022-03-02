#include "cru/ui/render/GeometryRenderObject.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/ui/render/RenderObject.h"

namespace cru::ui::render {
GeometryRenderObject::GeometryRenderObject() {}

GeometryRenderObject::~GeometryRenderObject() {}

std::shared_ptr<platform::graphics::IGeometry>
GeometryRenderObject::GetGeometry() const {
  return geometry_;
}

void GeometryRenderObject::SetGeometry(
    std::shared_ptr<platform::graphics::IGeometry> geometry,
    std::optional<Rect> view_port) {
  geometry_ = std::move(geometry);
  if (view_port) {
    view_port_ = *view_port;
  } else {
    view_port_ = geometry_ ? geometry_->GetBounds() : Rect{};
  }
}

Rect GeometryRenderObject::GetViewPort() const { return view_port_; }

void GeometryRenderObject::SetViewPort(const Rect& view_port) {
  view_port_ = view_port;
  InvalidatePaint();
}

std::shared_ptr<platform::graphics::IBrush> GeometryRenderObject::GetFillBrush()
    const {
  return fill_brush_;
}

void GeometryRenderObject::SetFillBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  fill_brush_ = std::move(brush);
  InvalidatePaint();
}

std::shared_ptr<platform::graphics::IBrush>
GeometryRenderObject::GetStrokeBrush() const {
  return stroke_brush_;
}

void GeometryRenderObject::SetStrokeBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  stroke_brush_ = std::move(brush);
  InvalidatePaint();
}

float GeometryRenderObject::GetStrokeWidth() const { return stroke_width_; }

void GeometryRenderObject::SetStrokeWidth(float width) {
  stroke_width_ = width;
  InvalidatePaint();
}

void GeometryRenderObject::Draw(platform::graphics::IPainter* painter) {
  if (!geometry_) return;

  painter->PushState();

  auto content_rect = GetContentRect();

  painter->ConcatTransform(Matrix::Translation(content_rect.GetLeftTop()));

  painter->ConcatTransform(
      Matrix::Translation(view_port_.GetLeftTop().Negate()) *
      Matrix::Scale(content_rect.width / view_port_.width,
                    content_rect.height / view_port_.height));

  if (fill_brush_) {
    painter->FillGeometry(geometry_.get(), fill_brush_.get());
  }

  if (stroke_brush_ && stroke_width_) {
    painter->StrokeGeometry(geometry_.get(), stroke_brush_.get(),
                            stroke_width_);
  }

  painter->PopState();
}

RenderObject* GeometryRenderObject::HitTest(const Point& point) {
  return GetPaddingRect().IsPointInside(point) ? this : nullptr;
}

Size GeometryRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement, const MeasureSize& preferred_size) {
  Size result = GetViewPort().GetSize();

  if (preferred_size.width.IsSpecified()) {
    result.width = preferred_size.width.GetLengthOrUndefined();
  }

  if (preferred_size.height.IsSpecified()) {
    result.height = preferred_size.height.GetLengthOrUndefined();
  }

  return requirement.Coerce(result);
}

void GeometryRenderObject::OnLayoutContent(const Rect& content_rect) {}
}  // namespace cru::ui::render
