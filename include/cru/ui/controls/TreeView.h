#pragma once
#include "Control.h"
#include "cru/ui/render/TreeRenderObject.h"

namespace cru::ui::controls {
class TreeView;

class CRU_UI_API TreeViewItem : public Object {
  friend TreeView;

 private:
  TreeViewItem(TreeView* tree_view, TreeViewItem* parent,
               render::TreeRenderObjectItem* render_object_item);
  ~TreeViewItem() override;

 public:
  CRU_DELETE_COPY(TreeViewItem)
  CRU_DELETE_MOVE(TreeViewItem)

  TreeView* GetTreeView() { return tree_view_; }
  TreeViewItem* GetParent() { return parent_; }

  render::TreeRenderObjectItem* GetRenderObjectItem() const {
    return render_object_item_;
  }

  const std::vector<TreeViewItem*>& GetChildren() const { return children_; }
  Index GetChildCount() const { return children_.size(); }
  TreeViewItem* GetChildAt(Index index) {
    Expects(index >= 0 && index < children_.size());
    return children_[index];
  }
  Index IndexOf(TreeViewItem* item) const;

  TreeViewItem* AddItem(Index position);
  void RemoveItem(Index position);
  void RemoveFromParent();

  Control* GetControl() const { return control_; }
  void SetControl(Control* control);

  void TraverseDescendants(std::function<void(TreeViewItem*)> callback);

 private:
  TreeView* tree_view_;
  render::TreeRenderObjectItem* render_object_item_;
  TreeViewItem* parent_;
  std::vector<TreeViewItem*> children_;

  Control* control_;
};

class CRU_UI_API TreeView : public Control {
 public:
  constexpr static StringView kControlType = u"TreeView";

  TreeView();
  CRU_DELETE_COPY(TreeView)
  CRU_DELETE_MOVE(TreeView)
  ~TreeView() override;

  String GetControlType() const override { return kControlType.ToString(); }
  render::TreeRenderObject* GetRenderObject() { return &render_object_; }

  void ForEachChild(const std::function<void(Control*)>& predicate) override;
  void RemoveChild(Control* control) override;

  TreeViewItem* GetRootItem() { return &root_item_; }

 private:
  render::TreeRenderObject render_object_;
  TreeViewItem root_item_;
};
}  // namespace cru::ui::controls
