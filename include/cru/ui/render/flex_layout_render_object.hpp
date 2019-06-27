#pragma once
#include "render_object.hpp"

#include <optional>

namespace cru::ui::render {
enum class FlexDirection {
  Horizontal,
  HorizontalReverse,
  Vertical,
  VertivalReverse
};

namespace internal {
constexpr int align_start = 0;
constexpr int align_end = align_start + 1;
constexpr int align_center = align_end + 1;
//constexpr int align_stretch = align_center + 1;
}  // namespace internal

enum class FlexMainAlignment {
  Start = internal::align_start,
  End = internal::align_end,
  Center = internal::align_center
};
enum class FlexCrossAlignment {
  Start = internal::align_start,
  End = internal::align_end,
  Center = internal::align_center,
//  Stretch = internal::align_stretch
};

struct FlexChildLayoutData {
  // nullopt stands for looking at my content
  std::optional<float> flex_basis = std::nullopt;
  float flex_grow = 0;
  float flex_shrink = 0;
  // nullopt stands for looking at parent's setting
  std::optional<FlexCrossAlignment> cross_alignment = std::nullopt;
};

class FlexLayoutRenderObject : public RenderObject {
 public:
  FlexLayoutRenderObject() = default;
  FlexLayoutRenderObject(const FlexLayoutRenderObject& other) = delete;
  FlexLayoutRenderObject& operator=(const FlexLayoutRenderObject& other) =
      delete;
  FlexLayoutRenderObject(FlexLayoutRenderObject&& other) = delete;
  FlexLayoutRenderObject& operator=(FlexLayoutRenderObject&& other) = delete;
  ~FlexLayoutRenderObject() override = default;

  FlexDirection GetFlexDirection() const { return direction_; }
  void SetFlexDirection(FlexDirection direction) { direction_ = direction; }

  FlexMainAlignment GetContentMainAlign() const { return content_main_align_; }
  void SetContentMainAlign(FlexMainAlignment align) {
    content_main_align_ = align;
  }

  FlexCrossAlignment GetItemCrossAlign() const { return item_cross_align_; }
  void SetItemCrossAlign(FlexCrossAlignment align) {
    item_cross_align_ = align;
  }

  FlexChildLayoutData* GetChildLayoutData(int position);

  void Draw(platform::graph::Painter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnAddChild(RenderObject* new_child, int position) override;
  void OnRemoveChild(RenderObject* removed_child, int position) override;

  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  FlexDirection direction_ = FlexDirection::Horizontal;
  FlexMainAlignment content_main_align_ = FlexMainAlignment::Start;
  FlexCrossAlignment item_cross_align_ = FlexCrossAlignment::Center;
  std::vector<FlexChildLayoutData> child_layout_data_{};
};
}  // namespace cru::ui::render
