#pragma once
#include "RenderObject.h"

#include "cru/platform/graphics/Painter.h"

namespace cru::ui::render {
template <typename TChildLayoutData>
class LayoutRenderObject : public RenderObject {
 public:
  using ChildLayoutData = TChildLayoutData;

 private:
  struct ChildData {
    /**
     * May be nullptr.
     */
    RenderObject* render_object;
    ChildLayoutData layout_data;
    EventHandlerRevokerListGuard event_guard;
  };

 protected:
  LayoutRenderObject(std::string name) : RenderObject(std::move(name)) {}

 public:
  Index GetChildCount() { return static_cast<Index>(children_.size()); }

  RenderObject* GetChildAt(Index position) {
    Expects(position >= 0 && position < GetChildCount());
    return children_[position].render_object;
  }

  void AddChild(RenderObject* render_object, Index position) {
    if (position < 0) position = 0;
    if (position > GetChildCount()) position = GetChildCount();
    auto iter = children_.insert(children_.begin() + position,
                                 ChildData{render_object, ChildLayoutData()});
    render_object->SetParent(this);
    iter->event_guard +=
        render_object->DestroyEvent()->AddSpyOnlyHandler([this, render_object] {
          auto iter = std::ranges::find_if(
              children_, [render_object](const ChildData& data) {
                return data.render_object == render_object;
              });
          if (iter != children_.cend()) {
            iter->render_object = nullptr;
          }
        });
    InvalidateLayout();
  }

  void RemoveChild(Index position) {
    Expects(position >= 0 && position < GetChildCount());
    auto render_object = children_[position].render_object;
    if (render_object) {
      render_object->SetParent(nullptr);
    }
    children_.erase(children_.begin() + position);
    InvalidateLayout();
  }

  void ClearChildren() {
    for (auto child : children_) {
      auto render_object = child.render_object;
      if (render_object) {
        render_object->SetParent(nullptr);
      }
    }
    children_.clear();
    InvalidateLayout();
  }

  const ChildLayoutData& GetChildLayoutDataAt(Index position) {
    Expects(position >= 0 && position < GetChildCount());
    return children_[position].layout_data;
  }

  void SetChildLayoutDataAt(Index position, ChildLayoutData data) {
    Expects(position >= 0 && position < GetChildCount());
    children_[position].layout_data = std::move(data);
    InvalidateLayout();
  }

  void Draw(platform::graphics::IPainter* painter) override {
    for (const auto& child : children_) {
      painter->PushState();
      painter->ConcatTransform(
          Matrix::Translation(child.render_object->GetOffset()));
      child.render_object->Draw(painter);
      painter->PopState();
    }
  }

  RenderObject* HitTest(const Point& point) override {
    const auto child_count = GetChildCount();
    for (auto i = child_count - 1; i >= 0; --i) {
      const auto child = GetChildAt(i);
      const auto result = child->HitTest(point - child->GetOffset());
      if (result != nullptr) {
        return result;
      }
    }

    return GetPaddingRect().IsPointInside(point) ? this : nullptr;
  }

 private:
  std::vector<ChildData> children_;
};
}  // namespace cru::ui::render
