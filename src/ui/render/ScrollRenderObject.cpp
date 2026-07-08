#include "cru/ui/render/ScrollRenderObject.h"

#include "cru/platform/GraphicsBase.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/Base.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/render/RenderObject.h"
#include "cru/ui/render/ScrollRenderObject.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace cru::ui::render {
constexpr float kLineHeight = 16;

constexpr float kScrollBarCollapseThumbWidth = 3;
constexpr float kScrollBarCollapsedTriggerExpandAreaWidth = 7;
constexpr float kScrollBarExpandWidth = 14;
constexpr float kScrollBarArrowHeight = 4;

namespace {
// This method assumes margin offset is already considered.
// It promises that it won't return negative value.
Point CoerceScroll(const Point& scroll_offset, const Size& content_size,
                   const Size& child_size) {
  Point result(scroll_offset);

  Point max_scroll(child_size - content_size);
  max_scroll.x = std::max(max_scroll.x, 0.f);
  max_scroll.y = std::max(max_scroll.y, 0.f);

  auto coerce = [](float& n, float max) {
    if (n < 0)
      n = 0;
    else if (n > max)
      n = max;
  };

  coerce(result.x, max_scroll.x);
  coerce(result.y, max_scroll.y);

  return result;
}
}  // namespace

ScrollRenderObject::ScrollRenderObject()
    : SingleChildRenderObject(kRenderObjectName) {}

RenderObject* ScrollRenderObject::HitTest(const Point& point) {
  if (auto child = GetChild()) {
    const auto offset = child->GetOffset();
    const auto r = child->HitTest(point - offset);
    if (r != nullptr) return r;
  }

  const auto rect = GetPaddingRect();
  return rect.IsPointInside(point) ? this : nullptr;
}  // namespace cru::ui::render

Point ScrollRenderObject::GetScrollOffset() {
  if (auto child = GetChild()) {
    return CoerceScroll(scroll_offset_, GetContentRect().GetSize(),
                        child->GetSize());
  } else {
    return scroll_offset_;
  }
}

ScrollState ScrollRenderObject::GetScrollState() {
  auto child = GetChild();
  auto view_rect = GetViewRect();
  return ScrollState{
      .offset = GetScrollOffset(),
      .view_size = GetViewRect().GetSize(),
      .child_size = child ? child->GetSize() : Size{},
  };
}

void ScrollRenderObject::SetScrollOffset(const Point& offset) {
  if (scroll_offset_ == offset) return;
  scroll_offset_ = offset;
  InvalidateLayout();
  RaiseScrollEvent();
}

void ScrollRenderObject::SetScrollOffset(std::optional<float> x,
                                         std::optional<float> y) {
  auto new_offset = scroll_offset_;

  if (x.has_value()) {
    new_offset.x = *x;
  }

  if (y.has_value()) {
    new_offset.y = *y;
  }

  SetScrollOffset(new_offset);
}

void ScrollRenderObject::ApplyScroll(const struct Scroll& scroll) {
  auto direction = scroll.direction;

  switch (scroll.kind) {
    case ScrollKind::Absolute:
      SetScrollOffset(direction, scroll.value);
      break;
    case ScrollKind::Relative:
      SetScrollOffset(direction,
                      GetScrollOffset(scroll.direction) + scroll.value);
      break;
    case ScrollKind::Page:
      SetScrollOffset(direction, GetScrollOffset(direction) +
                                     (direction == Direction::Horizontal
                                          ? GetViewRect().width
                                          : GetViewRect().height) *
                                         scroll.value);
      break;
    case ScrollKind::Line:
      SetScrollOffset(direction,
                      GetScrollOffset(direction) + kLineHeight * scroll.value);
      break;
    default:
      break;
  }
}

void ScrollRenderObject::ScrollToContain(const Rect& rect,
                                         const Thickness& margin) {
  std::optional<float> new_scroll_x;
  std::optional<float> new_scroll_y;

  Rect real_rect = rect.Expand(margin);

  Rect view_rect = GetViewRect();

  // horizontal
  if (real_rect.left < view_rect.left) {
    new_scroll_x = real_rect.left;
  } else if (real_rect.GetRight() > view_rect.GetRight()) {
    new_scroll_x = real_rect.GetRight() - view_rect.width;
  }

  // vertical
  if (real_rect.top < view_rect.top) {
    new_scroll_y = real_rect.top;
  } else if (real_rect.GetBottom() > view_rect.GetBottom()) {
    new_scroll_y = real_rect.GetBottom() - view_rect.height;
  }

  SetScrollOffset(new_scroll_x, new_scroll_y);
}

