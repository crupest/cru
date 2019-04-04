 #include "cru/ui/render/border_render_object.hpp"

#include "cru/platform/debug.hpp"
#include "cru/platform/geometry.hpp"
#include "cru/platform/graph_factory.hpp"
#include "cru/platform/painter_util.hpp"
#include "cru/platform/ui_applicaition.hpp"

#include <algorithm>
#include <cassert>

namespace cru::ui::render {
BorderRenderObject::BorderRenderObject(std::shared_ptr<platform::Brush> brush) {
  assert(brush);
  this->border_brush_ = std::move(brush);
  RecreateGeometry();
}

void BorderRenderObject::Draw(platform::Painter* painter) {
  painter->FillGeometry(geometry_.get(), border_brush_.get());
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    platform::util::WithTransform(
        painter, platform::Matrix::Translation(offset.x, offset.y),
        [child](auto p) { child->Draw(p); });
  }
}

RenderObject* BorderRenderObject::HitTest(const Point& point) {
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    Point p{point.x - offset.x, point.y - offset.y};
    const auto result = child->HitTest(point);
    if (result != nullptr) {
      return result;
    }
  }

  if (is_enabled_) {
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

void BorderRenderObject::OnAddChild(RenderObject* new_child, int position) {
  assert(GetChildren().size() == 1);
}

void BorderRenderObject::OnSizeChanged(const Size& old_size,
                                       const Size& new_size) {
  RecreateGeometry();
}

void BorderRenderObject::OnMeasureCore(const Size& available_size) {
  const auto margin = GetMargin();
  const auto padding = GetPadding();
  Size margin_border_padding_size{
      margin.GetHorizontalTotal() + padding.GetHorizontalTotal(),
      margin.GetVerticalTotal() + padding.GetVerticalTotal()};

  if (is_enabled_) {
    margin_border_padding_size.width += border_thickness_.GetHorizontalTotal();
    margin_border_padding_size.height += border_thickness_.GetVerticalTotal();
  }

  auto coerced_margin_border_padding_size = margin_border_padding_size;
  if (coerced_margin_border_padding_size.width > available_size.width) {
    platform::debug::DebugMessage(
        L"Measure: horizontal length of padding, border and margin is bigger "
        L"than available length.");
    coerced_margin_border_padding_size.width = available_size.width;
  }
  if (coerced_margin_border_padding_size.height > available_size.height) {
    platform::debug::DebugMessage(
        L"Measure: vertical length of padding, border and margin is bigger "
        L"than available length.");
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

  if (is_enabled_) {
    margin_border_padding_size.width += border_thickness_.GetHorizontalTotal();
    margin_border_padding_size.height += border_thickness_.GetVerticalTotal();
  }

  const auto content_available_size =
      rect.GetSize() - margin_border_padding_size;
  auto coerced_content_available_size = content_available_size;

  if (coerced_content_available_size.width < 0) {
    platform::debug::DebugMessage(
        L"Layout: horizontal length of padding, border and margin is bigger "
        L"than available length.");
    coerced_content_available_size.width = 0;
  }
  if (coerced_content_available_size.height < 0) {
    platform::debug::DebugMessage(
        L"Layout: vertical length of padding, border and margin is bigger "
        L"than "
        L"available length.");
    coerced_content_available_size.height = 0;
  }

  OnLayoutContent(Rect{
      margin.left + (is_enabled_ ? border_thickness_.left : 0) + padding.left,
      margin.top + (is_enabled_ ? border_thickness_.top : 0) + padding.top,
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

void BorderRenderObject::RecreateGeometry() {
  geometry_.reset();
  border_outer_geometry_.reset();

  auto f = [](platform::GeometryBuilder* builder, const Rect& rect,
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
  const auto graph_factory =
      platform::UiApplication::GetInstance()->GetGraphFactory();
  std::unique_ptr<platform::GeometryBuilder> builder{graph_factory->CreateGeometryBuilder()};
  f(builder.get(), outer_rect, corner_radius_);
  border_outer_geometry_.reset(builder->Build());
  builder.reset();

  const Rect inner_rect = outer_rect.Shrink(border_thickness_);
  builder.reset(graph_factory->CreateGeometryBuilder());
  f(builder.get(), outer_rect, corner_radius_);
  f(builder.get(), inner_rect, corner_radius_);
  geometry_.reset(builder->Build());
  builder.reset();
}
}  // namespace cru::ui::render
