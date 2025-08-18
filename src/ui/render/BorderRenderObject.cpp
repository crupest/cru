#include "cru/ui/render/BorderRenderObject.h"

#include "../Helper.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/render/RenderObject.h"

#include <algorithm>

namespace cru::ui::render {
BorderRenderObject::BorderRenderObject() { RecreateGeometry(); }

BorderRenderObject::~BorderRenderObject() {}

void BorderRenderObject::ApplyBorderStyle(
    const style::ApplyBorderStyleInfo& style) {
  if (style.border_brush) border_brush_ = *style.border_brush;
  if (style.border_thickness) border_thickness_ = *style.border_thickness;
  if (style.border_radius) border_radius_ = *style.border_radius;
  if (style.foreground_brush) foreground_brush_ = *style.foreground_brush;
  if (style.background_brush) background_brush_ = *style.background_brush;
  InvalidateLayout();
}

void BorderRenderObject::SetBorderEnabled(bool enabled) {
  if (is_border_enabled_ == enabled) return;
  is_border_enabled_ = enabled;
  InvalidateLayout();
}

void BorderRenderObject::SetBorderBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  if (brush == border_brush_) return;
  border_brush_ = std::move(brush);
  InvalidatePaint();
}

void BorderRenderObject::SetBorderThickness(const Thickness thickness) {
  if (thickness == border_thickness_) return;
  border_thickness_ = thickness;
  InvalidateLayout();
}

void BorderRenderObject::SetBorderRadius(const CornerRadius radius) {
  if (radius == border_radius_) return;
  border_radius_ = radius;
  RecreateGeometry();
}

void BorderRenderObject::SetForegroundBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  if (brush == foreground_brush_) return;
  foreground_brush_ = std::move(brush);
  InvalidatePaint();
}

void BorderRenderObject::SetBackgroundBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  if (brush == background_brush_) return;
  background_brush_ = std::move(brush);
  InvalidatePaint();
}

RenderObject* BorderRenderObject::HitTest(const Point& point) {
  if (auto child = GetChild()) {
    const auto result = child->HitTest(point - child->GetOffset());
    if (result != nullptr) {
      return result;
    }
  }

  if (is_border_enabled_) {
    return border_outer_geometry_->FillContains(point) ? this : nullptr;
  } else {
    return GetPaddingRect().IsPointInside(point) ? this : nullptr;
  }
}

void BorderRenderObject::Draw(platform::graphics::IPainter* painter) {
  if constexpr (debug_flags::draw) {
    CRU_LOG_TAG_DEBUG(
        u"BorderRenderObject draw, background: {}, foreground: {}.",
        background_brush_ == nullptr ? u"NONE"
                                     : background_brush_->GetDebugString(),
        foreground_brush_ == nullptr ? u"NONE"
                                     : foreground_brush_->GetDebugString());
  }

  if (background_brush_ != nullptr)
    painter->FillGeometry(border_inner_geometry_.get(),
                          background_brush_.get());

  if (is_border_enabled_) {
    if (border_brush_ == nullptr) {
      CRU_LOG_TAG_WARN(u"Border is enabled but border brush is null.");
    } else {
      painter->FillGeometry(geometry_.get(), border_brush_.get());
    }
  }

  if (auto child = GetChild()) {
    painter->PushState();
    painter->ConcatTransform(Matrix::Translation(child->GetOffset()));
    child->Draw(painter);
    painter->PopState();
  }

  if (foreground_brush_ != nullptr)
    painter->FillGeometry(border_inner_geometry_.get(),
                          foreground_brush_.get());
}

Size BorderRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                          const MeasureSize& preferred_size) {
  if (auto child = GetChild()) {
    child->Measure(requirement, preferred_size);
    return child->GetDesiredSize();
  } else {
    return preferred_size.GetSizeOr0();
  }
}

Size BorderRenderObject::OnMeasureContent1(const BoxConstraint& constraint) {
  auto child = GetChild();
  if (child == nullptr) return constraint.min;
  auto child_size = child->Measure1(BoxConstraint::kNotLimit);
  return constraint.Coerce(child_size);
}

void BorderRenderObject::OnLayoutContent(const Rect& content_rect) {
  if (auto child = GetChild()) {
    child->Layout(content_rect.GetLeftTop());
  }
}

void BorderRenderObject::OnResize(const Size& new_size) { RecreateGeometry(); }

Thickness BorderRenderObject::GetTotalSpaceThickness() const {
  return is_border_enabled_
             ? RenderObject::GetTotalSpaceThickness() + GetBorderThickness()
             : RenderObject::GetTotalSpaceThickness();
}

