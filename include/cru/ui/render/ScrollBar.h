#pragma once
#include <cru/Base.h>
#include <cru/Event.h>
#include "cru/graphics/Base.h"
#include "cru/graphics/Brush.h"
#include "cru/graphics/Geometry.h"
#include "cru/graphics/Painter.h"
#include "cru/gui/Cursor.h"
#include "cru/gui/TimerHelper.h"
#include "cru/gui/UiApplication.h"
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

String CRU_UI_API GenerateScrollBarThemeColorKey(ScrollBarBrushUsageKind usage,
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

  void Draw(graphics::IPainter* painter);

  IEvent<Scroll>* ScrollAttemptEvent() { return &scroll_attempt_event_; }

  void InstallHandlers(controls::Control* control);
  void UninstallHandlers() { InstallHandlers(nullptr); }

  std::shared_ptr<graphics::IBrush> GetCollapsedThumbBrush();
  // Brush could be nullptr to use the theme brush.
  void SetCollapsedThumbBrush(
      std::shared_ptr<graphics::IBrush> brush);
  std::shared_ptr<graphics::IBrush> GetBrush(
      ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state);
  // Brush could be nullptr to use the theme brush.
  void SetBrush(ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state,
                std::shared_ptr<graphics::IBrush> brush);

 protected:
  void OnDraw(graphics::IPainter* painter, bool expand);

  virtual void DrawUpArrow(graphics::IPainter* painter,
                           const Rect& area,
                           graphics::IBrush* arrow_brush,
                           graphics::IBrush* background_brush) = 0;
  virtual void DrawDownArrow(graphics::IPainter* painter,
                             const Rect& area,
                             graphics::IBrush* arrow_brush,
                             graphics::IBrush* background_brush) = 0;

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

  std::unique_ptr<graphics::IGeometry> arrow_geometry_;

 private:
  Direction direction_;

  bool is_enabled_ = true;

  bool is_expanded_ = false;

  std::shared_ptr<graphics::IBrush> collapsed_thumb_brush_;
  std::unordered_map<
      ScrollBarBrushUsageKind,
      std::unordered_map<ScrollBarBrushStateKind,
                         std::shared_ptr<graphics::IBrush>>>
      brushes_;

  Rect move_thumb_thumb_original_rect_;
  std::optional<Point> move_thumb_start_;

  std::optional<ScrollBarAreaKind> mouse_hover_;
  std::optional<ScrollBarAreaKind> mouse_press_;

  EventRevokerListGuard event_guard_;

  Event<Scroll> scroll_attempt_event_;

  bool cursor_overrided_ = false;

  platform::gui::TimerAutoCanceler auto_collapse_timer_canceler_;
};

class CRU_UI_API HorizontalScrollBar : public ScrollBar {
 public:
  explicit HorizontalScrollBar(ScrollRenderObject* render_object);


  ~HorizontalScrollBar() override = default;

 protected:
  void DrawUpArrow(graphics::IPainter* painter, const Rect& area,
                   graphics::IBrush* arrow_brush,
                   graphics::IBrush* background_brush) override;
  void DrawDownArrow(graphics::IPainter* painter, const Rect& area,
                     graphics::IBrush* arrow_brush,
                     graphics::IBrush* background_brush) override;

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


  ~VerticalScrollBar() override = default;

 protected:
  void DrawUpArrow(graphics::IPainter* painter, const Rect& area,
                   graphics::IBrush* arrow_brush,
                   graphics::IBrush* background_brush) override;
  void DrawDownArrow(graphics::IPainter* painter, const Rect& area,
                     graphics::IBrush* arrow_brush,
                     graphics::IBrush* background_brush) override;

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

  void DrawScrollBar(graphics::IPainter* painter);

  void InstallHandlers(controls::Control* control);
  void UninstallHandlers() { InstallHandlers(nullptr); }

 private:
  ScrollRenderObject* render_object_;

  HorizontalScrollBar horizontal_bar_;
  VerticalScrollBar vertical_bar_;

  Event<Scroll> scroll_attempt_event_;
};
}  // namespace cru::ui::render