Size ScrollRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  if (auto child = GetChild()) {
    child->Measure({MeasureSize::NotSpecified(), MeasureSize::NotSpecified(),
                    MeasureSize::NotSpecified()});

    auto result =
        requirement.ExpandToSuggestAndCoerce(child->GetMeasureResultSize());

    return result;
  } else {
    return requirement.suggest.GetSizeOr0();
  }
}

void ScrollRenderObject::OnLayoutContent(const Rect& content_rect) {
  if (auto child = GetChild()) {
    child->Layout(content_rect.GetLeftTop() - GetScrollOffset());
  }
  RaiseScrollEvent();
}

void ScrollRenderObject::OnDraw(RenderObjectDrawContext& context) {
  auto painter = context.painter;
  if (auto child = GetChild()) {
    painter->PushLayer(this->GetContentRect());
    context.DrawChild(child);
    painter->PopLayer();
  }
}

void ScrollRenderObject::RaiseScrollEvent() {
  scroll_event_.Raise(GetScrollState());
}

std::string GenerateScrollBarThemeColorKey(ScrollBarBrushUsageKind usage,
                                           ScrollBarBrushStateKind state) {
  std::string result = "scrollbar.";
  switch (usage) {
    case ScrollBarBrushUsageKind::Arrow:
      result.append("arrow");
      break;
    case ScrollBarBrushUsageKind::ArrowBackground:
      result.append("arrow-background");
      break;
    case ScrollBarBrushUsageKind::Slot:
      result.append("slot");
      break;
    case ScrollBarBrushUsageKind::Thumb:
      result.append("thumb");
      break;
  }
  result.push_back('.');
  switch (state) {
    case ScrollBarBrushStateKind::Normal:
      result.append("normal");
      break;
    case ScrollBarBrushStateKind::Hover:
      result.append("hover");
      break;
    case ScrollBarBrushStateKind::Press:
      result.append("press");
      break;
    case ScrollBarBrushStateKind::Disable:
      result.append("disable");
      break;
  }
  result.append(".color");
  return result;
}

namespace {
/**
 * \brief Build a arrow geometry pointing to left direction.
 */
std::unique_ptr<platform::graphics::IGeometry> CreateScrollBarArrowGeometry() {
  auto geometry_builder = platform::gui::IUiApplication::GetInstance()
                              ->GetGraphicsFactory()
                              ->CreateGeometryBuilder();
  geometry_builder->MoveTo({-kScrollBarArrowHeight / 2, 0});
  geometry_builder->LineTo({kScrollBarArrowHeight / 2, kScrollBarArrowHeight});
  geometry_builder->LineTo({kScrollBarArrowHeight / 2, -kScrollBarArrowHeight});
  geometry_builder->CloseFigure(true);
  return geometry_builder->Build();
}

void DrawScrollBarArrow(platform::graphics::IPainter* painter,
                        platform::graphics::IGeometry* base_arrow_geometry,
                        const Rect& area,
                        platform::graphics::IBrush* arrow_brush, float angle) {
  auto geometry = base_arrow_geometry->Transform(Matrix::Rotation(angle));
  geometry = geometry->Transform(Matrix::Translation(area.GetCenter()));
  painter->FillGeometry(geometry.get(), arrow_brush);
}
}  // namespace

ScrollBarRenderObject::ScrollBarRenderObject(std::string name,
                                             Direction direction)
    : RenderObject(std::move(name)), direction_(direction) {
  arrow_geometry_ = CreateScrollBarArrowGeometry();
}

RenderObject* ScrollBarRenderObject::HitTest(const Point& point) {
  if (!IsEnabled()) {
    return nullptr;
  }

  auto size = GetSize();
  if (size.width <= 0 || size.height <= 0) return nullptr;
  if (IsExpanded()) {
    return Rect({0, 0}, size).IsPointInside(point) ? this : nullptr;
  } else {
    return GetCollapsedThumbRect()->IsPointInside(point) ? this : nullptr;
  }
}

void ScrollBarRenderObject::OnDraw(RenderObjectDrawContext& context) {
  if (is_enabled_) {
    DoDraw(context.painter, is_expanded_);
  }
}

void ScrollBarRenderObject::SetEnabled(bool value) {
  if (value == is_enabled_) return;
  is_enabled_ = value;
  InvalidateLayout();
}

void ScrollBarRenderObject::SetExpanded(bool value) {
  if (is_expanded_ == value) return;
  is_expanded_ = value;
  InvalidateLayout();
}

void ScrollBarRenderObject::SetScrollState(
    const ScrollPercentage& scroll_state) {
  if (scroll_state == scroll_state_) return;
  scroll_state_ = scroll_state;
  InvalidateLayout();
}

