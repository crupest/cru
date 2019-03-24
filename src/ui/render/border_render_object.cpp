#include "border_render_object.hpp"

#include <d2d1_1.h>
#include <wrl/client.h>
#include <algorithm>

#include "cru_debug.hpp"
#include "exception.hpp"
#include "graph/graph_manager.hpp"
#include "graph/graph_util.hpp"
#include "util/com_util.hpp"

namespace cru::ui::render {
BorderRenderObject::BorderRenderObject(ID2D1Brush* brush) {
  assert(brush);
  brush->AddRef();
  this->border_brush_ = brush;
  try {
    RecreateGeometry();
  } catch (...) {
    brush->Release();
    throw;
  }
}

BorderRenderObject::~BorderRenderObject() {
  util::SafeRelease(border_brush_);
  util::SafeRelease(geometry_);
  util::SafeRelease(border_outer_geometry_);
}

void BorderRenderObject::SetBrush(ID2D1Brush* new_brush) {
  assert(new_brush);
  util::SafeRelease(border_brush_);
  new_brush->AddRef();
  border_brush_ = new_brush;
}

void BorderRenderObject::Draw(ID2D1RenderTarget* render_target) {
  render_target->FillGeometry(geometry_, border_brush_);
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    graph::WithTransform(render_target,
                         D2D1::Matrix3x2F::Translation(offset.x, offset.y),
                         [child](auto rt) { child->Draw(rt); });
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
    BOOL contains;
    ThrowIfFailed(border_outer_geometry_->FillContainsPoint(
        D2D1::Point2F(point.x, point.y), D2D1::Matrix3x2F::Identity(),
        &contains));
    return contains != 0 ? this : nullptr;
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
    debug::DebugMessage(
        L"Measure: horizontal length of padding, border and margin is bigger "
        L"than available length.");
    coerced_margin_border_padding_size.width = available_size.width;
  }
  if (coerced_margin_border_padding_size.height > available_size.height) {
    debug::DebugMessage(
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
    debug::DebugMessage(
        L"Layout: horizontal length of padding, border and margin is bigger "
        L"than available length.");
    coerced_content_available_size.width = 0;
  }
  if (coerced_content_available_size.height < 0) {
    debug::DebugMessage(
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
    return Size::Zero();
  }
}

void BorderRenderObject::OnLayoutContent(const Rect& content_rect) {
  const auto child = GetChild();
  if (child) {
    child->Layout(content_rect);
  }
}

void BorderRenderObject::RecreateGeometry() {
  util::SafeRelease(geometry_);
  util::SafeRelease(border_outer_geometry_);

  const auto d2d_factory = graph::GraphManager::GetInstance()->GetD2D1Factory();

  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
  ThrowIfFailed(d2d_factory->CreatePathGeometry(&geometry));

  Microsoft::WRL::ComPtr<ID2D1PathGeometry> border_outer_geometry;
  ThrowIfFailed(d2d_factory->CreatePathGeometry(&border_outer_geometry));

  Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
  auto f = [&sink](const Rect& rect, const CornerRadius& corner) {
    sink->BeginFigure(D2D1::Point2F(rect.left + corner.left_top.x, rect.top),
                      D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(
        D2D1::Point2F(rect.GetRight() - corner.right_top.x, rect.top));
    sink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
        D2D1::Point2F(rect.GetRight(), rect.top),
        D2D1::Point2F(rect.GetRight(), rect.top + corner.right_top.y)));
    sink->AddLine(D2D1::Point2F(rect.GetRight(),
                                rect.GetBottom() - corner.right_bottom.y));
    sink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
        D2D1::Point2F(rect.GetRight(), rect.GetBottom()),
        D2D1::Point2F(rect.GetRight() - corner.right_bottom.x,
                      rect.GetBottom())));
    sink->AddLine(
        D2D1::Point2F(rect.left + corner.left_bottom.x, rect.GetBottom()));
    sink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
        D2D1::Point2F(rect.left, rect.GetBottom()),
        D2D1::Point2F(rect.left, rect.GetBottom() - corner.left_bottom.y)));
    sink->AddLine(D2D1::Point2F(rect.left, rect.top + corner.left_top.y));
    sink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
        D2D1::Point2F(rect.left, rect.top),
        D2D1::Point2F(rect.left + corner.left_top.x, rect.top)));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
  };

  const auto size = GetSize();
  const auto margin = GetMargin();
  const Rect outer_rect{margin.left, margin.top,
                        size.width - margin.GetHorizontalTotal(),
                        size.height - margin.GetVerticalTotal()};
  ThrowIfFailed(border_outer_geometry->Open(&sink));
  f(outer_rect, corner_radius_);
  ThrowIfFailed(sink->Close());
  sink = nullptr;

  const Rect inner_rect = outer_rect.Shrink(border_thickness_);
  ThrowIfFailed(geometry->Open(&sink));
  f(outer_rect, corner_radius_);
  f(inner_rect, corner_radius_);
  ThrowIfFailed(sink->Close());
  sink = nullptr;

  geometry_ = geometry.Detach();
  border_outer_geometry_ = border_outer_geometry.Detach();
}
}  // namespace cru::ui::render
