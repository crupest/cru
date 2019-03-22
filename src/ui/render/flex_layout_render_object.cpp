#include "flex_layout_render_object.hpp"

#include <algorithm>
#include <functional>

#include "cru_debug.hpp"
#include "graph/graph.hpp"

namespace cru::ui::render {
FlexChildLayoutData* FlexLayoutRenderObject::GetChildLayoutData(int position) {
  assert(position >= 0 &&
         position < child_layout_data_.size());  // Position out of bound.

  return &child_layout_data_[position];
}

void FlexLayoutRenderObject::Draw(ID2D1RenderTarget* render_target) {
  for (const auto child : GetChildren()) {
    auto offset = child->GetOffset();
    graph::WithTransform(render_target,
                         D2D1::Matrix3x2F::Translation(offset.x, offset.y),
                         [child](auto rt) { child->Draw(rt); });
  }
}

RenderObject* FlexLayoutRenderObject::HitTest(const Point& point) {
  const auto& children = GetChildren();
  for (auto i = children.crbegin(); i != children.crend(); ++i) {
    auto offset = (*i)->GetOffset();
    Point p{point.x - offset.x, point.y - offset.y};
    const auto result = (*i)->HitTest(point);
    if (result != nullptr) {
      return result;
    }
  }
  return Rect{Point::Zero(), GetSize()}.IsPointInside(point) ? this : nullptr;
}

void FlexLayoutRenderObject::OnAddChild(RenderObject* new_child, int position) {
  child_layout_data_.emplace(child_layout_data_.cbegin() + position);
}

void FlexLayoutRenderObject::OnRemoveChild(RenderObject* removed_child,
                                           int position) {
  child_layout_data_.erase(child_layout_data_.cbegin() + position);
}

Size FlexLayoutRenderObject::OnMeasureContent(const Size& available_size) {
  std::vector<int> has_basis_children;
  std::vector<int> no_basis_children;
  std::vector<int> grow_children;
  std::vector<int> shrink_chilren;
  for (int i = 0; i < child_layout_data_.size(); i++) {
    const auto& layout_data = child_layout_data_[i];
    if (layout_data.flex_basis.has_value())
      has_basis_children.push_back(i);
    else
      no_basis_children.push_back(i);
    if (layout_data.flex_grow > 0) grow_children.push_back(i);
    if (layout_data.flex_shrink > 0) shrink_chilren.push_back(i);
  }

  std::function<float(const Size&)> get_main_length;
  std::function<float(const Size&)> get_cross_length;
  std::function<Size(float main, float cross)> create_size;

  if (direction_ == FlexDirection::Horizontal ||
      direction_ == FlexDirection::HorizontalReverse) {
    get_main_length = [](const Size& size) { return size.width; };
    get_cross_length = [](const Size& size) { return size.height; };
    create_size = [](float main, float cross) { return Size(main, cross); };
  } else {
    get_main_length = [](const Size& size) { return size.height; };
    get_cross_length = [](const Size& size) { return size.width; };
    create_size = [](float main, float cross) { return Size(cross, main); };
  }

  const auto& children = GetChildren();

  float remain_main_length = get_main_length(available_size);
  float max_cross_length = 0;

  for (const int i : has_basis_children) {
    const auto child = children[i];
    const float basis = child_layout_data_[i].flex_basis.value();
    child->Measure(create_size(basis, get_cross_length(available_size)));
    remain_main_length -= basis;
    const float child_preferred_cross_length =
        get_cross_length(child->GetPreferredSize());
    child->SetPreferredSize(create_size(basis, child_preferred_cross_length));
    max_cross_length = std::max(max_cross_length, child_preferred_cross_length);
  }

  for (const int i : no_basis_children) {
    const auto child = children[i];
    child->Measure(create_size(remain_main_length > 0 ? remain_main_length : 0,
                               get_cross_length(available_size)));
    remain_main_length -= get_main_length(child->GetPreferredSize());
    max_cross_length =
        std::max(max_cross_length, get_cross_length(child->GetPreferredSize()));
  }

  if (remain_main_length > 0) {
    float total_grow = 0;
    for (const int i : grow_children)
      total_grow += child_layout_data_[i].flex_grow;

    for (const int i : grow_children) {
      const float distributed_grow_length =
          remain_main_length * (child_layout_data_[i].flex_grow / total_grow);
      const auto child = children[i];
      const float new_main_length =
          get_main_length(child->GetPreferredSize()) + distributed_grow_length;
      child->Measure(
          create_size(new_main_length, get_cross_length(available_size)));
      const float new_child_preferred_cross_length =
          get_cross_length(child->GetPreferredSize());
      child->SetPreferredSize(
          create_size(new_main_length, new_child_preferred_cross_length));
      max_cross_length =
          std::max(max_cross_length, new_child_preferred_cross_length);
    }
  }

  if (remain_main_length < 0) {
    float total_shrink = 0;
    for (const int i : shrink_chilren)
      total_shrink += child_layout_data_[i].flex_shrink;

    for (const int i : shrink_chilren) {
      const float distributed_shrink_length =  // negative
          remain_main_length *
          (child_layout_data_[i].flex_shrink / total_shrink);
      const auto child = children[i];
      float new_main_length = get_main_length(child->GetPreferredSize()) +
                              distributed_shrink_length;
      new_main_length = new_main_length > 0 ? new_main_length : 0;
      child->Measure(
          create_size(new_main_length, get_cross_length(available_size)));
      const float new_child_preferred_cross_length =
          get_cross_length(child->GetPreferredSize());
      child->SetPreferredSize(
          create_size(new_main_length, new_child_preferred_cross_length));
      max_cross_length =
          std::max(max_cross_length, new_child_preferred_cross_length);
    }
  }

  return create_size(get_main_length(available_size) -
                         (remain_main_length > 0 ? remain_main_length : 0),
                     max_cross_length);
}

void FlexLayoutRenderObject::OnLayoutContent(const Rect& content_rect) {
  auto calculate_anchor = [](Alignment alignment, float start_point,
                             float total_length,
                             float content_length) -> float {
    switch (alignment) {
      case Alignment::Start:
        return start_point;
      case Alignment::Center:
        return start_point + (total_length - content_length) / 2.0f;
      case Alignment::End:
        return start_point + total_length - content_length;
      default:
        UnreachableCode();
    }
  };

  const auto& children = GetChildren();
  if (direction_ == FlexDirection::Horizontal ||
      direction_ == FlexDirection::HorizontalReverse) {
    float actual_content_width = 0;
    for (const auto child : children) {
      actual_content_width += child->GetPreferredSize().width;
    }

    const float content_anchor_x = calculate_anchor(
        content_main_align_, 0, content_rect.width, actual_content_width);

    auto anchor_x = 0;
    for (int i = 0; i < children.size(); i++) {
      const auto child = children[i];
      const auto size = child->GetPreferredSize();

      float real_anchor_x = anchor_x + content_anchor_x;
      if (direction_ == FlexDirection::Horizontal)
        real_anchor_x = content_rect.left + real_anchor_x;
      else
        real_anchor_x = content_rect.GetRight() - real_anchor_x;
      child->Layout(Rect{
          real_anchor_x,
          calculate_anchor(child_layout_data_[i].alignment, content_rect.top,
                           content_rect.height, size.height),
          size.width, size.height});

      anchor_x += size.width;
    }
  } else {
    float actual_content_height = 0;
    for (const auto child : children) {
      actual_content_height = child->GetPreferredSize().height;
    }

    const float content_anchor_y = calculate_anchor(
        content_main_align_, 0, content_rect.height, actual_content_height);

    auto anchor_y = 0;
    for (int i = 0; i < children.size(); i++) {
      const auto child = children[i];
      const auto size = child->GetPreferredSize();

      float real_anchor_y = anchor_y + content_anchor_y;
      if (direction_ == FlexDirection::Vertical) {
        real_anchor_y = content_rect.top + real_anchor_y;
      } else {
        real_anchor_y = content_rect.GetBottom() - real_anchor_y;
      }
      child->Layout(Rect{
          real_anchor_y,
          calculate_anchor(child_layout_data_[i].alignment, content_rect.left,
                           content_rect.width, size.width),
          size.width, size.height});

      anchor_y += size.height;
    }
  }
}
}  // namespace cru::ui::render
