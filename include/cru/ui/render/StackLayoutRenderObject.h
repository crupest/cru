#pragma once
#include "LayoutRenderObject.h"

namespace cru::ui::render {
struct StackChildLayoutData {
  std::optional<Alignment> horizontal;
  std::optional<Alignment> vertical;
};

// Measure Logic:
// Following rules are applied both horizontally and vertically.
//
// 1. Measure each children with min size not specified and max size as
// following rules:
//
//    1.1. If parent's preferred size is set and it is not less than child's min
//    size, then it is used as child's max size.
//
//    1.2. Or if parent's max size is set, then it is used as child's max size.
//    If it is less than child's min size, then log an warning about that.
//
// 2. Result size is children's max size.
//
// 3. If preferred size is specified and result size is smaller than it, coerce
// result size to preferred size.
//
// 4. If result size is smaller than min size (if specified), coerce result size
// to min size.
class CRU_UI_API StackLayoutRenderObject
    : public LayoutRenderObject<StackChildLayoutData> {
  CRU_DEFINE_CLASS_LOG_TAG("StackLayoutRenderObject")

 public:
  StackLayoutRenderObject() = default;
  CRU_DELETE_COPY(StackLayoutRenderObject)
  CRU_DELETE_MOVE(StackLayoutRenderObject)
  ~StackLayoutRenderObject() = default;

  std::string GetName() const override { return "StackLayoutRenderObject"; }

  Alignment GetDefaultHorizontalAlignment() const {
    return default_vertical_alignment_;
  }
  void SetDefaultHorizontalAlignment(Alignment alignment);
  Alignment GetDefaultVerticalAlignment() {
    return default_horizontal_alignment_;
  }
  void SetDefaultVertialAlignment(Alignment alignment);

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  Alignment default_horizontal_alignment_ = Alignment::Start;
  Alignment default_vertical_alignment_ = Alignment::Start;
};
}  // namespace cru::ui::render
