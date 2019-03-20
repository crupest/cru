#pragma once
#include "pre.hpp"

#include <optional>
#include <vector>
#include "system_headers.hpp"

#include "base.hpp"
#include "ui/ui_base.hpp"

namespace cru::ui::render {
struct IRenderHost : Interface {
  virtual void InvalidatePaint() = 0;
  virtual void InvalidateLayout() = 0;
};

class RenderObject : public Object {
 protected:
  RenderObject() = default;

 public:
  RenderObject(const RenderObject& other) = delete;
  RenderObject(RenderObject&& other) = delete;
  RenderObject& operator=(const RenderObject& other) = delete;
  RenderObject& operator=(RenderObject&& other) = delete;
  ~RenderObject() override = default;

  IRenderHost* GetRenderHost() const { return render_host_; }
  void SetRenderHost(IRenderHost* new_render_host);

  RenderObject* GetParent() const { return parent_; }

  const std::vector<RenderObject*>& GetChildren() const { return children_; }
  void AddChild(RenderObject* render_object, int position);
  void RemoveChild(int position);

  Point GetOffset() const { return offset_; }
  void SetOffset(const Point& offset) { offset_ = offset; }
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

  virtual void Draw(ID2D1RenderTarget* render_target) = 0;

  virtual RenderObject* HitTest(const Point& point) = 0;

 protected:
  virtual void OnRenderHostChanged(IRenderHost* old_render_host,
                                   IRenderHost* new_render_host);

  void InvalidateRenderHostPaint() const;
  void InvalidateRenderHostLayout() const;

  virtual void OnParentChanged(RenderObject* old_parent,
                               RenderObject* new_parent);

  virtual void OnAddChild(RenderObject* new_child, int position);
  virtual void OnRemoveChild(RenderObject* removed_child, int position);

  virtual Size OnMeasureContent(const Size& available_size) = 0;
  virtual void OnLayoutContent(const Rect& content_rect) = 0;

 private:
  void SetParent(RenderObject* new_parent);

  void OnMeasureCore(const Size& available_size);
  void OnLayoutCore(const Rect& rect);

 private:
  IRenderHost* render_host_ = nullptr;

  RenderObject* parent_ = nullptr;
  std::vector<RenderObject*> children_{};

  Point offset_ = Point::Zero();
  Size size_ = Size::Zero();

  Thickness margin_ = Thickness::Zero();
  Thickness padding_ = Thickness::Zero();

  Size preferred_size_ = Size::Zero();
};
}  // namespace cru::ui::render
