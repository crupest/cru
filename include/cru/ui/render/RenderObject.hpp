#pragma once
#include "Base.hpp"

#include "MeasureRequirement.hpp"
#include "cru/common/Event.hpp"
#include "cru/ui/Base.hpp"

#include <cstddef>
#include <string>
#include <string_view>

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
//  void Draw(platform::graph::IPainter* painter) override;
//  RenderObject* HitTest(const Point& point) override;
// protected:
//  Size OnMeasureContent(const MeasureRequirement& requirement) override;
//  void OnLayoutContent(const Rect& content_rect) override;
class RenderObject : public Object {
  friend WindowHost;

  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::render::RenderObject")

 protected:
  enum class ChildMode {
    None,
    Single,
    Multiple,
  };

  RenderObject() = default;
  RenderObject(ChildMode child_mode) : RenderObject() {
    SetChildMode(child_mode);
  }

 public:
  RenderObject(const RenderObject& other) = delete;
  RenderObject(RenderObject&& other) = delete;
  RenderObject& operator=(const RenderObject& other) = delete;
  RenderObject& operator=(RenderObject&& other) = delete;
  ~RenderObject() override = default;

  Control* GetAttachedControl() const { return control_; }
  void SetAttachedControl(Control* new_control) { control_ = new_control; }

  WindowHost* GetWindowHost() const { return window_host_; }

  RenderObject* GetParent() const { return parent_; }

  const std::vector<RenderObject*>& GetChildren() const { return children_; }
  Index GetChildCount() const { return static_cast<Index>(children_.size()); }
  void AddChild(RenderObject* render_object, Index position);
  void RemoveChild(Index position);

  void TraverseDescendants(const std::function<void(RenderObject*)>& action);

  // Offset from parent's lefttop to lefttop of this render object. Margin is
  // accounted for.
  Point GetOffset() const { return offset_; }
  Size GetSize() const { return size_; }
  Point GetTotalOffset() const;
  Point FromRootToContent(const Point& point) const;

  Thickness GetMargin() const { return margin_; }
  void SetMargin(const Thickness& margin) {
    margin_ = margin;
    InvalidateLayout();
  }

  Thickness GetPadding() const { return padding_; }
  void SetPadding(const Thickness& padding) {
    padding_ = padding;
    InvalidateLayout();
  }

  MeasureSize GetPreferredSize() const { return preferred_size_; }
  void SetPreferredSize(const MeasureSize& preferred_size) {
    preferred_size_ = preferred_size;
    InvalidateLayout();
  }

  MeasureSize GetMinSize() const { return custom_measure_requirement_.min; }
  void SetMinSize(const MeasureSize& min_size) {
    custom_measure_requirement_.min = min_size;
    InvalidateLayout();
  }

  MeasureSize GetMaxSize() const { return custom_measure_requirement_.max; }
  void SetMaxSize(const MeasureSize& max_size) {
    custom_measure_requirement_.max = max_size;
    InvalidateLayout();
  }

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

  void Draw(platform::graph::IPainter* painter);

  // Param point must be relative the lefttop of render object including margin.
  // Add offset before pass point to children.
  virtual RenderObject* HitTest(const Point& point) = 0;

  IEvent<WindowHost*>* AttachToHostEvent() { return &attach_to_host_event_; }
  IEvent<std::nullptr_t>* DetachFromHostEvent() {
    return &detach_from_host_event_;
  }

 public:
  void InvalidateLayout();
  void InvalidatePaint();

 public:
  virtual std::u16string_view GetName() const;
  std::u16string GetDebugPathInTree() const;

 protected:
  void SetChildMode(ChildMode mode) { child_mode_ = mode; }

 protected:
  RenderObject* GetSingleChild() const;

  virtual void OnParentChanged(RenderObject* old_parent,
                               RenderObject* new_parent);

  // default is to invalidate both layout and paint
  virtual void OnAddChild(RenderObject* new_child, Index position);
  // default is to invalidate both layout and paint
  virtual void OnRemoveChild(RenderObject* removed_child, Index position);

  // Draw all children with offset.
  void DefaultDrawChildren(platform::graph::IPainter* painter);

  // Draw all children with translation of content rect lefttop.
  void DefaultDrawContent(platform::graph::IPainter* painter);

  // Call DefaultDrawContent. Then call DefaultDrawChildren.
  virtual void OnDrawCore(platform::graph::IPainter* painter);

  virtual void OnDrawContent(platform::graph::IPainter* painter);

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

  virtual void OnAfterLayout();

  virtual Rect GetPaddingRect() const;
  virtual Rect GetContentRect() const;

 private:
  void SetParent(RenderObject* new_parent);

  void SetWindowHostRecursive(WindowHost* host);

 private:
  Control* control_ = nullptr;
  WindowHost* window_host_ = nullptr;

  RenderObject* parent_ = nullptr;
  std::vector<RenderObject*> children_{};

  ChildMode child_mode_ = ChildMode::None;

  Point offset_{};
  Size size_{};

  MeasureSize preferred_size_;
  MeasureRequirement custom_measure_requirement_;

  Thickness margin_{};
  Thickness padding_{};

  Event<WindowHost*> attach_to_host_event_;
  Event<std::nullptr_t> detach_from_host_event_;
};
}  // namespace cru::ui::render
