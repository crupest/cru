#pragma once
#include "../Base.h"

#include "MeasureRequirement.h"
#include "cru/common/String.h"

namespace cru::ui::render {
// Render object will not destroy its children when destroyed. Control must
// manage lifecycle of its render objects. Since control will destroy its
// children when destroyed, render objects will be destroyed along with it.
//
// About layout:
// Render object should be able to deal with arbitrary size as the result of
// measure and layout.
//
// Parent may pass calculated preferred size down. But the preferred size set on
// child itself takes precedence.
//
// Each render object should obey the measure requirement to set size and report
// a warning when that requirement can't be satisfied with probably bigger size
// of children than that of itself and optional visual effect to indicate that.
//
// If size of chilren are less than min size requirement, then render object
// should try to fill the rest area. If size of children are more than max size
// requirement, then render object should display a warning of the layout
// problem and use the max size of itself with children exceeding its bound.
// (Or better you could use some visual effect to indicate that.)
//
// To write a custom RenderObject, override following methods:
// public:
//  void Draw(platform::graphics::IPainter* painter) override;
//  RenderObject* HitTest(const Point& point) override;
// protected:
//  Size OnMeasureContent(const MeasureRequirement& requirement) override;
//  void OnLayoutContent(const Rect& content_rect) override;
class CRU_UI_API RenderObject : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::render::RenderObject")

 protected:
  RenderObject() = default;

 public:
  CRU_DELETE_COPY(RenderObject)
  CRU_DELETE_MOVE(RenderObject)
  ~RenderObject() override = default;

  controls::Control* GetAttachedControl() const { return control_; }
  void SetAttachedControl(controls::Control* new_control);

  RenderObject* GetParent() const { return parent_; }
  void SetParent(RenderObject* new_parent) { parent_ = new_parent; }

  // Offset from parent's lefttop to lefttop of this render object. Margin is
  // accounted for.
  Point GetOffset() const { return offset_; }
  Size GetSize() const { return size_; }

  Point GetTotalOffset() const;
  Point FromRootToContent(const Point& point) const;

  Size GetDesiredSize() const { return desired_size_; }

  Thickness GetMargin() const { return margin_; }
  void SetMargin(const Thickness& margin);

  Thickness GetPadding() const { return padding_; }
  void SetPadding(const Thickness& padding);

  MeasureSize GetPreferredSize() const { return preferred_size_; }
  void SetPreferredSize(const MeasureSize& preferred_size);

  MeasureSize GetMinSize() const { return custom_measure_requirement_.min; }
  void SetMinSize(const MeasureSize& min_size);

  MeasureSize GetMaxSize() const { return custom_measure_requirement_.max; }
  void SetMaxSize(const MeasureSize& max_size);

  MeasureRequirement GetCustomMeasureRequirement() const {
    return custom_measure_requirement_;
  }

  // This method will merge requirement passed by argument and requirement of
  // the render object using MeasureRequirement::Merge and then call
  // MeasureRequirement::Normalize on it. And it will use preferred size of the
  // render object to override the one passed by argument. Then pass the two to
  // OnMeasureCore and use the return value of it to set the size of this render
  // object. This can be called multiple times on children during measure to
  // adjust for better size.
  void Measure(const MeasureRequirement& requirement,
               const MeasureSize& preferred_size);
  // This will set offset of this render object and call OnLayoutCore.
  void Layout(const Point& offset);

  virtual Thickness GetOuterSpaceThickness() const;
  virtual Rect GetPaddingRect() const;
  virtual Rect GetContentRect() const;

  virtual void Draw(platform::graphics::IPainter* painter) = 0;

  // Param point must be relative the lefttop of render object including margin.
  // Add offset before pass point to children.
  virtual RenderObject* HitTest(const Point& point) = 0;

 public:
  host::WindowHost* GetWindowHost();
  void InvalidateLayout();
  void InvalidatePaint();

 public:
  virtual String GetName() const;
  String GetDebugPathInTree() const;

 protected:
  // Size measure including margin and padding. Please reduce margin and padding
  // or other custom things and pass the result content measure requirement and
  // preferred size to OnMeasureContent. Return value must not be negative and
  // must obey requirement.
  // Note: Implementation should coerce the preferred size into the requirement
  // when pass them to OnMeasureContent.
  virtual Size OnMeasureCore(const MeasureRequirement& requirement,
                             const MeasureSize& preferred_size);

  // Please reduce margin and padding or other custom things and pass the result
  // content rect to OnLayoutContent.
  virtual void OnLayoutCore();

  // Override this function to measure content and children(Call Measure on
  // them). Do not consider margin or padding in this method because they are
  // already considered in OnMeasureCore. Returned size must obey requirement.
  // Caller should guarantee preferred_size is corerced into required range.
  virtual Size OnMeasureContent(const MeasureRequirement& requirement,
                                const MeasureSize& preferred_size) = 0;

  // Layout all content and children(Call Layout on them).
  // Lefttop of content_rect should be added when calculated children's offset.
  virtual void OnLayoutContent(const Rect& content_rect) = 0;

  virtual void OnAttachedControlChanged(controls::Control* old_control,
                                        controls::Control* new_control) {}
  virtual void OnResize(const Size& new_size) {}

 private:
  controls::Control* control_ = nullptr;

  RenderObject* parent_ = nullptr;

  Point offset_{};
  Size size_{};

  Size desired_size_{};

  Thickness margin_{};
  Thickness padding_{};

  MeasureSize preferred_size_{};
  MeasureRequirement custom_measure_requirement_{};
};
}  // namespace cru::ui::render
