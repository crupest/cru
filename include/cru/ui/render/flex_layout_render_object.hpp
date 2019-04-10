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

enum class Alignment { Start, End, Center };

struct FlexChildLayoutData {
  std::optional<float> flex_basis;  // nullopt stands for content
  float flex_grow = 0;
  float flex_shrink = 0;
  Alignment alignment = Alignment::Center;
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

  Alignment GetContentMainAlign() const { return content_main_align_; }
  void SetContentMainAlign(Alignment align) { content_main_align_ = align; }

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
  Alignment content_main_align_ = Alignment::Start;
  std::vector<FlexChildLayoutData> child_layout_data_{};
};
}  // namespace cru::ui::render
