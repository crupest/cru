#pragma once
#include "Base.hpp"

#include "MeasureRequirement.hpp"
#include "cru/common/Event.hpp"

namespace cru::ui::render {

// Render object will not destroy its children when destroyed. Control must
// manage lifecycle of its render objects. Since control will destroy its
// children when destroyed, render objects will be destroyed along with it.
//
// To write a custom RenderObject, override following methods:
// public:
//  void Draw(platform::graph::IPainter* painter) override;
//  RenderObject* HitTest(const Point& point) override;
// protected:
//  Size OnMeasureContent(const MeasureRequirement& requirement) override;
//  void OnLayoutContent(const Rect& content_rect) override;
class RenderObject : public Object {
  friend WindowRenderObject;

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

  UiHost* GetUiHost() const { return ui_host_; }

  RenderObject* GetParent() const { return parent_; }

  const std::vector<RenderObject*>& GetChildren() const { return children_; }
  Index GetChildCount() const { return static_cast<Index>(children_.size()); }
  void AddChild(RenderObject* render_object, Index position);
  void RemoveChild(Index position);

  // Offset from parent's lefttop to lefttop of this render object. Margin is
  // accounted for.
  Point GetOffset() const { return offset_; }
  Size GetSize() const { return size_; }
  Point GetTotalOffset() const;
  Point FromRootToContent(const Point& point) const;

  Thickness GetMargin() const { return margin_; }
  void SetMargin(const Thickness& margin) { margin_ = margin; }

  Thickness GetPadding() const { return padding_; }
  void SetPadding(const Thickness& padding) { padding_ = padding; }

  Size GetMeasuredSize() const { return measured_size_; }

  void Measure(const MeasureRequirement& requirement);
  // Size of rect must not be negative.
  void Layout(const Rect& rect);

  virtual void Draw(platform::graph::IPainter* painter) = 0;

  // Param point must be relative the lefttop of render object including margin.
  // Add offset before pass point to children.
  virtual RenderObject* HitTest(const Point& point) = 0;

 public:
  void InvalidateLayout();
  void InvalidatePaint();

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

  // Size measure including margin and padding. Please reduce margin and padding
  // or other custom things and pass the result content measure requirement to
  // OnMeasureContent.
  // Return value must not be negative and not bigger than requirement.
  virtual Size OnMeasureCore(const MeasureRequirement& requirement);

  // Size including margin and padding. Please reduce margin and padding or
  // other custom things and pass the result content rect to OnLayoutContent.
  // Parameter size are never negative.
  virtual void OnLayoutCore(const Size& size);

  // Do not consider margin or padding in this method because they are already
  // considered in OnMeasureCore. Returned size should never be bigger than
  // requirement.
  virtual Size OnMeasureContent(const MeasureRequirement& requirement) = 0;

  // Lefttop of content_rect should be added when calculated children's offset.
  virtual void OnLayoutContent(const Rect& content_rect) = 0;

  virtual void OnAfterLayout();
  static void NotifyAfterLayoutRecursive(RenderObject* render_object);

  virtual Rect GetPaddingRect() const;
  virtual Rect GetContentRect() const;

 private:
  void SetParent(RenderObject* new_parent);

  void SetRenderHostRecursive(UiHost* host);

 private:
  Control* control_ = nullptr;
  UiHost* ui_host_ = nullptr;

  RenderObject* parent_ = nullptr;
  std::vector<RenderObject*> children_{};

  ChildMode child_mode_ = ChildMode::None;

  Point offset_{};
  Size size_{};

  Thickness margin_{};
  Thickness padding_{};

  Size measured_size_{};
};
}  // namespace cru::ui::render
