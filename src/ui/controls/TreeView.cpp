#include "cru/ui/controls/TreeView.h"

namespace cru::ui::controls {
TreeViewItem::TreeViewItem(TreeView* tree_view, TreeViewItem* parent,
                           render::TreeRenderObjectItem* render_object_item)
    : tree_view_(tree_view),
      parent_(parent),
      render_object_item_(render_object_item) {}

TreeViewItem::~TreeViewItem() {
  if (control_) {
    control_->SetParent(nullptr);
  }

  for (auto item : children_) {
    delete item;
  }
}

Index TreeViewItem::IndexOf(TreeViewItem* item) const {
  auto result = std::find(children_.begin(), children_.end(), item);
  if (result == children_.end()) {
    return -1;
  }
  return result - children_.begin();
}

void TreeViewItem::RemoveFromParent() {
  if (parent_ == nullptr) return;
  parent_->RemoveItem(parent_->IndexOf(this));
}

TreeViewItem* TreeViewItem::AddItem(Index position) {
  auto render_object_item = render_object_item_->AddItem(position);
  auto item = new TreeViewItem(tree_view_, this, render_object_item);
  children_.insert(children_.begin() + position, item);
  return item;
}

void TreeViewItem::RemoveItem(Index position) {
  Expects(position >= 0 && position < children_.size());
  delete children_[position];
  children_.erase(children_.begin() + position);
  render_object_item_->RemoveItem(position);
}

void TreeViewItem::SetControl(Control* control) {
  if (control_) {
    control_->SetParent(nullptr);
    render_object_item_->SetRenderObject(nullptr);
  }
  control_ = control;
  if (control) {
    control->SetParent(tree_view_);
    render_object_item_->SetRenderObject(control->GetRenderObject());
  }
}

void TreeViewItem::TraverseDescendants(
    std::function<void(TreeViewItem*)> callback) {
  callback(this);
  for (auto item : children_) {
    TraverseDescendants(callback);
  }
}

TreeView::TreeView()
    : root_item_(this, nullptr, render_object_.GetRootItem()) {}

TreeView::~TreeView() {}

void TreeView::ForEachChild(const std::function<void(Control*)>& predicate) {
  root_item_.TraverseDescendants([&predicate](TreeViewItem* item) {
    if (auto control = item->GetControl()) {
      predicate(control);
    }
  });
}

void TreeView::RemoveChild(Control* control) {
  root_item_.TraverseDescendants([&control](TreeViewItem* item) {
    if (item->GetControl() == control) {
      item->SetControl(nullptr);
    }
  });
}
}  // namespace cru::ui::controls
