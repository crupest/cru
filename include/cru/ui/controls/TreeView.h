#pragma once
#include "Control.h"

namespace cru::ui::controls {
class TreeView;

class CRU_UI_API TreeViewItem : public Object {
  friend TreeView;

 public:
  TreeViewItem(TreeView* tree_view, TreeViewItem* parent);
  CRU_DELETE_COPY(TreeViewItem)
  CRU_DELETE_MOVE(TreeViewItem)
  ~TreeViewItem() override;

  TreeView* GetTreeView() { return tree_view_; }
  TreeViewItem* GetParent() { return parent_; }

  const std::vector<TreeViewItem*>& GetChildren() const { return children_; }
  Index GetChildCount() const { return children_.size(); }
  TreeViewItem* GetChildAt(Index index) {
    Expects(index >= 0 && index < children_.size());
    return children_[index];
  }

  TreeViewItem* AddItem(Index position);
  void RemoveItem(Index position);

 private:
  TreeView* tree_view_;
  TreeViewItem* parent_;
  std::vector<TreeViewItem*> children_;

  Control* control_;
};

}  // namespace cru::ui::controls
