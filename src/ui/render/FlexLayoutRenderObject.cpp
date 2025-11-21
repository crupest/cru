#include "cru/ui/render/FlexLayoutRenderObject.h"

#include "cru/base/log/Logger.h"
#include "cru/ui/render/LayoutHelper.h"

#include <algorithm>
#include <type_traits>

namespace cru::ui::render {
FlexLayoutRenderObject::FlexLayoutRenderObject()
    : LayoutRenderObject<FlexChildLayoutData>(kRenderObjectName) {}

struct tag_horizontal_t {};
struct tag_vertical_t {};

template <typename TSize>
constexpr auto GetMain(const TSize& size, tag_horizontal_t) {
  return size.width;
}

template <typename TSize>
constexpr auto GetCross(const TSize& size, tag_horizontal_t) {
  return size.height;
}

template <typename TSize>
constexpr auto GetMain(const TSize& size, tag_vertical_t) {
  return size.height;
}

template <typename TSize>
constexpr auto GetCross(const TSize& size, tag_vertical_t) {
  return size.width;
}

template <typename TSize>
constexpr auto& GetMain(TSize& size, tag_horizontal_t) {
  return size.width;
}

template <typename TSize>
constexpr auto& GetCross(TSize& size, tag_horizontal_t) {
  return size.height;
}

template <typename TSize>
constexpr auto& GetMain(TSize& size, tag_vertical_t) {
  return size.height;
}

template <typename TSize>
constexpr auto& GetCross(TSize& size, tag_vertical_t) {
  return size.width;
}

template <typename TSize>
constexpr TSize CreateTSize(decltype(std::declval<TSize>().width) main,
                            decltype(std::declval<TSize>().height) cross,
                            tag_horizontal_t) {
  return TSize{main, cross};
}

template <typename TSize>
constexpr TSize CreateTSize(decltype(std::declval<TSize>().width) main,
                            decltype(std::declval<TSize>().height) cross,
                            tag_vertical_t) {
  return TSize{cross, main};
}

enum class FlexLayoutAdjustType { None, Expand, Shrink };

namespace {
void Remove(std::vector<Index>& v, const std::vector<Index>& to_remove_v) {
  Index current = 0;
  for (auto to_remove : to_remove_v) {
    while (v[current] != to_remove) {
      current++;
    }
    v.erase(v.cbegin() + current);
  }
}

template <typename direction_tag_t,
          typename = std::enable_if_t<
              std::is_same_v<direction_tag_t, tag_horizontal_t> ||
              std::is_same_v<direction_tag_t, tag_vertical_t>>>
Size FlexLayoutMeasureContentImpl(
    const MeasureRequirement& requirement, const MeasureSize& preferred_size,
    const std::vector<RenderObject*>& children,
    const std::vector<FlexChildLayoutData>& layout_data,
    Alignment item_cross_align, std::string kLogTag) {
  Expects(children.size() == layout_data.size());

  direction_tag_t direction_tag;

  const Index child_count = children.size();

  MeasureLength preferred_main_length = GetMain(preferred_size, direction_tag);
  MeasureLength preferred_cross_length =
      GetCross(preferred_size, direction_tag);
  MeasureLength max_main_length = GetMain(requirement.max, direction_tag);
  MeasureLength max_cross_length = GetCross(requirement.max, direction_tag);
  MeasureLength min_main_length = GetMain(requirement.min, direction_tag);
  MeasureLength min_cross_length = GetCross(requirement.min, direction_tag);

  // step 1.
  for (Index i = 0; i < child_count; i++) {
    const auto child = children[i];
    child->Measure(MeasureRequirement{CreateTSize<MeasureSize>(
                                          MeasureLength::NotSpecified(),
                                          max_cross_length, direction_tag),
                                      MeasureSize::NotSpecified()},
                   MeasureSize::NotSpecified());
  }

  float total_length = 0.f;
  for (auto child : children) {
    total_length += GetMain(child->GetDesiredSize(), direction_tag);
  }

  // step 2.
  FlexLayoutAdjustType adjust_type = FlexLayoutAdjustType::None;
  float target_length =
      -1.f;  // Use a strange value to indicate error. This value should be
             // assigned before usage. Or program has a bug.

  if (preferred_main_length.IsSpecified()) {
    const float preferred_main_length_value =
        preferred_main_length.GetLengthOrUndefined();
    target_length = preferred_main_length_value;
    if (total_length > preferred_main_length_value) {
      adjust_type = FlexLayoutAdjustType::Shrink;
    } else if (total_length < preferred_main_length_value) {
      adjust_type = FlexLayoutAdjustType::Expand;
    }
  } else {
    if (max_main_length.IsSpecified()) {
      const float max_main_length_value =
          max_main_length.GetLengthOrUndefined();
      if (max_main_length_value < total_length) {
        adjust_type = FlexLayoutAdjustType::Shrink;
        target_length = max_main_length_value;
      } else if (max_main_length_value > total_length) {
        for (auto data : layout_data) {
          if (data.expand_factor > 0) {
            adjust_type = FlexLayoutAdjustType::Expand;
            target_length = max_main_length_value;
            break;
          }
        }
      }
    }
    // Do not use else here.
    if (adjust_type == FlexLayoutAdjustType::None &&
        min_main_length.IsSpecified() &&
        min_main_length.GetLengthOrUndefined() > total_length) {
      adjust_type = FlexLayoutAdjustType::Expand;
      target_length = min_main_length.GetLengthOrUndefined();
    }
  }

  // step 3.
  if (adjust_type == FlexLayoutAdjustType::Shrink) {
    std::vector<Index> shrink_list;

    for (Index i = 0; i < child_count; i++) {
      if (layout_data[i].shrink_factor > 0) {
        shrink_list.push_back(i);
      }
    }

    while (!shrink_list.empty()) {
      const float total_shrink_length = total_length - target_length;

      float total_shrink_factor = 0.f;
      std::vector<Index> to_remove;

      for (Index i : shrink_list) {
        total_shrink_factor += layout_data[i].shrink_factor;
      }

      if (total_shrink_factor == 0.0f) {
        break;
      }

      for (Index i : shrink_list) {
        const auto child = children[i];
        const float shrink_length = layout_data[i].shrink_factor /
                                    total_shrink_factor * total_shrink_length;
        float new_measure_length =
            GetMain(child->GetDesiredSize(), direction_tag) - shrink_length;

        MeasureLength child_min_main_length =
            GetMain(child->GetMinSize(), direction_tag);

        if (child_min_main_length.IsSpecified() &&
            new_measure_length < child_min_main_length.GetLengthOrUndefined()) {
          new_measure_length = child_min_main_length.GetLengthOrUndefined();
        } else if (new_measure_length < 0.f) {
          new_measure_length = 0.f;
        }

        child->Measure(MeasureRequirement{CreateTSize<MeasureSize>(
                                              new_measure_length,
                                              max_cross_length, direction_tag),
                                          MeasureSize::NotSpecified()},
                       CreateTSize<MeasureSize>(new_measure_length,
                                                MeasureLength::NotSpecified(),
                                                direction_tag));

        const Size new_size = child->GetDesiredSize();
        const float new_main_length = GetMain(new_size, direction_tag);
        if (new_main_length >= new_measure_length) {
          to_remove.push_back(i);
        }
      }

      total_length = 0.f;
      for (auto child : children) {
        total_length += GetMain(child->GetDesiredSize(), direction_tag);
      }

      if (total_length <= target_length) break;

      Remove(shrink_list, to_remove);
    }
  } else if (adjust_type == FlexLayoutAdjustType::Expand) {
    std::vector<Index> expand_list;

    for (Index i = 0; i < child_count; i++) {
      if (layout_data[i].expand_factor > 0) {
        expand_list.push_back(i);
      }
    }

    while (!expand_list.empty()) {
      const float total_expand_length = target_length - total_length;

      float total_expand_factor = 0.f;
      std::vector<Index> to_remove;

      for (Index i : expand_list) {
        total_expand_factor += layout_data[i].expand_factor;
      }

      for (Index i : expand_list) {
        const auto child = children[i];
        const float expand_length = layout_data[i].expand_factor /
                                    total_expand_factor * total_expand_length;
        float new_measure_length =
            GetMain(child->GetDesiredSize(), direction_tag) + expand_length;

        MeasureLength child_max_main_length =
            GetMain(child->GetMaxSize(), direction_tag);

        if (child_max_main_length.IsSpecified() &&
            new_measure_length > child_max_main_length.GetLengthOrUndefined()) {
          new_measure_length = child_max_main_length.GetLengthOrUndefined();
        }

        child->Measure(
            MeasureRequirement{
                CreateTSize<MeasureSize>(MeasureLength::NotSpecified(),
                                         max_cross_length, direction_tag),
                CreateTSize<MeasureSize>(new_measure_length,
                                         MeasureLength::NotSpecified(),
                                         direction_tag)},
            CreateTSize<MeasureSize>(new_measure_length,
                                     MeasureLength::NotSpecified(),
                                     direction_tag));

        const Size new_size = child->GetDesiredSize();
        const float new_main_length = GetMain(new_size, direction_tag);
        if (new_main_length <= new_measure_length) {
          to_remove.push_back(i);
        }
      }

      total_length = 0.f;
      for (auto child : children) {
        total_length += GetMain(child->GetDesiredSize(), direction_tag);
      }

      if (total_length >= target_length) break;

      Remove(expand_list, to_remove);
    }
  }

  float child_max_cross_length = 0.f;

  for (auto child : children) {
    const float cross_length = GetCross(child->GetDesiredSize(), direction_tag);
    if (cross_length > child_max_cross_length) {
      child_max_cross_length = cross_length;
    }
  }

  if (max_main_length.IsSpecified() &&
      total_length > max_main_length.GetLengthOrUndefined()) {
    CRU_LOG_TAG_WARN(
        "(Measure) Children's main axis length exceeds required max length.");
    total_length = max_main_length.GetLengthOrUndefined();
  } else if (min_main_length.IsSpecified() &&
             total_length < min_main_length.GetLengthOrUndefined()) {
    total_length = min_main_length.GetLengthOrUndefined();
  }

  child_max_cross_length =
      std::max(preferred_cross_length.GetLengthOr0(), child_max_cross_length);
  child_max_cross_length =
      std::max(min_cross_length.GetLengthOr0(), child_max_cross_length);
  child_max_cross_length =
      std::min(max_cross_length.GetLengthOrMax(), child_max_cross_length);

  for (Index i = 0; i < child_count; i++) {
    auto child_layout_data = layout_data[i];
    auto child = children[i];
    if (child_layout_data.cross_alignment.value_or(item_cross_align) ==
        Alignment::Stretch) {
      auto size = child->GetDesiredSize();
      GetCross(size, direction_tag) = child_max_cross_length;
      child->Measure({size, size}, MeasureSize::NotSpecified());
    }
  }

  return CreateTSize<Size>(total_length, child_max_cross_length, direction_tag);
}
}  // namespace

Size FlexLayoutRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement, const MeasureSize& preferred_size) {
  const bool horizontal = (direction_ == FlexDirection::Horizontal ||
                           direction_ == FlexDirection::HorizontalReverse);
  std::vector<RenderObject*> children;
  std::vector<FlexChildLayoutData> layout_data_list;
  for (int i = 0; i < GetChildCount(); i++) {
    children.push_back(GetChildAt(i));
    layout_data_list.push_back(GetChildLayoutDataAt(i));
  }

  if (horizontal) {
    return FlexLayoutMeasureContentImpl<tag_horizontal_t>(
        requirement, preferred_size, children, layout_data_list,
        item_cross_align_, kLogTag);
  } else {
    return FlexLayoutMeasureContentImpl<tag_vertical_t>(
        requirement, preferred_size, children, layout_data_list,
        item_cross_align_, kLogTag);
  }
}

