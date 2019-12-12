#include "cru/ui/render/border_render_object.hpp"

#include "../helper.hpp"
#include "cru/common/logger.hpp"
#include "cru/platform/graph/factory.hpp"
#include "cru/platform/graph/geometry.hpp"
#include "cru/platform/graph/util/painter.hpp"

#include <algorithm>
#include <cassert>

namespace cru::ui::render {
BorderRenderObject::BorderRenderObject() {
  SetChildMode(ChildMode::Single);
  RecreateGeometry();
}

BorderRenderObject::~BorderRenderObject() {}

void BorderRenderObject::Draw(platform::graph::IPainter* painter) {
  if (background_brush_ != nullptr)
    painter->FillGeometry(border_inner_geometry_.get(),
                          background_brush_.get());
  if (is_border_enabled_) {
    if (border_brush_ == nullptr) {
      log::Warn("Border is enabled but brush is null");
    } else {
      painter->FillGeometry(geometry_.get(), border_brush_.get());
    }
  }
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    platform::graph::util::WithTransform(
        painter, platform::Matrix::Translation(offset.x, offset.y),
        [child](auto p) { child->Draw(p); });
  }
  if (foreground_brush_ != nullptr)
    painter->FillGeometry(border_inner_geometry_.get(),
                          foreground_brush_.get());
}

RenderObject* BorderRenderObject::HitTest(const Point& point) {
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    Point p{point.x - offset.x, point.y - offset.y};
    const auto result = child->HitTest(p);
    if (result != nullptr) {
      return result;
    }
  }

  if (is_border_enabled_) {
    const auto contains =
        border_outer_geometry_->FillContains(Point{point.x, point.y});
    return contains ? this : nullptr;
  } else {
    const auto margin = GetMargin();
    const auto size = GetSize();
    return Rect{margin.left, margin.top,
                std::max(size.width - margin.GetHorizontalTotal(), 0.0f),
                std::max(size.height - margin.GetVerticalTotal(), 0.0f)}
                   .IsPointInside(point)
               ? this
               : nullptr;
  }
}

void BorderRenderObject::OnMeasureCore(const Size& available_size) {
  const auto margin = GetMargin();
  const auto padding = GetPadding();
  Size margin_border_padding_size{
      margin.GetHorizontalTotal() + padding.GetHorizontalTotal(),
      margin.GetVerticalTotal() + padding.GetVerticalTotal()};

  if (is_border_enabled_) {
    margin_border_padding_size.width += border_thickness_.GetHorizontalTotal();
    margin_border_padding_size.height += border_thickness_.GetVerticalTotal();
  }

  auto coerced_margin_border_padding_size = margin_border_padding_size;
  if (coerced_margin_border_padding_size.width > available_size.width) {
    log::Warn(
        "Measure: horizontal length of padding, border and margin is bigger "
        "than available length.");
    coerced_margin_border_padding_size.width = available_size.width;
  }
  if (coerced_margin_border_padding_size.height > available_size.height) {
    log::Warn(
        "Measure: vertical length of padding, border and margin is bigger "
        "than available length.");
    coerced_margin_border_padding_size.height = available_size.height;
  }

  const auto coerced_content_available_size =
      available_size - coerced_margin_border_padding_size;

  const auto actual_content_size =
      OnMeasureContent(coerced_content_available_size);

  SetPreferredSize(coerced_margin_border_padding_size + actual_content_size);
}

void BorderRenderObject::OnLayoutCore(const Rect& rect) {
  const auto margin = GetMargin();
  const auto padding = GetPadding();
  Size margin_border_padding_size{
      margin.GetHorizontalTotal() + padding.GetHorizontalTotal(),
      margin.GetVerticalTotal() + padding.GetVerticalTotal()};

  if (is_border_enabled_) {
    margin_border_padding_size.width += border_thickness_.GetHorizontalTotal();
    margin_border_padding_size.height += border_thickness_.GetVerticalTotal();
  }

  const auto content_available_size =
      rect.GetSize() - margin_border_padding_size;
  auto coerced_content_available_size = content_available_size;

  if (coerced_content_available_size.width < 0) {
    log::Warn(
        "Layout: horizontal length of padding, border and margin is bigger "
        "than available length.");
    coerced_content_available_size.width = 0;
  }
  if (coerced_content_available_size.height < 0) {
    log::Warn(
        "Layout: vertical length of padding, border and margin is bigger "
        "than available length.");
    coerced_content_available_size.height = 0;
  }

  OnLayoutContent(
      Rect{margin.left + (is_border_enabled_ ? border_thickness_.left : 0) +
               padding.left,
           margin.top + (is_border_enabled_ ? border_thickness_.top : 0) +
               padding.top,
           coerced_content_available_size.width,
           coerced_content_available_size.height});
}

Size BorderRenderObject::OnMeasureContent(const Size& available_size) {
  const auto child = GetChild();
  if (child) {
    child->Measure(available_size);
    return child->GetPreferredSize();
  } else {
    return Size{};
  }
}

void BorderRenderObject::OnLayoutContent(const Rect& content_rect) {
  const auto child = GetChild();
  if (child) {
    child->Layout(content_rect);
  }
}

void BorderRenderObject::OnAfterLayout() { RecreateGeometry(); }

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

  auto f = [](platform::graph::IGeometryBuilder* builder, const Rect& rect,
              const CornerRadius& corner) {
    builder->BeginFigure(Point(rect.left + corner.left_top.x, rect.top));
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

  const auto size = GetSize();
  const auto margin = GetMargin();
  const Rect outer_rect{margin.left, margin.top,
                        size.width - margin.GetHorizontalTotal(),
                        size.height - margin.GetVerticalTotal()};
  const auto graph_factory = GetGraphFactory();
  std::unique_ptr<platform::graph::IGeometryBuilder> builder{
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
}  // namespace cru::ui::render
