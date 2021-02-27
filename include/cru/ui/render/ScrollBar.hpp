#pragma once
#include "Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/platform/graphics/Base.hpp"
#include "cru/platform/graphics/Painter.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/controls/Control.hpp"

#include <gsl/pointers>
#include <memory>
#include <optional>

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

class ScrollBar : public Object {
 public:
  ScrollBar(gsl::not_null<ScrollRenderObject*> render_object,
            Direction direction);

  CRU_DELETE_COPY(ScrollBar)
  CRU_DELETE_MOVE(ScrollBar)

  ~ScrollBar() override = default;

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

  gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
  GetCollapsedThumbBrush() const;
  gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
  GetExpandedThumbBrush() const;
  gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
  GetExpandedSlotBrush() const;
  gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
  GetExpandedArrowBrush() const;
  gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
  GetExpandedArrowBackgroundBrush() const;

 protected:
  void OnDraw(platform::graphics::IPainter* painter, bool expand);

  virtual void DrawUpArrow(platform::graphics::IPainter* painter,
                           const Rect& area) = 0;
  virtual void DrawDownArrow(platform::graphics::IPainter* painter,
                             const Rect& area) = 0;

  std::optional<ScrollBarAreaKind> ExpandedHitTest(const Point& point);

  virtual bool IsShowBar() = 0;

  virtual std::optional<Rect> GetExpandedAreaRect(
      ScrollBarAreaKind area_kind) = 0;
  virtual std::optional<Rect> GetCollapsedTriggerExpandAreaRect() = 0;
  virtual std::optional<Rect> GetCollapsedThumbRect() = 0;

  virtual float CalculateNewScrollPosition(const Rect& thumb_original_rect,
                                           const Point& mouse_offset) = 0;

 protected:
  gsl::not_null<ScrollRenderObject*> render_object_;

 private:
  Direction direction_;

  bool is_enabled_ = true;

  bool is_expanded_ = false;

  std::shared_ptr<platform::graphics::IBrush> collapsed_thumb_brush_;
  std::shared_ptr<platform::graphics::IBrush> expanded_thumb_brush_;
  std::shared_ptr<platform::graphics::IBrush> expanded_slot_brush_;
  std::shared_ptr<platform::graphics::IBrush> expanded_arrow_brush_;
  std::shared_ptr<platform::graphics::IBrush> expanded_arrow_background_brush_;

  Rect move_thumb_thumb_original_rect_;
  std::optional<Point> move_thumb_start_;

  EventRevokerListGuard event_guard_;

  Event<Scroll> scroll_attempt_event_;
};

class HorizontalScrollBar : public ScrollBar {
 public:
  explicit HorizontalScrollBar(
      gsl::not_null<ScrollRenderObject*> render_object);

  CRU_DELETE_COPY(HorizontalScrollBar)
  CRU_DELETE_MOVE(HorizontalScrollBar)

  ~HorizontalScrollBar() override = default;

 protected:
  void DrawUpArrow(platform::graphics::IPainter* painter,
                   const Rect& area) override;
  void DrawDownArrow(platform::graphics::IPainter* painter,
                     const Rect& area) override;

  bool IsShowBar() override;

  std::optional<Rect> GetExpandedAreaRect(ScrollBarAreaKind area_kind) override;
  std::optional<Rect> GetCollapsedTriggerExpandAreaRect() override;
  std::optional<Rect> GetCollapsedThumbRect() override;

  float CalculateNewScrollPosition(const Rect& thumb_original_rect,
                                   const Point& mouse_offset) override;
};

class VerticalScrollBar : public ScrollBar {
 public:
  explicit VerticalScrollBar(gsl::not_null<ScrollRenderObject*> render_object);

  CRU_DELETE_COPY(VerticalScrollBar)
  CRU_DELETE_MOVE(VerticalScrollBar)

  ~VerticalScrollBar() override = default;

 protected:
  void DrawUpArrow(platform::graphics::IPainter* painter,
                   const Rect& area) override;
  void DrawDownArrow(platform::graphics::IPainter* painter,
                     const Rect& area) override;

  bool IsShowBar() override;

  std::optional<Rect> GetExpandedAreaRect(ScrollBarAreaKind area_kind) override;
  std::optional<Rect> GetCollapsedTriggerExpandAreaRect() override;
  std::optional<Rect> GetCollapsedThumbRect() override;

  float CalculateNewScrollPosition(const Rect& thumb_original_rect,
                                   const Point& mouse_offset) override;
};

// A delegate to draw scrollbar and register related events.
class ScrollBarDelegate : public Object {
 public:
  explicit ScrollBarDelegate(gsl::not_null<ScrollRenderObject*> render_object);

  CRU_DELETE_COPY(ScrollBarDelegate)
  CRU_DELETE_MOVE(ScrollBarDelegate)

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
  gsl::not_null<ScrollRenderObject*> render_object_;

  HorizontalScrollBar horizontal_bar_;
  VerticalScrollBar vertical_bar_;

  Event<Scroll> scroll_attempt_event_;
};
}  // namespace cru::ui::render
