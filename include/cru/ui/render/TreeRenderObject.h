#pragma once
#include "RenderObject.h"

#include <cru/platform/graphics/Painter.h>

namespace cru::ui::render {
class TreeRenderObject;

class CRU_UI_API TreeRenderObjectItem : public Object {
  friend TreeRenderObject;

 private:
  explicit TreeRenderObjectItem(TreeRenderObject* tree_render_object,
                                TreeRenderObjectItem* parent);

 public:
  ~TreeRenderObjectItem() override;

  RenderObject* GetRenderObject() { return render_object_; }
  void SetRenderObject(RenderObject* render_object);

  const std::vector<TreeRenderObjectItem*>& GetChildren() { return children_; }

  Index GetChildCount() { return children_.size(); }

  TreeRenderObjectItem* GetChildAt(Index index) {
    Expects(index >= 0 && index < children_.size());
    return children_[index];
  }

  TreeRenderObjectItem* AddItem(Index position);
  void RemoveItem(Index position);

  void* GetUserData() { return user_data_; }
  void SetUserData(void* user_data) { user_data_ = user_data; }

 private:
  TreeRenderObject* tree_render_object_;

  TreeRenderObjectItem* parent_;
  std::vector<TreeRenderObjectItem*> children_;

  RenderObject* render_object_;

  void* user_data_;
};

class CRU_UI_API TreeRenderObject : public RenderObject {
  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::render::TreeRenderObject")

 public:
  static constexpr auto kRenderObjectName = "TreeRenderObject";

  TreeRenderObject();
  ~TreeRenderObject() override;

  TreeRenderObjectItem* GetRootItem() { return root_item_; }

  float GetTabWidth() { return tab_width_; }
  void SetTabWidth(float tab_width);

  RenderObject* HitTest(const Point& point) override;

  void Draw(platform::graphics::IPainter* painter) override;

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  float tab_width_ = 12.f;

  TreeRenderObjectItem* root_item_;
};
}  // namespace cru::ui::render
