#pragma once
#include "base.hpp"

#include "cru/common/event.hpp"

namespace cru::ui::render {

struct AfterLayoutEventArgs {};

struct IRenderHost : Interface {
  // Mark the layout as invalid, and arrange a re-layout later.
  // Note this method might be called more than one times in a message cycle. So
  // implementation should merge multiple request into once.
  virtual void InvalidateLayout() = 0;

  // Mark the paint as invalid, and arrange a re-paint later.
  // Note this method might be called more than one times in a message cycle. So
  // implementation should merge multiple request into once.
  virtual void InvalidatePaint() = 0;

  virtual IEvent<AfterLayoutEventArgs>* AfterLayoutEvent() = 0;
};

// Render object will not destroy its children when destroyed. Control must
// manage lifecycle of its render objects. Since control will destroy its
// children when destroyed, render objects will be destroyed along with it.
class RenderObject : public Object {
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

  IRenderHost* GetRenderHost() const { return render_host_; }
  void SetRenderHost(IRenderHost* render_host) { render_host_ = render_host; }

  RenderObject* GetParent() const { return parent_; }

  const std::vector<RenderObject*>& GetChildren() const { return children_; }
  int GetChildCount() const { return static_cast<int>(children_.size()); }
  void AddChild(RenderObject* render_object, int position);
  void RemoveChild(int position);

  Point GetOffset() const { return offset_; }
  void SetOffset(const Point& offset) { offset_ = offset; }
  Point GetTotalOffset() const;
  Point FromRootToContent(const Point& point) const;
  Size GetSize() const { return size_; }
  void SetSize(const Size& size) { size_ = size; }

  Thickness GetMargin() const { return margin_; }
  void SetMargin(const Thickness& margin) { margin_ = margin; }

  Thickness GetPadding() const { return padding_; }
  void SetPadding(const Thickness& padding) { padding_ = padding; }

  Size GetPreferredSize() const { return preferred_size_; }
  void SetPreferredSize(const Size& preferred_size) {
    preferred_size_ = preferred_size;
  }

  void Measure(const Size& available_size);
  void Layout(const Rect& rect);

  virtual void Draw(platform::graph::IPainter* painter) = 0;

  virtual RenderObject* HitTest(const Point& point) = 0;

 public:
  void InvalidateLayout() const {
    if (render_host_ != nullptr) render_host_->InvalidateLayout();
  }

  void InvalidatePaint() const {
    if (render_host_ != nullptr) render_host_->InvalidatePaint();
  }

 protected:
  void SetChildMode(ChildMode mode) { child_mode_ = mode; }

 protected:
  virtual void OnParentChanged(RenderObject* old_parent,
                               RenderObject* new_parent);

  // default is to invalidate both layout and paint
  virtual void OnAddChild(RenderObject* new_child, int position);
  // default is to invalidate both layout and paint
  virtual void OnRemoveChild(RenderObject* removed_child, int position);

  virtual void OnMeasureCore(const Size& available_size);
  virtual void OnLayoutCore(const Rect& rect);
  virtual Size OnMeasureContent(const Size& available_size) = 0;
  virtual void OnLayoutContent(const Rect& content_rect) = 0;

  virtual void OnAfterLayout();
  static void NotifyAfterLayoutRecursive(RenderObject* render_object);

  Rect GetPaddingRect() const;
  Rect GetContentRect() const;

 private:
  void SetParent(RenderObject* new_parent);

  void SetRenderHostRecursive(IRenderHost* host);

 private:
  Control* control_ = nullptr;
  IRenderHost* render_host_ = nullptr;

  RenderObject* parent_ = nullptr;
  std::vector<RenderObject*> children_{};

  ChildMode child_mode_ = ChildMode::None;

  Point offset_{};
  Size size_{};

  Thickness margin_{};
  Thickness padding_{};

  Size preferred_size_{};
};
}  // namespace cru::ui::render
