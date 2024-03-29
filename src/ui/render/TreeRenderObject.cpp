#include "cru/ui/render/TreeRenderObject.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/ui/render/MeasureRequirement.h"
#include "cru/ui/render/RenderObject.h"

namespace cru::ui::render {
TreeRenderObjectItem::TreeRenderObjectItem(TreeRenderObject* tree_render_object,
                                           TreeRenderObjectItem* parent)
    : tree_render_object_(tree_render_object), parent_(parent) {}

TreeRenderObjectItem::~TreeRenderObjectItem() {
  if (render_object_) {
    render_object_->SetParent(nullptr);
    render_object_ = nullptr;
  }

  for (auto child : children_) {
    delete child;
  }
}

void TreeRenderObjectItem::SetRenderObject(RenderObject* render_object) {
  if (render_object == render_object_) return;
  if (render_object_) {
    render_object_->SetParent(nullptr);
  }
  render_object_ = render_object;
  if (render_object) {
    assert(render_object->GetParent() == nullptr);
    render_object->SetParent(tree_render_object_);
  }
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
  root_item_ = new TreeRenderObjectItem(this, nullptr);
}

TreeRenderObject::~TreeRenderObject() { delete root_item_; }

void TreeRenderObject::SetTabWidth(float tab_width) {
  if (tab_width == tab_width_) return;
  tab_width_ = tab_width;
  InvalidateLayout();
}

RenderObject* TreeRenderObjectItemHitTest(TreeRenderObjectItem* item,
                                          const Point& point) {
  auto render_object = item->GetRenderObject();
  if (render_object) {
    auto result = render_object->HitTest(point - render_object->GetOffset());
    if (result) return result;
  }

  for (auto child : item->GetChildren()) {
    auto result = TreeRenderObjectItemHitTest(child, point);
    if (result) return result;
  }

  return nullptr;
}

RenderObject* TreeRenderObject::HitTest(const Point& point) {
  return TreeRenderObjectItemHitTest(root_item_, point);
}

void TreeRenderObjectItemDraw(TreeRenderObjectItem* item,
                              platform::graphics::IPainter* painter) {
  auto render_object = item->GetRenderObject();
  if (render_object) {
    painter->PushState();
    painter->ConcatTransform(Matrix::Translation(render_object->GetOffset()));
    render_object->Draw(painter);
    painter->PopState();
  }

  for (auto child : item->GetChildren()) {
    TreeRenderObjectItemDraw(child, painter);
  }
}

void TreeRenderObject::Draw(platform::graphics::IPainter* painter) {
  TreeRenderObjectItemDraw(root_item_, painter);
}

static Size MeasureTreeRenderObjectItem(MeasureSize max_size,
                                        TreeRenderObjectItem* item,
                                        float tab_width) {
  auto render_object = item->GetRenderObject();
  if (render_object) {
    render_object->Measure(
        MeasureRequirement(max_size, MeasureSize::NotSpecified()),
        MeasureSize::NotSpecified());
  }

  Size item_size = render_object ? render_object->GetDesiredSize() : Size{};

  if (max_size.width.IsSpecified()) {
    max_size.width = max_size.width.GetLengthOrUndefined() - tab_width;
  }

  if (max_size.height.IsSpecified()) {
    max_size.height = max_size.height.GetLengthOrUndefined() - item_size.height;
  }

  Size result_size = item_size;

  for (auto child : item->GetChildren()) {
    auto child_size = MeasureTreeRenderObjectItem(max_size, child, tab_width);
    if (child_size.width > result_size.width)
      result_size.width = child_size.width;
    result_size.height += child_size.height;
  }

  return result_size;
}

Size TreeRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                        const MeasureSize& preferred_size) {
  auto size =
      MeasureTreeRenderObjectItem(requirement.max, root_item_, tab_width_);

  size = Max(size, requirement.min.GetSizeOr0());

  return size;
}

static void LayoutTreeRenderObjectItem(Rect rect, TreeRenderObjectItem* item,
                                       float tab_width) {
  auto render_object = item->GetRenderObject();
  float item_height = 0.f;
  if (render_object) {
    render_object->Layout(rect.GetLeftTop());
    item_height = render_object->GetDesiredSize().height;
  }

  rect.left += tab_width;
  rect.width -= tab_width;
  rect.top += item_height;
  rect.height -= item_height;

  for (auto child : item->GetChildren()) {
    LayoutTreeRenderObjectItem(rect, child, tab_width);
    auto child_render_object = child->GetRenderObject();
    auto child_height = child_render_object
                            ? child_render_object->GetDesiredSize().height
                            : 0.f;
    rect.top += child_height;
    rect.height -= child_height;
  }
}

void TreeRenderObject::OnLayoutContent(const Rect& content_rect) {
  LayoutTreeRenderObjectItem(content_rect, root_item_, tab_width_);
}
}  // namespace cru::ui::render
