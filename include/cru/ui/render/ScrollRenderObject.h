#pragma once
#include "../Base.h"
#include "RenderObject.h"

#include <cru/platform/graphics/Base.h>
#include <cru/platform/graphics/Brush.h>
#include <cru/platform/graphics/Geometry.h>
#include <cru/platform/graphics/Painter.h>
#include <cru/platform/gui/UiApplication.h>

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace cru::ui::render {
enum class ScrollKind { Absolute, Relative, Page, Line };

struct Scroll {
  Direction direction;
  ScrollKind kind;
  // For absolute, the new scroll position. Otherwise, offset.
  float value;
};

struct ScrollPercentage {
  float start;
  float length;

  float GetEnd() const { return start + length; }

  bool CanScrollBackward() const { return start > 0.f; }
  bool CanScrollForward() const { return GetEnd() < 1.f; }
  bool CanScroll() const { return CanScrollBackward() || CanScrollForward(); }

  static ScrollPercentage FromAbsolute(float offset, float view_length,
                                       float child_length) {
    if (child_length <= 0.f) return {0.f, 1.f};
    auto start = std::clamp(offset / child_length, 0.f, 1.f);
    auto length = std::clamp(view_length / child_length, 0.f, 1.f - start);
    return {start, length};
  }

  bool operator==(const ScrollPercentage& other) const = default;
};

struct ScrollPercentageState {
  ScrollPercentage horizontal;
  ScrollPercentage vertical;
};

struct ScrollState {
  Point offset;
  Size view_size;
  /**
   * @brief The size of the child render object, which might be smaller than
   * view_size or be zero.
   */
  Size child_size;

  bool CanScrollUp() const { return offset.y > 0.f; }
  bool CanScrollDown() const {
    return offset.y < child_size.height - view_size.height;
  }
  bool CanScrollLeft() const { return offset.x > 0.f; }
  bool CanScrollRight() const {
    return offset.x < child_size.width - view_size.width;
  }
  bool CanScrollVertical() const { return CanScrollUp() || CanScrollDown(); }
  bool CanScrollHorizontal() const {
    return CanScrollLeft() || CanScrollRight();
  }
  bool CanScroll() const {
    return CanScrollVertical() || CanScrollHorizontal();
  }

  ScrollPercentage GetHorizontalPercentage() const {
    return ScrollPercentage::FromAbsolute(offset.x, view_size.width,
                                          child_size.width);
  }

  ScrollPercentage GetVerticalPercentage() const {
    return ScrollPercentage::FromAbsolute(offset.y, view_size.height,
                                          child_size.height);
  }

  ScrollPercentageState GetPercentage() const {
    ScrollPercentageState result{};
    result.horizontal = GetHorizontalPercentage();
    result.vertical = GetVerticalPercentage();
    return result;
  };
};

// Measure logic:
// Measure child with unspecified min and max size.
// If parent's preferred size is specified, then it is used as measure result.
// Or child's size is coerced into requirement and then used as result.
// If no child, then use the preferred size if set or min size if set or 0.
// Layout logic:
// If child is smaller than content area, layout at lefttop.
// Or layout by scroll state.
class CRU_UI_API ScrollRenderObject
    : public RenderObject,
      public RenderObject::SingleChildMixin<ScrollRenderObject> {
 public:
  static constexpr auto kRenderObjectName = "ScrollRenderObject";

  ScrollRenderObject();

  RenderObject* HitTest(const Point& point) override;

  // Return the coerced scroll offset.
  Point GetScrollOffset();
  float GetScrollOffset(Direction direction) {
    return direction == Direction::Horizontal ? GetScrollOffset().x
                                              : GetScrollOffset().y;
  }
  void SetScrollOffset(const Point& offset);
  void SetScrollOffset(std::optional<float> x, std::optional<float> y);
  void SetScrollOffset(Direction direction, std::optional<float> value) {
    if (direction == Direction::Horizontal) {
      SetScrollOffset(value, std::nullopt);
    } else {
      SetScrollOffset(std::nullopt, value);
    }
  }

  ScrollState GetScrollState();

  IEvent<ScrollState>* ScrollEvent() { return &scroll_event_; }

  void ApplyScroll(const Scroll& scroll);

  Point GetRawScrollOffset() { return scroll_offset_; }

  // Return the viewable area rect.
  // Lefttop is scroll offset. Size is content size.
  // If size exceeds view area, left and top is more important when calculate
  // new scroll offset.
  Rect GetViewRect() {
    return Rect{GetScrollOffset(), GetContentRect().GetSize()};
  }

  // Rect lefttop relative to content rect.
  // Param margin is just for convenience and it will just add to the rect.
  void ScrollToContain(const Rect& rect, const Thickness& margin = Thickness{});

 protected:
  // Logic:
  // If available size is bigger than child's preferred size, then child's
  // preferred size is taken.
  // If not, all available size is taken while forming a scroll area.
  Size OnMeasureContent(const MeasureRequirement& requirement) override;
  void OnLayoutContent(const Rect& content_rect) override;
  void OnDraw(RenderObjectDrawContext& context) override;

 private:
  void RaiseScrollEvent();

  Point scroll_offset_;
  Event<ScrollState> scroll_event_;
};

enum class ScrollBarAreaKind {
  UpArrow,    // Line up
  DownArrow,  // Line down
  UpSlot,     // Page up
  DownSlot,   // Page down
  Thumb,
  /**
   * up slot + down slot + thumb
   */
  Scroll,
};

constexpr std::array kScrollBarAreaKindList{
    ScrollBarAreaKind::UpArrow, ScrollBarAreaKind::DownArrow,
    ScrollBarAreaKind::UpSlot, ScrollBarAreaKind::DownSlot,
    ScrollBarAreaKind::Thumb};

enum class ScrollBarBrushUsageKind { Arrow, ArrowBackground, Slot, Thumb };
enum class ScrollBarBrushStateKind { Normal, Hover, Press, Disable };

std::string CRU_UI_API GenerateScrollBarThemeColorKey(
    ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state);

class CRU_UI_API ScrollBarRenderObject : public RenderObject {
 public:
  ScrollBarRenderObject(std::string name, Direction direction);

 public:
  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnDraw(RenderObjectDrawContext& context) override;

 public:
  Direction GetDirection() { return direction_; }

  bool IsEnabled() { return is_enabled_; }
  void SetEnabled(bool value);

  bool IsExpanded() { return is_expanded_; }
  void SetExpanded(bool value);

  const ScrollPercentage& GetScrollState() { return scroll_state_; }
  void SetScrollState(const ScrollPercentage& scroll_state);

  ScrollBarBrushStateKind GetBrushState(ScrollBarAreaKind area);
  /**
   * @brief Get the brush state used for painting.
   *
   * This starts from GetBrushState(area), then applies the current scroll
   * state: UpArrow is Disable when the bar cannot scroll backward, and
   * DownArrow is Disable when the bar cannot scroll forward. Slot and Thumb
   * keep their stored hover/press/normal state. Disable takes precedence over
   * hover and press for the arrow at a scroll boundary.
   */
  ScrollBarBrushStateKind GetCalculatedBrushState(ScrollBarAreaKind area);
  void SetBrushState(ScrollBarAreaKind area, ScrollBarBrushStateKind state);

  std::shared_ptr<platform::graphics::IBrush> GetCollapsedThumbBrush();
  // Brush could be nullptr to use the theme brush.
  void SetCollapsedThumbBrush(
      std::shared_ptr<platform::graphics::IBrush> brush);
  std::shared_ptr<platform::graphics::IBrush> GetBrush(
      ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state);
  // Brush could be nullptr to use the theme brush.
  void SetBrush(ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state,
                std::shared_ptr<platform::graphics::IBrush> brush);

  virtual std::optional<Rect> GetExpandedAreaRect(
      ScrollBarAreaKind area_kind) = 0;
  virtual std::optional<Rect> GetCollapsedTriggerExpandAreaRect() = 0;
  virtual std::optional<Rect> GetCollapsedThumbRect() = 0;
  std::optional<ScrollBarAreaKind> ExpandedHitTest(const Point& point);

 protected:
  void DoDraw(platform::graphics::IPainter* painter, bool expand);

  virtual void DrawUpArrow(platform::graphics::IPainter* painter,
                           const Rect& area,
                           platform::graphics::IBrush* arrow_brush,
                           platform::graphics::IBrush* background_brush) = 0;
  virtual void DrawDownArrow(platform::graphics::IPainter* painter,
                             const Rect& area,
                             platform::graphics::IBrush* arrow_brush,
                             platform::graphics::IBrush* background_brush) = 0;

 protected:
  std::unique_ptr<platform::graphics::IGeometry> arrow_geometry_;

 private:
  Direction direction_;

  bool is_enabled_ = true;

  bool is_expanded_ = false;

  ScrollPercentage scroll_state_ = {};

  ScrollBarBrushStateKind up_arrow_state_ = ScrollBarBrushStateKind::Normal;
  ScrollBarBrushStateKind down_arrow_state_ = ScrollBarBrushStateKind::Normal;
  ScrollBarBrushStateKind up_slot_state_ = ScrollBarBrushStateKind::Normal;
  ScrollBarBrushStateKind down_slot_state_ = ScrollBarBrushStateKind::Normal;
  ScrollBarBrushStateKind thumb_state_ = ScrollBarBrushStateKind::Normal;

  std::shared_ptr<platform::graphics::IBrush> collapsed_thumb_brush_;
  std::unordered_map<
      ScrollBarBrushUsageKind,
      std::unordered_map<ScrollBarBrushStateKind,
                         std::shared_ptr<platform::graphics::IBrush>>>
      brushes_;
};

class CRU_UI_API HorizontalScrollBarRenderObject
    : public ScrollBarRenderObject {
 public:
  constexpr static auto kRenderObjectName = "HorizontalScrollBarRenderObject";

  HorizontalScrollBarRenderObject();

  std::optional<Rect> GetExpandedAreaRect(ScrollBarAreaKind area_kind) override;
  std::optional<Rect> GetCollapsedTriggerExpandAreaRect() override;
  std::optional<Rect> GetCollapsedThumbRect() override;

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement) override;

  void DrawUpArrow(platform::graphics::IPainter* painter, const Rect& area,
                   platform::graphics::IBrush* arrow_brush,
                   platform::graphics::IBrush* background_brush) override;
  void DrawDownArrow(platform::graphics::IPainter* painter, const Rect& area,
                     platform::graphics::IBrush* arrow_brush,
                     platform::graphics::IBrush* background_brush) override;
};

class CRU_UI_API VerticalScrollBarRenderObject : public ScrollBarRenderObject {
 public:
  constexpr static auto kRenderObjectName = "VerticalScrollBarRenderObject";

  explicit VerticalScrollBarRenderObject();

  std::optional<Rect> GetExpandedAreaRect(ScrollBarAreaKind area_kind) override;
  std::optional<Rect> GetCollapsedTriggerExpandAreaRect() override;
  std::optional<Rect> GetCollapsedThumbRect() override;

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement) override;

  void DrawUpArrow(platform::graphics::IPainter* painter, const Rect& area,
                   platform::graphics::IBrush* arrow_brush,
                   platform::graphics::IBrush* background_brush) override;
  void DrawDownArrow(platform::graphics::IPainter* painter, const Rect& area,
                     platform::graphics::IBrush* arrow_brush,
                     platform::graphics::IBrush* background_brush) override;
};
}  // namespace cru::ui::render