Thickness BorderRenderObject::GetInnerSpaceThickness() const {
  return is_border_enabled_
             ? RenderObject::GetInnerSpaceThickness() + GetBorderThickness()
             : RenderObject::GetInnerSpaceThickness();
}

Rect BorderRenderObject::GetPaddingRect() const {
  const auto size = GetSize();
  Rect rect{Point{}, size};
  rect = rect.Shrink(GetMargin());
  if (is_border_enabled_) rect = rect.Shrink(border_thickness_);
  rect.left = std::min(rect.left, size.width);
  rect.top = std::min(rect.top, size.height);
  rect.width = std::max(rect.width, 0.0f);
  rect.height = std::max(rect.height, 0.0f);
  return rect;
}

Rect BorderRenderObject::GetContentRect() const {
  const auto size = GetDesiredSize();
  Rect rect{Point{}, size};
  rect = rect.Shrink(GetMargin());
  if (is_border_enabled_) rect = rect.Shrink(border_thickness_);
  rect = rect.Shrink(GetPadding());
  rect.left = std::min(rect.left, size.width);
  rect.top = std::min(rect.top, size.height);
  rect.width = std::max(rect.width, 0.0f);
  rect.height = std::max(rect.height, 0.0f);
  return rect;
}

void BorderRenderObject::RecreateGeometry() {
  geometry_.reset();
  border_outer_geometry_.reset();

  Thickness t = border_thickness_;
  t.left /= 2.0;
  t.top /= 2.0;
  t.right /= 2.0;
  t.bottom /= 2.0;
  const CornerRadius& r = border_radius_;

  CornerRadius outer_radius(r.left_top + Point{t.left, t.top},
                            r.right_top + Point{t.right, t.top},
                            r.left_bottom + Point{t.left, t.bottom},
                            r.right_bottom + Point{t.right, t.bottom});

  CornerRadius inner_radius(r.left_top - Point{t.left, t.top},
                            r.right_top - Point{t.right, t.top},
                            r.left_bottom - Point{t.left, t.bottom},
                            r.right_bottom - Point{t.right, t.bottom});

  auto f = [](platform::graphics::IGeometryBuilder* builder, const Rect& rect,
              const CornerRadius& corner) {
    builder->MoveTo(Point(rect.left + corner.left_top.x, rect.top));
    builder->LineTo(Point(rect.GetRight() - corner.right_top.x, rect.top));
    builder->QuadraticBezierTo(
        Point(rect.GetRight(), rect.top),
        Point(rect.GetRight(), rect.top + corner.right_top.y));
    builder->LineTo(
        Point(rect.GetRight(), rect.GetBottom() - corner.right_bottom.y));
    builder->QuadraticBezierTo(
        Point(rect.GetRight(), rect.GetBottom()),
        Point(rect.GetRight() - corner.right_bottom.x, rect.GetBottom()));
    builder->LineTo(Point(rect.left + corner.left_bottom.x, rect.GetBottom()));
    builder->QuadraticBezierTo(
        Point(rect.left, rect.GetBottom()),
        Point(rect.left, rect.GetBottom() - corner.left_bottom.y));
    builder->LineTo(Point(rect.left, rect.top + corner.left_top.y));
    builder->QuadraticBezierTo(Point(rect.left, rect.top),
                               Point(rect.left + corner.left_top.x, rect.top));
    builder->CloseFigure(true);
  };

  const auto size = GetDesiredSize();
  const auto margin = GetMargin();
  const Rect outer_rect{margin.left, margin.top,
                        size.width - margin.GetHorizontalTotal(),
                        size.height - margin.GetVerticalTotal()};
  const auto graph_factory = GetGraphicsFactory();
  std::unique_ptr<platform::graphics::IGeometryBuilder> builder{
      graph_factory->CreateGeometryBuilder()};
  f(builder.get(), outer_rect, outer_radius);
  border_outer_geometry_ = builder->Build();
  builder.reset();

  const Rect inner_rect = outer_rect.Shrink(border_thickness_);

  builder = graph_factory->CreateGeometryBuilder();
  f(builder.get(), inner_rect, inner_radius);
  border_inner_geometry_ = builder->Build();
  builder.reset();

  builder = graph_factory->CreateGeometryBuilder();
  f(builder.get(), outer_rect, outer_radius);
  f(builder.get(), inner_rect, inner_radius);
  geometry_ = builder->Build();
  builder.reset();
}

String BorderRenderObject::GetName() const { return u"BorderRenderObject"; }
}  // namespace cru::ui::render
