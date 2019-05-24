#pragma once
#include "cru/common/base.hpp"

#include "cru/common/ui_base.hpp"

#include <vector>

// forward declarations
namespace cru::ui {
class Control;
}

namespace cru::platform::graph {
struct IPainter;
}

namespace cru::ui::render {
class RenderObject : public Object {
 protected:
  RenderObject() = default;

 public:
  RenderObject(const RenderObject& other) = delete;
  RenderObject(RenderObject&& other) = delete;
  RenderObject& operator=(const RenderObject& other) = delete;
  RenderObject& operator=(RenderObject&& other) = delete;
  ~RenderObject() override = default;

  Control* GetAttachedControl() const { return control_; }
  void SetAttachedControl(Control* new_control) { control_ = new_control; }

  RenderObject* GetParent() const { return parent_; }

  const std::vector<RenderObject*>& GetChildren() const { return children_; }
  void AddChild(RenderObject* render_object, int position);
  void RemoveChild(int position);

  Point GetOffset() const { return offset_; }
  void SetOffset(const Point& offset) { offset_ = offset; }
  Size GetSize() const { return size_; }
  void SetSize(const Size& size) {
    const auto old_size = size_;
    size_ = size;
    OnSizeChanged(old_size, size);
  }

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

 protected:
  virtual void OnParentChanged(RenderObject* old_parent,
                               RenderObject* new_parent);

  virtual void OnAddChild(RenderObject* new_child, int position);
  virtual void OnRemoveChild(RenderObject* removed_child, int position);

  virtual void OnSizeChanged(const Size& old_size, const Size& new_size);

  virtual void OnMeasureCore(const Size& available_size);
  virtual void OnLayoutCore(const Rect& rect);
  virtual Size OnMeasureContent(const Size& available_size) = 0;
  virtual void OnLayoutContent(const Rect& content_rect) = 0;

  Rect GetContentRect() const;

 private:
  void SetParent(RenderObject* new_parent);

 private:
  Control* control_ = nullptr;

  RenderObject* parent_ = nullptr;
  std::vector<RenderObject*> children_{};

  Point offset_{};
  Size size_{};

  Thickness margin_{};
  Thickness padding_{};

  Size preferred_size_{};
};
}  // namespace cru::ui::render
