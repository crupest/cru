#include "cru/ui/render/flex_layout_render_object.hpp"

#include "cru/platform/graph/util/painter.hpp"

#include <algorithm>
#include <cassert>
#include <functional>

namespace cru::ui::render {
Size FlexLayoutRenderObject::OnMeasureContent(const Size& available_size) {
  std::vector<int> has_basis_children;
  std::vector<int> no_basis_children;
  std::vector<int> grow_children;
  std::vector<int> shrink_chilren;
  const auto child_count = GetChildCount();
  for (int i = 0; i < child_count; i++) {
    const auto& layout_data = *GetChildLayoutData(i);
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
    const float basis = GetChildLayoutData(i)->flex_basis.value();
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
      total_grow += GetChildLayoutData(i)->flex_grow;

    for (const int i : grow_children) {
      const float distributed_grow_length =
          remain_main_length * (GetChildLayoutData(i)->flex_grow / total_grow);
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
      total_shrink += GetChildLayoutData(i)->flex_shrink;

    for (const int i : shrink_chilren) {
      const float distributed_shrink_length =  // negative
          remain_main_length *
          (GetChildLayoutData(i)->flex_shrink / total_shrink);
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
  auto calculate_anchor = [](int alignment, float start_point,
                             float total_length,
                             float content_length) -> float {
    switch (alignment) {
      case internal::align_start:
        return start_point;
      case internal::align_center:
        return start_point + (total_length - content_length) / 2.0f;
      case internal::align_end:
        return start_point + total_length - content_length;
      default:
        return 0;
    }
  };

  const auto& children = GetChildren();
  if (direction_ == FlexDirection::Horizontal ||
      direction_ == FlexDirection::HorizontalReverse) {
    float actual_content_width = 0;
    for (const auto child : children) {
      actual_content_width += child->GetPreferredSize().width;
    }

    const float content_anchor_x =
        calculate_anchor(static_cast<int>(content_main_align_), 0,
                         content_rect.width, actual_content_width);

    float anchor_x = 0;
    for (int i = 0; i < static_cast<int>(children.size()); i++) {
      const auto child = children[i];
      const auto size = child->GetPreferredSize();

      float real_anchor_x = anchor_x + content_anchor_x;
      if (direction_ == FlexDirection::Horizontal)
        real_anchor_x = content_rect.left + real_anchor_x;
      else
        real_anchor_x = content_rect.GetRight() - real_anchor_x;
      child->Layout(Rect{
          real_anchor_x,
          calculate_anchor(
              static_cast<int>(GetChildLayoutData(i)->cross_alignment.value_or(
                  this->item_cross_align_)),
              content_rect.top, content_rect.height, size.height),
          size.width, size.height});

      anchor_x += size.width;
    }
  } else {
    float actual_content_height = 0;
    for (const auto child : children) {
      actual_content_height = child->GetPreferredSize().height;
    }

    const float content_anchor_y =
        calculate_anchor(static_cast<int>(content_main_align_), 0,
                         content_rect.height, actual_content_height);

    float anchor_y = 0;
    for (int i = 0; i < static_cast<int>(children.size()); i++) {
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
          calculate_anchor(
              static_cast<int>(GetChildLayoutData(i)->cross_alignment.value_or(
                  this->item_cross_align_)),
              content_rect.left, content_rect.width, size.width),
          size.width, size.height});

      anchor_y += size.height;
    }
  }
}
}  // namespace cru::ui::render