ScrollBarBrushStateKind ScrollBarRenderObject::GetBrushState(
    ScrollBarAreaKind area) {
  switch (area) {
    case ScrollBarAreaKind::UpArrow:
      return up_arrow_state_;
    case ScrollBarAreaKind::DownArrow:
      return down_arrow_state_;
    case ScrollBarAreaKind::UpSlot:
      return up_slot_state_;
    case ScrollBarAreaKind::DownSlot:
      return down_slot_state_;
    case ScrollBarAreaKind::Thumb:
      return thumb_state_;
    default:
      std::unreachable();
  }
}

ScrollBarBrushStateKind ScrollBarRenderObject::GetCalculatedBrushState(
    ScrollBarAreaKind area) {
  switch (area) {
    case ScrollBarAreaKind::UpArrow:
      if (!scroll_state_.CanScrollBackward())
        return ScrollBarBrushStateKind::Disable;
      break;
    case ScrollBarAreaKind::DownArrow:
      if (!scroll_state_.CanScrollForward())
        return ScrollBarBrushStateKind::Disable;
      break;
    case ScrollBarAreaKind::UpSlot:
    case ScrollBarAreaKind::DownSlot:
    case ScrollBarAreaKind::Thumb:
      break;
    default:
      std::unreachable();
  }

  return GetBrushState(area);
}

void ScrollBarRenderObject::SetBrushState(ScrollBarAreaKind area,
                                          ScrollBarBrushStateKind state) {
  bool changed = false;
  switch (area) {
    case ScrollBarAreaKind::UpArrow:
      if (up_arrow_state_ != state) {
        up_arrow_state_ = state;
        changed = true;
      }
      break;
    case ScrollBarAreaKind::DownArrow:
      if (down_arrow_state_ != state) {
        down_arrow_state_ = state;
        changed = true;
      }
      break;
    case ScrollBarAreaKind::UpSlot:
      if (up_slot_state_ != state) {
        up_slot_state_ = state;
        changed = true;
      }
      break;
    case ScrollBarAreaKind::DownSlot:
      if (down_slot_state_ != state) {
        down_slot_state_ = state;
        changed = true;
      }
      break;
    case ScrollBarAreaKind::Thumb:
      if (thumb_state_ != state) {
        thumb_state_ = state;
        changed = true;
      }
      break;
      break;
    default:
      std::unreachable();
  }
  if (changed) InvalidatePaint();
}

std::shared_ptr<platform::graphics::IBrush>
ScrollBarRenderObject::GetCollapsedThumbBrush() {
  return collapsed_thumb_brush_ ? collapsed_thumb_brush_
                                : ThemeManager::GetInstance()->GetResourceBrush(
                                      "scrollbar.collapse-thumb.color");
}

void ScrollBarRenderObject::SetCollapsedThumbBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  if (brush == collapsed_thumb_brush_) return;
  collapsed_thumb_brush_ = std::move(brush);
  InvalidatePaint();
}

std::shared_ptr<platform::graphics::IBrush> ScrollBarRenderObject::GetBrush(
    ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state) {
  auto b = brushes_[usage][state];
  return b ? b
           : ThemeManager::GetInstance()->GetResourceBrush(
                 GenerateScrollBarThemeColorKey(usage, state));
}

// Brush could be nullptr to use the theme brush.
void ScrollBarRenderObject::SetBrush(
    ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state,
    std::shared_ptr<platform::graphics::IBrush> brush) {
  if (brushes_[usage][state] == brush) return;
  brushes_[usage][state] = std::move(brush);
  InvalidatePaint();
}

