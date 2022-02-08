#include "cru/ui/render/TreeRenderObject.h"

namespace cru::ui::render {
TreeRenderObjectItem::TreeRenderObjectItem(TreeRenderObject* tree_render_object,
                                           TreeRenderObjectItem* parent)
    : tree_render_object_(tree_render_object), parent_(parent) {}

TreeRenderObjectItem::~TreeRenderObjectItem() {
  for (auto child : children_) {
    delete child;
  }
}

void TreeRenderObjectItem::SetRenderObject(RenderObject* render_object) {
  if (render_object == render_object_) return;
  render_object_ = render_object;
  tree_render_object_->InvalidateLayout();
}

TreeRenderObjectItem* TreeRenderObjectItem::AddItem(Index position) {
  auto item = new TreeRenderObjectItem(tree_render_object_, this);
  children_.insert(children_.begin() + position, item);
  return item;
}

void TreeRenderObjectItem::RemoveItem(Index position) {
  Expects(position >= 0 && position < children_.size());
  delete children_[position];
  children_.erase(children_.begin() + position);
  tree_render_object_->InvalidateLayout();
}

TreeRenderObject::TreeRenderObject() {
  SetChildMode(ChildMode::None);
  root_item_ = new TreeRenderObjectItem(this, nullptr);
}

TreeRenderObject::~TreeRenderObject() { delete root_item_; }

Size TreeRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                        const MeasureSize& preferred_size) {
  throw Exception(u"Not implemented.");
}

void TreeRenderObject::OnLayoutContent(const Rect& content_rect) {
  throw Exception(u"Not implemented.");
}
}  // namespace cru::ui::render
