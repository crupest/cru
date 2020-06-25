#include "cru/ui/render/FlexLayoutRenderObject.hpp"

#include "cru/platform/graph/util/Painter.hpp"

#include <algorithm>
#include <functional>

namespace cru::ui::render {
Size FlexLayoutRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  const bool horizontal = (direction_ == FlexDirection::Horizontal ||
                           direction_ == FlexDirection::HorizontalReverse);

  const auto main_max_length =
      horizontal ? requirement.max_width : requirement.max_height;
  const auto cross_max_length =
      horizontal ? requirement.max_height : requirement.max_width;

  std::function<float(const Size&)> get_main_length;
  std::function<float(const Size&)> get_cross_length;
  std::function<void(Size&, const Size&)> calculate_result_size;
  std::function<MeasureRequirement(MeasureLength main, MeasureLength cross)>
      create_requirement;

  if (horizontal) {
    get_main_length = [](const Size& size) { return size.width; };
    get_cross_length = [](const Size& size) { return size.height; };
    calculate_result_size = [](Size& result, const Size& child_size) {
      result.width += child_size.width;
      result.height = std::max(result.height, child_size.height);
    };
    create_requirement = [](MeasureLength main, MeasureLength cross) {
      return MeasureRequirement{main, cross};
    };
  } else {
    get_main_length = [](const Size& size) { return size.height; };
    get_cross_length = [](const Size& size) { return size.width; };
    calculate_result_size = [](Size& result, const Size& child_size) {
      result.height += child_size.height;
      result.width = std::max(result.width, child_size.width);
    };
    create_requirement = [](MeasureLength main, MeasureLength cross) {
      return MeasureRequirement{cross, main};
    };
  }

  const auto& children = GetChildren();
  Index children_count = children.size();

  if (!main_max_length.IsNotSpecify()) {
    float remain_main_length = main_max_length.GetLength();

    for (Index i = 0; i < children_count; i++) {
      const auto child = children[i];
      child->Measure(create_requirement(remain_main_length, cross_max_length));
      const auto measure_result = child->GetMeasuredSize();
      remain_main_length -= get_main_length(measure_result);
    }

    if (remain_main_length > 0) {
      std::vector<Index> expand_children;
      float total_expand_factor = 0;

      for (Index i = 0; i < children_count; i++) {
        const auto factor = GetChildLayoutData(i)->expand_factor;
        if (factor > 0) {
          expand_children.push_back(i);
          total_expand_factor += factor;
        }
      }

      for (const Index i : expand_children) {
        const float distributed_grow_length =
            remain_main_length *
            (GetChildLayoutData(i)->expand_factor / total_expand_factor);
        const auto child = children[i];
        const float new_main_length =
            get_main_length(child->GetMeasuredSize()) + distributed_grow_length;
        child->Measure(create_requirement(new_main_length, cross_max_length));
      }
    } else if (remain_main_length < 0) {
      std::vector<Index> shrink_children;
      float total_shrink_factor = 0;

      for (Index i = 0; i < children_count; i++) {
        const auto factor = GetChildLayoutData(i)->shrink_factor;
        if (factor > 0) {
          shrink_children.push_back(i);
          total_shrink_factor += factor;
        }
      }

      for (const Index i : shrink_children) {
        const float distributed_shrink_length =  // negative
            remain_main_length *
            (GetChildLayoutData(i)->shrink_factor / total_shrink_factor);
        const auto child = children[i];
        float new_main_length = get_main_length(child->GetMeasuredSize()) +
                                distributed_shrink_length;
        new_main_length = new_main_length > 0 ? new_main_length : 0;
        child->Measure(create_requirement(new_main_length, cross_max_length));
      }
    }
  } else {
    for (Index i = 0; i < children_count; i++) {
      const auto child = children[i];
      child->Measure(requirement);
    }
  }

  Size result;
  for (auto child : children) {
    calculate_result_size(result, child->GetMeasuredSize());
  }

  return result;
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
        return start_point;
    }
  };

  const auto& children = GetChildren();
  if (direction_ == FlexDirection::Horizontal ||
      direction_ == FlexDirection::HorizontalReverse) {
    float actual_content_width = 0;
    for (const auto child : children) {
      actual_content_width += child->GetMeasuredSize().width;
    }

    const float content_anchor_x =
        calculate_anchor(static_cast<int>(content_main_align_), 0,
                         content_rect.width, actual_content_width);

    float anchor_x = 0;
    for (int i = 0; i < static_cast<int>(children.size()); i++) {
      const auto child = children[i];
      const auto size = child->GetMeasuredSize();

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
      actual_content_height = child->GetMeasuredSize().height;
    }

    const float content_anchor_y =
        calculate_anchor(static_cast<int>(content_main_align_), 0,
                         content_rect.height, actual_content_height);

    float anchor_y = 0;
    for (int i = 0; i < static_cast<int>(children.size()); i++) {
      const auto child = children[i];
      const auto size = child->GetMeasuredSize();

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
