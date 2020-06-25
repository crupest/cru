#pragma once
#include "LayoutRenderObject.hpp"

namespace cru::ui::render {
class FlexLayoutRenderObject : public LayoutRenderObject<FlexChildLayoutData> {
 public:
  FlexLayoutRenderObject() = default;
  FlexLayoutRenderObject(const FlexLayoutRenderObject& other) = delete;
  FlexLayoutRenderObject& operator=(const FlexLayoutRenderObject& other) =
      delete;
  FlexLayoutRenderObject(FlexLayoutRenderObject&& other) = delete;
  FlexLayoutRenderObject& operator=(FlexLayoutRenderObject&& other) = delete;
  ~FlexLayoutRenderObject() override = default;

  FlexDirection GetFlexDirection() const { return direction_; }
  void SetFlexDirection(FlexDirection direction) {
    direction_ = direction;
    InvalidateLayout();
  }

  FlexMainAlignment GetContentMainAlign() const { return content_main_align_; }
  void SetContentMainAlign(FlexMainAlignment align) {
    content_main_align_ = align;
    InvalidateLayout();
  }

  FlexCrossAlignment GetItemCrossAlign() const { return item_cross_align_; }
  void SetItemCrossAlign(FlexCrossAlignment align) {
    item_cross_align_ = align;
    InvalidateLayout();
  }

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement, const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  FlexDirection direction_ = FlexDirection::Horizontal;
  FlexMainAlignment content_main_align_ = FlexMainAlignment::Start;
  FlexCrossAlignment item_cross_align_ = FlexCrossAlignment::Center;
};
}  // namespace cru::ui::render
