#include "cru/ui/render/BorderRenderObject.hpp"

#include "../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/graphics/Geometry.hpp"
#include "cru/platform/graphics/util/Painter.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"
#include "gsl/gsl_assert"

#include <algorithm>

namespace cru::ui::render {
BorderRenderObject::BorderRenderObject() {
  SetChildMode(ChildMode::Single);
  RecreateGeometry();
}

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

RenderObject* BorderRenderObject::HitTest(const Point& point) {
  if (const auto child = GetSingleChild()) {
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

void BorderRenderObject::OnDrawCore(platform::graphics::IPainter* painter) {
  if (background_brush_ != nullptr)
    painter->FillGeometry(border_inner_geometry_.get(),
                          background_brush_.get());
  if (is_border_enabled_) {
    if (border_brush_ == nullptr) {
      log::TagWarn(log_tag, u"Border is enabled but border brush is null.");
    } else {
      painter->FillGeometry(geometry_.get(), border_brush_.get());
    }
  }

  DefaultDrawContent(painter);

  if (foreground_brush_ != nullptr)
    painter->FillGeometry(border_inner_geometry_.get(),
                          foreground_brush_.get());

  DefaultDrawChildren(painter);
}

Size BorderRenderObject::OnMeasureCore(const MeasureRequirement& requirement,
                                       const MeasureSize& preferred_size) {
  if (!is_border_enabled_) {
    return RenderObject::OnMeasureCore(requirement, preferred_size);
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

  if (!requirement.max.width.IsNotSpecified()) {
    const auto max_width = requirement.max.width.GetLengthOrMax();
    if (coerced_space_size.width > max_width) {
      log::TagWarn(log_tag,
                   u"(Measure) Horizontal length of padding, border and margin "
                   u"is bigger than required max length.");
      coerced_space_size.width = max_width;
    }
    content_requirement.max.width = max_width - coerced_space_size.width;
  }

  if (!requirement.min.width.IsNotSpecified()) {
    const auto min_width = requirement.min.width.GetLengthOr0();
    content_requirement.min.width = std::max(0.f, min_width - space_size.width);
  }

  if (!requirement.max.height.IsNotSpecified()) {
    const auto max_height = requirement.max.height.GetLengthOrMax();
    if (coerced_space_size.height > max_height) {
      log::TagWarn(
          log_tag,
          u"(Measure) Vertical length of padding, border and margin is "
          u"bigger than required max length.");
      coerced_space_size.height = max_height;
    }
    content_requirement.max.height = max_height - coerced_space_size.height;
  }

  if (!requirement.min.height.IsNotSpecified()) {
    const auto min_height = requirement.min.height.GetLengthOr0();
    content_requirement.min.height =
        std::max(0.f, min_height - space_size.height);
  }

  MeasureSize content_preferred_size =
      content_requirement.Coerce(preferred_size.Minus(space_size));

  const auto content_size =
      OnMeasureContent(content_requirement, content_preferred_size);

  return coerced_space_size + content_size;
}

void BorderRenderObject::OnLayoutCore() {
  if (!is_border_enabled_) {
    return RenderObject::OnLayoutCore();
  }

  const auto margin = GetMargin();
  const auto padding = GetPadding();
  Size space_size{margin.GetHorizontalTotal() + padding.GetHorizontalTotal() +
                      border_thickness_.GetHorizontalTotal(),
                  margin.GetVerticalTotal() + padding.GetVerticalTotal() +
                      border_thickness_.GetVerticalTotal()};

  const auto size = GetSize();

  auto content_size = size - space_size;

  if (content_size.width < 0) {
    content_size.width = 0;
  }
  if (content_size.height < 0) {
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

Size BorderRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                          const MeasureSize& preferred_size) {
  const auto child = GetSingleChild();
  if (child) {
    child->Measure(requirement, preferred_size);
    return child->GetSize();
  } else {
    return Size{};
  }
}

void BorderRenderObject::OnLayoutContent(const Rect& content_rect) {
  const auto child = GetSingleChild();
  if (child) {
    child->Layout(content_rect.GetLeftTop());
  }
}

void BorderRenderObject::OnAfterLayout() { RecreateGeometry(); }

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
  const auto size = GetSize();
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
}  // namespace cru::ui::render