void ScrollBarRenderObject::DoDraw(platform::graphics::IPainter* painter,
                                   bool is_expanded) {
  if (is_expanded) {
    auto thumb_rect = GetExpandedAreaRect(ScrollBarAreaKind::Thumb);
    if (thumb_rect) {
      auto thumb_state = GetCalculatedBrushState(ScrollBarAreaKind::Thumb);
      painter->FillRectangle(
          *thumb_rect,
          GetBrush(ScrollBarBrushUsageKind::Thumb, thumb_state).get());
    }

    auto up_slot_rect = GetExpandedAreaRect(ScrollBarAreaKind::UpSlot);
    auto up_slot_state = GetCalculatedBrushState(ScrollBarAreaKind::UpSlot);
    if (up_slot_rect)
      painter->FillRectangle(
          *up_slot_rect,
          GetBrush(ScrollBarBrushUsageKind::Slot, up_slot_state).get());

    auto down_slot_rect = GetExpandedAreaRect(ScrollBarAreaKind::DownSlot);
    auto down_slot_state = GetCalculatedBrushState(ScrollBarAreaKind::DownSlot);
    if (down_slot_rect)
      painter->FillRectangle(
          *down_slot_rect,
          GetBrush(ScrollBarBrushUsageKind::Slot, down_slot_state).get());

    auto up_arrow_rect = GetExpandedAreaRect(ScrollBarAreaKind::UpArrow);
    auto up_arrow_state = GetCalculatedBrushState(ScrollBarAreaKind::UpArrow);
    if (up_arrow_rect)
      this->DrawUpArrow(
          painter, *up_arrow_rect,
          GetBrush(ScrollBarBrushUsageKind::Arrow, up_arrow_state).get(),
          GetBrush(ScrollBarBrushUsageKind::ArrowBackground, up_arrow_state)
              .get());

    auto down_arrow_rect = GetExpandedAreaRect(ScrollBarAreaKind::DownArrow);
    auto down_arrow_state =
        GetCalculatedBrushState(ScrollBarAreaKind::DownArrow);
    if (down_arrow_rect)
      this->DrawDownArrow(
          painter, *down_arrow_rect,
          GetBrush(ScrollBarBrushUsageKind::Arrow, down_arrow_state).get(),
          GetBrush(ScrollBarBrushUsageKind::ArrowBackground, down_arrow_state)
              .get());
  } else {
    auto optional_rect = GetCollapsedThumbRect();
    if (optional_rect) {
      painter->FillRectangle(*optional_rect, GetCollapsedThumbBrush().get());
    }
  }
}

std::optional<ScrollBarAreaKind> ScrollBarRenderObject::ExpandedHitTest(
    const Point& point) {
  for (auto kind : kScrollBarAreaKindList) {
    auto rect = this->GetExpandedAreaRect(kind);
    if (rect) {
      if (rect->IsPointInside(point)) return kind;
    }
  }
  return std::nullopt;
}

HorizontalScrollBarRenderObject::HorizontalScrollBarRenderObject()
    : ScrollBarRenderObject(kRenderObjectName, Direction::Horizontal) {}

std::optional<Rect> HorizontalScrollBarRenderObject::GetExpandedAreaRect(
    ScrollBarAreaKind area_kind) {
  if (!IsEnabled()) return std::nullopt;

  const auto& scroll_state = GetScrollState();

  const auto height = kScrollBarExpandWidth;
  // Exclude arrow.
  const float scroll_area_length = GetSize().width - 2 * kScrollBarExpandWidth;

  switch (area_kind) {
    case ScrollBarAreaKind::UpArrow:
      return Rect{0, 0, kScrollBarExpandWidth, height};
    case ScrollBarAreaKind::DownArrow:
      return Rect{kScrollBarExpandWidth + scroll_area_length, 0,
                  kScrollBarExpandWidth, height};
    case ScrollBarAreaKind::UpSlot:
      return Rect{kScrollBarExpandWidth, 0,
                  scroll_area_length * scroll_state.start, height};
    case ScrollBarAreaKind::DownSlot:
      return Rect{
          kScrollBarExpandWidth + scroll_area_length * scroll_state.GetEnd(), 0,
          scroll_area_length * (1 - scroll_state.GetEnd()), height};
    case ScrollBarAreaKind::Thumb:
      return Rect{
          kScrollBarExpandWidth + scroll_area_length * scroll_state.start, 0,
          scroll_area_length * scroll_state.length, height};
    case ScrollBarAreaKind::Scroll:
      return Rect{kScrollBarExpandWidth, 0, scroll_area_length, height};
    default:
      std::unreachable();
  }
}

std::optional<Rect>
HorizontalScrollBarRenderObject::GetCollapsedTriggerExpandAreaRect() {
  if (!IsEnabled()) return std::nullopt;

  return Rect{0, GetSize().height - kScrollBarCollapsedTriggerExpandAreaWidth,
              GetSize().width, kScrollBarCollapsedTriggerExpandAreaWidth};
}

std::optional<Rect> HorizontalScrollBarRenderObject::GetCollapsedThumbRect() {
  auto rect = GetExpandedAreaRect(ScrollBarAreaKind::Thumb);
  if (!rect) return std::nullopt;
  rect->top = GetSize().height - kScrollBarCollapseThumbWidth;
  rect->height = kScrollBarCollapseThumbWidth;
  return rect;
}

