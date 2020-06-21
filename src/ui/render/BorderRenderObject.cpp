#include "cru/ui/render/BorderRenderObject.hpp"

#include "../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/platform/graph/Factory.hpp"
#include "cru/platform/graph/Geometry.hpp"
#include "cru/platform/graph/util/Painter.hpp"

#include <algorithm>

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

void BorderRenderObject::SetBorderStyle(const BorderStyle& style) {
  border_brush_ = style.border_brush;
  border_thickness_ = style.border_thickness;
  border_radius_ = style.border_radius;
  foreground_brush_ = style.foreground_brush;
  background_brush_ = style.background_brush;
  InvalidateLayout();
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

Size BorderRenderObject::OnMeasureCore(const MeasureRequirement& requirement) {
  if (!is_border_enabled_) {
    return RenderObject::OnMeasureCore(requirement);
  }

  const auto margin = GetMargin();
  const auto padding = GetPadding();
  const Size space_size{margin.GetHorizontalTotal() +
                            padding.GetHorizontalTotal() +
                            border_thickness_.GetHorizontalTotal(),
                        margin.GetVerticalTotal() + padding.GetVerticalTotal() +
                            border_thickness_.GetVerticalTotal()};

  auto coerced_space_size = space_size;

  MeasureRequirement content_requirement = requirement;

  if (!requirement.max_width.IsInfinate()) {
    const auto max_width = requirement.max_width.GetLength();
    if (coerced_space_size.width > max_width) {
      log::Warn(
          "Measure: horizontal length of padding and margin is bigger than "
          "available length.");
      coerced_space_size.width = max_width;
    }
    content_requirement.max_width = max_width - coerced_space_size.width;
  }

  if (!requirement.max_height.IsInfinate()) {
    const auto max_height = requirement.max_height.GetLength();
    if (coerced_space_size.height > max_height) {
      log::Warn(
          "Measure: horizontal length of padding and margin is bigger than "
          "available length.");
      coerced_space_size.height = max_height;
    }
    content_requirement.max_height = max_height - coerced_space_size.height;
  }

  const auto content_size = OnMeasureContent(content_requirement);

  return coerced_space_size + content_size;
}  // namespace cru::ui::render

void BorderRenderObject::OnLayoutCore(const Size& size) {
  if (!is_border_enabled_) {
    return RenderObject::OnLayoutCore(size);
  }

  const auto margin = GetMargin();
  const auto padding = GetPadding();
  Size space_size{margin.GetHorizontalTotal() + padding.GetHorizontalTotal() +
                      border_thickness_.GetHorizontalTotal(),
                  margin.GetVerticalTotal() + padding.GetVerticalTotal() +
                      border_thickness_.GetVerticalTotal()};

  auto content_size = size - space_size;

  if (content_size.width < 0) {
    log::Warn(
        "Layout: horizontal length of padding, border and margin is bigger "
        "than available length.");
    content_size.width = 0;
  }
  if (content_size.height < 0) {
    log::Warn(
        "Layout: vertical length of padding, border and margin is bigger "
        "than available length.");
    content_size.height = 0;
  }

  Point lefttop{margin.left + padding.left + border_thickness_.left,
                margin.top + padding.top + border_thickness_.top};
  if (lefttop.x > size.width) {
    lefttop.x = size.width;
  }
  if (lefttop.y > size.height) {
    lefttop.y = size.height;
  }

  const Rect content_rect{lefttop, content_size};

  OnLayoutContent(content_rect);
}

Size BorderRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  const auto child = GetChild();
  if (child) {
    child->Measure(requirement);
    return child->GetMeasuredSize();
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
