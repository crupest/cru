#pragma once
#include "cru/base/Base.h"
#include "cru/base/Event.h"
#include "cru/platform/graphics/Base.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/Base.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/helper/ClickDetector.h"

#include <memory>
#include <optional>
#include <unordered_map>

namespace cru::ui::render {
class ScrollRenderObject;

enum class ScrollKind { Absolute, Relative, Page, Line };

struct Scroll {
  Direction direction;
  ScrollKind kind;
  // For absolute, the new scroll position. Otherwise, offset.
  float value;
};

enum class ScrollBarAreaKind {
  UpArrow,    // Line up
  DownArrow,  // Line down
  UpSlot,     // Page up
  DownSlot,   // Page down
  Thumb
};

enum class ScrollBarBrushUsageKind { Arrow, ArrowBackground, Slot, Thumb };
enum class ScrollBarBrushStateKind { Normal, Hover, Press, Disable };

std::string CRU_UI_API GenerateScrollBarThemeColorKey(ScrollBarBrushUsageKind usage,
                                                 ScrollBarBrushStateKind state);

class CRU_UI_API ScrollBar : public Object {
 public:
  ScrollBar(ScrollRenderObject* render_object, Direction direction);
  ~ScrollBar() override;

 public:
  Direction GetDirection() const { return direction_; }

  bool IsEnabled() const { return is_enabled_; }
  void SetEnabled(bool value);

  bool IsExpanded() const { return is_expanded_; }
  void SetExpanded(bool value);

  void Draw(platform::graphics::IPainter* painter);

  IEvent<Scroll>* ScrollAttemptEvent() { return &scroll_attempt_event_; }

  void InstallHandlers(controls::Control* control);
  void UninstallHandlers() { InstallHandlers(nullptr); }

  std::shared_ptr<platform::graphics::IBrush> GetCollapsedThumbBrush();
  // Brush could be nullptr to use the theme brush.
  void SetCollapsedThumbBrush(
      std::shared_ptr<platform::graphics::IBrush> brush);
  std::shared_ptr<platform::graphics::IBrush> GetBrush(
      ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state);
  // Brush could be nullptr to use the theme brush.
  void SetBrush(ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state,
                std::shared_ptr<platform::graphics::IBrush> brush);

 protected:
  void OnDraw(platform::graphics::IPainter* painter, bool expand);

  virtual void DrawUpArrow(platform::graphics::IPainter* painter,
                           const Rect& area,
                           platform::graphics::IBrush* arrow_brush,
                           platform::graphics::IBrush* background_brush) = 0;
  virtual void DrawDownArrow(platform::graphics::IPainter* painter,
                             const Rect& area,
                             platform::graphics::IBrush* arrow_brush,
                             platform::graphics::IBrush* background_brush) = 0;

  std::optional<ScrollBarAreaKind> ExpandedHitTest(const Point& point);

  virtual bool IsShowBar() = 0;

  virtual std::optional<Rect> GetExpandedAreaRect(
      ScrollBarAreaKind area_kind) = 0;
  virtual std::optional<Rect> GetCollapsedTriggerExpandAreaRect() = 0;
  virtual std::optional<Rect> GetCollapsedThumbRect() = 0;

  virtual float CalculateNewScrollPosition(const Rect& thumb_original_rect,
                                           const Point& mouse_offset) = 0;

  virtual bool CanScrollUp() = 0;
  virtual bool CanScrollDown() = 0;

 private:
  void SetCursor();
  void RestoreCursor();

  void BeginAutoCollapseTimer();
  void StopAutoCollapseTimer();

  void OnMouseLeave();

  ScrollBarBrushStateKind GetState(ScrollBarAreaKind area);

 protected:
  ScrollRenderObject* render_object_;

  std::unique_ptr<platform::graphics::IGeometry> arrow_geometry_;

 private:
  Direction direction_;

  bool is_enabled_ = true;

  bool is_expanded_ = false;

  std::shared_ptr<platform::graphics::IBrush> collapsed_thumb_brush_;
  std::unordered_map<
      ScrollBarBrushUsageKind,
      std::unordered_map<ScrollBarBrushStateKind,
                         std::shared_ptr<platform::graphics::IBrush>>>
      brushes_;