Size HorizontalScrollBarRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  if (!IsEnabled()) {
    return Size{0, 0};
  }
  return Size{
      // Reduce the right bottom corner.
      std::max(0.f,
               requirement.max.width.GetLengthOr0() - kScrollBarExpandWidth),
      IsExpanded() ? kScrollBarExpandWidth : kScrollBarCollapseThumbWidth};
}

void HorizontalScrollBarRenderObject::DrawUpArrow(
    platform::graphics::IPainter* painter, const Rect& area,
    platform::graphics::IBrush* arrow_brush,
    platform::graphics::IBrush* background_brush) {
  painter->FillRectangle(area, background_brush);
  DrawScrollBarArrow(painter, arrow_geometry_.get(), area, arrow_brush, 0);
}

void HorizontalScrollBarRenderObject::DrawDownArrow(
    platform::graphics::IPainter* painter, const Rect& area,
    platform::graphics::IBrush* arrow_brush,
    platform::graphics::IBrush* background_brush) {
  painter->FillRectangle(area, background_brush);
  DrawScrollBarArrow(painter, arrow_geometry_.get(), area, arrow_brush, 180);
}

VerticalScrollBarRenderObject::VerticalScrollBarRenderObject()
    : ScrollBarRenderObject(kRenderObjectName, Direction::Vertical) {}

std::optional<Rect> VerticalScrollBarRenderObject::GetExpandedAreaRect(
    ScrollBarAreaKind area_kind) {
  if (!IsEnabled()) return std::nullopt;

  const auto& scroll_state = GetScrollState();

  const auto width = kScrollBarExpandWidth;
  // Without arrow.
  const float bar_area_length = GetSize().height - 2 * kScrollBarExpandWidth;

  switch (area_kind) {
    case ScrollBarAreaKind::UpArrow:
      return Rect{0, 0, width, kScrollBarExpandWidth};
    case ScrollBarAreaKind::DownArrow:
      return Rect{0, bar_area_length + kScrollBarExpandWidth, width,
                  kScrollBarExpandWidth};
    case ScrollBarAreaKind::UpSlot:
      return Rect{0, kScrollBarExpandWidth, width,
                  bar_area_length * scroll_state.start};
    case ScrollBarAreaKind::DownSlot:
      return Rect{
          0, kScrollBarExpandWidth + bar_area_length * scroll_state.GetEnd(),
          width, bar_area_length * (1 - scroll_state.GetEnd())};
    case ScrollBarAreaKind::Thumb:
      return Rect{0,
                  kScrollBarExpandWidth + bar_area_length * scroll_state.start,
                  width, bar_area_length * scroll_state.length};
    case ScrollBarAreaKind::Scroll:
      return Rect{0, kScrollBarExpandWidth, width, bar_area_length};
    default:
      std::unreachable();
  }
}

std::optional<Rect>
VerticalScrollBarRenderObject::GetCollapsedTriggerExpandAreaRect() {
  if (!IsEnabled()) return std::nullopt;

  return Rect{GetSize().width - kScrollBarCollapsedTriggerExpandAreaWidth, 0,
              kScrollBarCollapsedTriggerExpandAreaWidth, GetSize().height};
}

std::optional<Rect> VerticalScrollBarRenderObject::GetCollapsedThumbRect() {
  auto rect = GetExpandedAreaRect(ScrollBarAreaKind::Thumb);
  if (!rect) return std::nullopt;
  rect->left = GetSize().width - kScrollBarCollapseThumbWidth;
  rect->width = kScrollBarCollapseThumbWidth;
  return rect;
}

Size VerticalScrollBarRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  if (!IsEnabled()) {
    return Size{0, 0};
  }
  return Size{
      IsExpanded() ? kScrollBarExpandWidth : kScrollBarCollapseThumbWidth,
      // Reduce the right bottom corner.
      std::max(0.f,
               requirement.max.height.GetLengthOr0() - kScrollBarExpandWidth),
  };
}

void VerticalScrollBarRenderObject::DrawUpArrow(
    platform::graphics::IPainter* painter, const Rect& area,
    platform::graphics::IBrush* arrow_brush,
    platform::graphics::IBrush* background_brush) {
  painter->FillRectangle(area, background_brush);
  DrawScrollBarArrow(painter, arrow_geometry_.get(), area, arrow_brush, 90);
}

void VerticalScrollBarRenderObject::DrawDownArrow(
    platform::graphics::IPainter* painter, const Rect& area,
    platform::graphics::IBrush* arrow_brush,
    platform::graphics::IBrush* background_brush) {
  painter->FillRectangle(area, background_brush);
  DrawScrollBarArrow(painter, arrow_geometry_.get(), area, arrow_brush, 270);
}
}  // namespace cru::ui::render
