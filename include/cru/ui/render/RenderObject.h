#pragma once
#include "../Base.h"

#include "MeasureRequirement.h"

#include <cru/base/Event.h>
#include <cru/platform/graphics/Painter.h>
#include <string>

namespace cru::ui::render {
struct BoxConstraint {
  static const BoxConstraint kNotLimit;

  Size max;
  Size min;

  constexpr bool Validate() const {
    return max.width >= min.width && max.height >= min.height &&
           min.width >= 0 && min.height >= 0;
  }

  constexpr bool Satisfy(const Size& size) const {
    return size.width <= max.width && size.height <= max.height &&
           size.width >= min.width && size.height >= min.height;
  }

  constexpr Size Coerce(const Size& size) const {
    return Size{std::min(std::max(size.width, min.width), max.width),
                std::min(std::max(size.height, min.height), max.height)};
  }
};

/**
 *
 * ### Create New Render Object
 *
 * To write a custom RenderObject, override following methods:
 *
 * public:
 *  void Draw(platform::graphics::IPainter* painter) override;
 *  RenderObject* HitTest(const Point& point) override;
 * protected:
 *  Size OnMeasureContent(const MeasureRequirement& requirement) override;
 *  void OnLayoutContent(const Rect& content_rect) override;
 */
class CRU_UI_API RenderObject : public Object {
 private:
  constexpr static auto kLogTag = "cru::ui::render::RenderObject";

 public:
  RenderObject(std::string name);
  ~RenderObject() override;

  controls::Control* GetAttachedControl() { return control_; }
  void SetAttachedControl(controls::Control* new_control);

  RenderObject* GetParent() { return parent_; }
  void SetParent(RenderObject* new_parent);

  template <typename F>
  void WalkUp(const F& f, bool include_this = true) {
    auto parent = include_this ? this : GetParent();
    while (parent) {
      f(parent);
      parent = parent->GetParent();
    }
  }

  // Offset from parent's lefttop to lefttop of this render object. Margin is
  // accounted for.
  Point GetOffset() { return offset_; }
  Size GetSize() { return size_; }

  Point GetTotalOffset();
  Point FromRootToContent(const Point& point);

  Size GetMeasureResultSize() { return measure_result_size_; }

  Thickness GetMargin() { return margin_; }
  void SetMargin(const Thickness& margin);

  Thickness GetPadding() { return padding_; }
  void SetPadding(const Thickness& padding);

  MeasureSize GetSuggestSize() { return custom_measure_requirement_.suggest; }
  void SetSuggestSize(const MeasureSize& suggest_size);

  MeasureSize GetMinSize() { return custom_measure_requirement_.min; }
  void SetMinSize(const MeasureSize& min_size);

  MeasureSize GetMaxSize() { return custom_measure_requirement_.max; }
  void SetMaxSize(const MeasureSize& max_size);

  MeasureRequirement GetCustomMeasureRequirement() {
    return custom_measure_requirement_;
  }

  /**
   * First checks if layout cache is still valid. If not, invokes OnMeasureCore
   * to do the real measure and save the result. Use GetMeasureResultSize to get
   * the result size.
   *
   * The cache is valid only if,
   * 1. Measure requirement does not change.
   * 2. InvalidateLayout is not called on any descendents.
   */
  void Measure(const MeasureRequirement& requirement);

  /**
   * Sets offset and size (from GetMeasureResult) of this render object and call
   * OnLayoutCore. This will set layout as valid.
   */
  void Layout(const Point& offset);

  /**
   * Sets offset and size of this render object and call OnLayoutCore. This will
   * set layout as valid.
   */
  void Layout(const Rect& rect);

  virtual Thickness GetTotalSpaceThickness();
  virtual Thickness GetInnerSpaceThickness();

  virtual Rect GetPaddingRect();
  virtual Rect GetContentRect();

  virtual void Draw(platform::graphics::IPainter* painter) = 0;

  // Param point must be relative the lefttop of render object including margin.
  // Add offset before pass point to children.
  virtual RenderObject* HitTest(const Point& point) = 0;

 public:
  controls::ControlHost* GetControlHost();
  void InvalidateLayout();
  void InvalidatePaint();

 public:
  std::string GetName();
  std::string GetDebugPathInTree();

  CRU_DEFINE_EVENT(Destroy, RenderObject*)

 protected:
  /**
   * Implementation should adjust the requirement for content (like decreasing
   * the size of padding and margin), call OnMeasureContent to measure content
   * and adjust back to get the final measure result.
   *
   * The default implementation in RenderObject of calculating content's
   * requirement is first merging the given requirement with RenderObject's
   * custom measure requirement and then reducing GetTotalSpaceThickness.
   */
  virtual Size OnMeasureCore(const MeasureRequirement& requirement);

  // Please reduce margin and padding or other custom things and pass the result
  // content rect to OnLayoutContent.
  virtual void OnLayoutCore(const Rect& rect);

  // Override this function to measure content and children(Call Measure on
  // them). Do not consider margin or padding in this method because they are
  // already considered in OnMeasureCore. Returned size must obey requirement.
  // Caller should guarantee preferred_size is corerced into required range.
  virtual Size OnMeasureContent(const MeasureRequirement& requirement) = 0;

  // Layout all content and children(Call Layout on them).
  // Lefttop of content_rect should be added when calculated children's offset.
  virtual void OnLayoutContent(const Rect& content_rect) = 0;

  virtual void OnAttachedControlChanged(controls::Control* old_control,
                                        controls::Control* new_control) {}

  virtual void OnResize(const Size& new_size) {}

 private:
  std::string name_;
  controls::Control* control_;
  RenderObject* parent_;

  Point offset_;
  Size size_;

  Thickness margin_;
  Thickness padding_;

  bool layout_valid_;
  MeasureRequirement last_measure_requirement_;
  Size measure_result_size_;
  MeasureRequirement custom_measure_requirement_;
};
}  // namespace cru::ui::render