  Rect move_thumb_thumb_original_rect_;
  std::optional<Point> move_thumb_start_;

  std::optional<ScrollBarAreaKind> mouse_hover_;
  std::optional<ScrollBarAreaKind> mouse_press_;

  EventHandlerRevokerListGuard event_guard_;

  Event<Scroll> scroll_attempt_event_;

  bool cursor_overrided_ = false;

  platform::gui::TimerAutoCanceler auto_collapse_timer_canceler_;
};

class CRU_UI_API HorizontalScrollBar : public ScrollBar {
 public:
  explicit HorizontalScrollBar(ScrollRenderObject* render_object);

  CRU_DELETE_COPY(HorizontalScrollBar)
  CRU_DELETE_MOVE(HorizontalScrollBar)

  ~HorizontalScrollBar() override = default;

 protected:
  void DrawUpArrow(platform::graphics::IPainter* painter, const Rect& area,
                   platform::graphics::IBrush* arrow_brush,
                   platform::graphics::IBrush* background_brush) override;
  void DrawDownArrow(platform::graphics::IPainter* painter, const Rect& area,
                     platform::graphics::IBrush* arrow_brush,
                     platform::graphics::IBrush* background_brush) override;

  bool IsShowBar() override;

  std::optional<Rect> GetExpandedAreaRect(ScrollBarAreaKind area_kind) override;
  std::optional<Rect> GetCollapsedTriggerExpandAreaRect() override;
  std::optional<Rect> GetCollapsedThumbRect() override;

  float CalculateNewScrollPosition(const Rect& thumb_original_rect,
                                   const Point& mouse_offset) override;

  bool CanScrollUp() override;
  bool CanScrollDown() override;
};

class CRU_UI_API VerticalScrollBar : public ScrollBar {
 public:
  explicit VerticalScrollBar(ScrollRenderObject* render_object);

  CRU_DELETE_COPY(VerticalScrollBar)
  CRU_DELETE_MOVE(VerticalScrollBar)

  ~VerticalScrollBar() override = default;

 protected:
  void DrawUpArrow(platform::graphics::IPainter* painter, const Rect& area,
                   platform::graphics::IBrush* arrow_brush,
                   platform::graphics::IBrush* background_brush) override;
  void DrawDownArrow(platform::graphics::IPainter* painter, const Rect& area,
                     platform::graphics::IBrush* arrow_brush,
                     platform::graphics::IBrush* background_brush) override;

  bool IsShowBar() override;

  std::optional<Rect> GetExpandedAreaRect(ScrollBarAreaKind area_kind) override;
  std::optional<Rect> GetCollapsedTriggerExpandAreaRect() override;
  std::optional<Rect> GetCollapsedThumbRect() override;

  float CalculateNewScrollPosition(const Rect& thumb_original_rect,
                                   const Point& mouse_offset) override;

  bool CanScrollUp() override;
  bool CanScrollDown() override;
};

// A delegate to draw scrollbar and register related events.
class CRU_UI_API ScrollBarDelegate : public Object {
 public:
  explicit ScrollBarDelegate(ScrollRenderObject* render_object);
  ~ScrollBarDelegate() override = default;

 public:
  bool IsHorizontalBarEnabled() const { return horizontal_bar_.IsEnabled(); }
  void SetHorizontalBarEnabled(bool value) {
    horizontal_bar_.SetEnabled(value);
  }

  bool IsVerticalBarEnabled() const { return horizontal_bar_.IsEnabled(); }
  void SetVerticalBarEnabled(bool value) { horizontal_bar_.SetEnabled(value); }

  IEvent<Scroll>* ScrollAttemptEvent() { return &scroll_attempt_event_; }

  void DrawScrollBar(platform::graphics::IPainter* painter);

  void InstallHandlers(controls::Control* control);
  void UninstallHandlers() { InstallHandlers(nullptr); }

 private:
  ScrollRenderObject* render_object_;

  HorizontalScrollBar horizontal_bar_;
  VerticalScrollBar vertical_bar_;

  Event<Scroll> scroll_attempt_event_;
};
}  // namespace cru::ui::render
