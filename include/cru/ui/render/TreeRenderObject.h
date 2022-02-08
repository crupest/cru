#pragma once
#include "RenderObject.h"

namespace cru::ui::render {
class TreeRenderObject;

class CRU_UI_API TreeRenderObjectItem : public Object {
  friend TreeRenderObject;

 private:
  explicit TreeRenderObjectItem(TreeRenderObject* tree_render_object,
                                TreeRenderObjectItem* parent);

 public:
  CRU_DELETE_COPY(TreeRenderObjectItem)
  CRU_DELETE_MOVE(TreeRenderObjectItem)
  ~TreeRenderObjectItem() override;

  RenderObject* GetRenderObject() { return render_object_; }
  void SetRenderObject(RenderObject* render_object);

  const std::vector<TreeRenderObjectItem*>& GetChildren() const {
    return children_;
  }

  Index GetChildCount() const { return children_.size(); }

  TreeRenderObjectItem* GetChild(Index index) {
    Expects(index >= 0 && index < children_.size());
    return children_[index];
  }

  TreeRenderObjectItem* AddItem(Index position);
  void RemoveItem(Index position);

 private:
  TreeRenderObject* tree_render_object_;

  TreeRenderObjectItem* parent_;
  std::vector<TreeRenderObjectItem*> children_;

  RenderObject* render_object_;

  Rect rect_cache_;
};

class CRU_UI_API TreeRenderObject : public RenderObject {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::render:TreeRenderObject")

 public:
  TreeRenderObject();
  CRU_DELETE_COPY(TreeRenderObject)
  CRU_DELETE_MOVE(TreeRenderObject)
  ~TreeRenderObject() override;

  std::u16string_view GetName() const override { return u"TreeRenderObject"; }

  TreeRenderObjectItem* GetRootItem() { return root_item_; }

  float GetTabWidth() const { return tab_width_; }
  void SetTabWidth(float tab_width);

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  float tab_width_ = 12.f;

  TreeRenderObjectItem* root_item_;
};
}  // namespace cru::ui::render