void FlexLayoutRenderObject::OnLayoutContent(const Rect& content_rect) {
  const auto child_count = GetChildCount();
  std::vector<RenderObject*> children;
  std::vector<FlexChildLayoutData> layout_data_list;
  for (int i = 0; i < child_count; i++) {
    children.push_back(GetChildAt(i));
    layout_data_list.push_back(GetChildLayoutDataAt(i));
  }

  if (direction_ == FlexDirection::Horizontal) {
    float current_main_offset = 0;
    for (Index i = 0; i < child_count; i++) {
      const auto child = children[i];
      const auto size = child->GetDesiredSize();
      const auto cross_align =
          layout_data_list[i].cross_alignment.value_or(GetItemCrossAlign());
      child->Layout(
          Point{content_rect.left + current_main_offset,
                CalculateAnchorByAlignment(cross_align, content_rect.top,
                                           content_rect.height, size.height)});
      current_main_offset += size.width;
    }

  } else if (direction_ == FlexDirection::HorizontalReverse) {
    float current_main_offset = 0;
    for (Index i = 0; i < child_count; i++) {
      const auto child = children[i];
      const auto size = child->GetDesiredSize();
      const auto cross_align =
          layout_data_list[i].cross_alignment.value_or(GetItemCrossAlign());
      child->Layout(
          Point{content_rect.GetRight() - current_main_offset,
                CalculateAnchorByAlignment(cross_align, content_rect.top,
                                           content_rect.height, size.height)});
      current_main_offset += size.width;
    }
  } else if (direction_ == FlexDirection::Vertical) {
    float current_main_offset = 0;
    for (Index i = 0; i < child_count; i++) {
      const auto child = children[i];
      const auto size = child->GetDesiredSize();
      const auto cross_align =
          layout_data_list[i].cross_alignment.value_or(GetItemCrossAlign());
      child->Layout(Point{
          CalculateAnchorByAlignment(cross_align, content_rect.left,
                                     content_rect.width, size.width),
          content_rect.top + current_main_offset,
      });
      current_main_offset += size.height;
    }
  } else {
    float current_main_offset = 0;
    for (Index i = 0; i < child_count; i++) {
      const auto child = children[i];
      const auto size = child->GetDesiredSize();
      const auto cross_align =
          layout_data_list[i].cross_alignment.value_or(GetItemCrossAlign());
      child->Layout(
          Point{CalculateAnchorByAlignment(cross_align, content_rect.left,
                                           content_rect.width, size.width),
                content_rect.GetBottom() - current_main_offset});
      current_main_offset += size.height;
    }
  }
}
}  // namespace cru::ui::render
