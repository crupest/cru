#pragma once
#include "RenderObject.hpp"

#include "cru/platform/graphics/util/Painter.hpp"

#include <optional>

namespace cru::ui::render {
// Measure logic:
// Measure child with unspecified min and max size.
// If parent's preferred size is specified, then it is used as measure result.
// Or child's size is coerced into requirement and then used as result.
// If no child, then use the preferred size if set or min size if set or 0.
// Layout logic:
// If child is smaller than content area, layout at lefttop.
// Or layout by scroll state.
class ScrollRenderObject : public RenderObject {
 public:
  ScrollRenderObject() : RenderObject(ChildMode::Single) {}

  CRU_DELETE_COPY(ScrollRenderObject)
  CRU_DELETE_MOVE(ScrollRenderObject)

  ~ScrollRenderObject() override = default;

  RenderObject* HitTest(const Point& point) override;

  // Return the coerced scroll offset.
  Point GetScrollOffset();
  void SetScrollOffset(const Point& offset);
  void SetScrollOffset(std::optional<float> x, std::optional<float> y);
  Point GetRawScrollOffset() const { return scroll_offset_; }

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
  void OnDrawCore(platform::graphics::IPainter* painter) override;

  // Logic:
  // If available size is bigger than child's preferred size, then child's
  // preferred size is taken.
  // If not, all available size is taken while forming a scroll area.
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  Point scroll_offset_;
};
}  // namespace cru::ui::render
