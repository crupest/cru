#pragma once
#include "Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/platform/graphics/Base.hpp"
#include "cru/platform/graphics/Painter.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/controls/Control.hpp"

#include <gsl/pointers>
#include <memory>
#include <optional>

namespace cru::ui::render {
class ScrollRenderObject;

enum class ScrollBarAreaKind {
  UpArrow,    // Line up
  DownArrow,  // Line down
  UpThumb,    // Page up
  DownThumb,  // Page down
  Thumb
};

class ScrollBar : public Object {
 public:
  explicit ScrollBar(gsl::not_null<ScrollRenderObject*> render_object);

  CRU_DELETE_COPY(ScrollBar)
  CRU_DELETE_MOVE(ScrollBar)

  ~ScrollBar() override = default;

 public:
  bool IsEnabled() const { return is_enabled_; }
  void SetEnabled(bool value);

  void Draw(platform::graphics::IPainter* painter);

  virtual std::optional<ScrollBarAreaKind> HitTest(const Point& point) = 0;

  IEvent<float>* ScrollAttemptEvent() { return &scroll_attempt_event_; }

  void InstallHandlers(controls::Control* control);
  void UninstallHandlers() { InstallHandlers(nullptr); }

  gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
  GetCollapseThumbBrush() const;

 protected:
  virtual void OnDraw(platform::graphics::IPainter* painter, bool expand) = 0;

 protected:
  gsl::not_null<ScrollRenderObject*> render_object_;

 private:
  bool is_enabled_ = true;

  bool is_expanded_ = false;

  std::shared_ptr<platform::graphics::IBrush> collapse_thumb_brush_;

  EventRevokerListGuard event_guard_;

  Event<float> scroll_attempt_event_;
};

class HorizontalScrollBar : public ScrollBar {
 public:
  explicit HorizontalScrollBar(
      gsl::not_null<ScrollRenderObject*> render_object);

  CRU_DELETE_COPY(HorizontalScrollBar)
  CRU_DELETE_MOVE(HorizontalScrollBar)

  ~HorizontalScrollBar() override = default;

 public:
  std::optional<ScrollBarAreaKind> HitTest(const Point& point) override;

 protected:
  void OnDraw(platform::graphics::IPainter* painter, bool expand) override;
};

class VerticalScrollBar : public ScrollBar {
 public:
  explicit VerticalScrollBar(gsl::not_null<ScrollRenderObject*> render_object);

  CRU_DELETE_COPY(VerticalScrollBar)
  CRU_DELETE_MOVE(VerticalScrollBar)

  ~VerticalScrollBar() override = default;

 public:
  std::optional<ScrollBarAreaKind> HitTest(const Point& point) override;

 protected:
  void OnDraw(platform::graphics::IPainter* painter, bool expand) override;
};

struct ScrollBarScrollAttemptArgs {
  float x_offset;
  float y_offset;
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

  IEvent<ScrollBarScrollAttemptArgs>* ScrollAttemptEvent() {
    return &scroll_attempt_event_;
  }

  void DrawScrollBar(platform::graphics::IPainter* painter);

  void InstallHandlers(controls::Control* control);
  void UninstallHandlers() { InstallHandlers(nullptr); }

 private:
  gsl::not_null<ScrollRenderObject*> render_object_;

  HorizontalScrollBar horizontal_bar_;
  VerticalScrollBar vertical_bar_;

  Event<ScrollBarScrollAttemptArgs> scroll_attempt_event_;
};
}  // namespace cru::ui::render
