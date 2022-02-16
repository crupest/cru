#include "cru/ui/render/RenderObject.h"

#include "cru/common/Logger.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/host/WindowHost.h"

namespace cru::ui::render {

void RenderObject::SetParent(RenderObject* new_parent) {
#ifdef CRU_DEBUG
  // In case there is a cycle.
  auto parent = new_parent;
  while (parent) {
    assert(parent != this);
    parent = parent->GetParent();
  }
#endif
  parent_ = new_parent;
}

void RenderObject::SetAttachedControl(controls::Control* new_control) {
  auto old_control = control_;
  control_ = new_control;
  OnAttachedControlChanged(old_control, new_control);
}

Point RenderObject::GetTotalOffset() const {
  Point result{};
  const RenderObject* render_object = this;

  while (render_object != nullptr) {
    const auto o = render_object->GetOffset();
    result.x += o.x;
    result.y += o.y;
    render_object = render_object->GetParent();
  }

  return result;
}

Point RenderObject::FromRootToContent(const Point& point) const {
  const auto offset = GetTotalOffset();
  const auto rect = GetContentRect();
  return Point{point.x - (offset.x + rect.left),
               point.y - (offset.y + rect.top)};
}

void RenderObject::SetMargin(const Thickness& margin) {
  margin_ = margin;
  InvalidateLayout();
}

void RenderObject::SetPadding(const Thickness& padding) {
  padding_ = padding;
  InvalidateLayout();
}

void RenderObject::SetPreferredSize(const MeasureSize& preferred_size) {
  preferred_size_ = preferred_size;
  InvalidateLayout();
}

void RenderObject::SetMinSize(const MeasureSize& min_size) {
  custom_measure_requirement_.min = min_size;
  InvalidateLayout();
}

void RenderObject::SetMaxSize(const MeasureSize& max_size) {
  custom_measure_requirement_.max = max_size;
  InvalidateLayout();
}

void RenderObject::Measure(const MeasureRequirement& requirement,
                           const MeasureSize& preferred_size) {
  MeasureRequirement merged_requirement =
      MeasureRequirement::Merge(requirement, custom_measure_requirement_)
          .Normalize();
  MeasureSize merged_preferred_size =
      preferred_size.OverrideBy(preferred_size_);

  if constexpr (cru::ui::debug_flags::layout) {
    log::Debug(u"{} Measure begins :\nrequirement: {}\npreferred size: {}",
               this->GetDebugPathInTree(), requirement.ToDebugString(),
               preferred_size.ToDebugString());
  }

  desired_size_ = OnMeasureCore(merged_requirement, merged_preferred_size);

  if constexpr (cru::ui::debug_flags::layout) {
    log::Debug(u"{} Measure ends :\nresult size: {}",
               this->GetDebugPathInTree(), desired_size_);
  }

  Ensures(desired_size_.width >= 0);
  Ensures(desired_size_.height >= 0);
}

void RenderObject::Layout(const Point& offset) {
  if constexpr (cru::ui::debug_flags::layout) {
    log::Debug(u"{} Layout :\noffset: {} size: {}", this->GetDebugPathInTree(),
               offset, desired_size_);
  }
  offset_ = offset;
  size_ = desired_size_;

  OnResize(size_);

  OnLayoutCore();
}

Thickness RenderObject::GetOuterSpaceThickness() const {
  return margin_ + padding_;
}

Size RenderObject::OnMeasureCore(const MeasureRequirement& requirement,
                                 const MeasureSize& preferred_size) {
  const Thickness outer_space = GetOuterSpaceThickness();
  const Size space_size{outer_space.GetHorizontalTotal(),
                        outer_space.GetVerticalTotal()};

  MeasureRequirement content_requirement = requirement;

  content_requirement.max = content_requirement.max.Minus(space_size);
  content_requirement.min = content_requirement.min.Minus(space_size);

  MeasureSize content_preferred_size =
      content_requirement.Coerce(preferred_size.Minus(space_size));

  const auto content_size =
      OnMeasureContent(content_requirement, content_preferred_size);

  return space_size + content_size;
}

void RenderObject::OnLayoutCore() {
  Size total_size = GetDesiredSize();
  const Thickness outer_space = GetOuterSpaceThickness();
  const Size space_size{outer_space.GetHorizontalTotal(),
                        outer_space.GetVerticalTotal()};

  auto content_size = total_size - space_size;

  if (content_size.width < 0) {
    content_size.width = 0;
  }
  if (content_size.height < 0) {
    content_size.height = 0;
  }

  Point lefttop{outer_space.left, outer_space.top};
  if (lefttop.x > total_size.width) {
    lefttop.x = total_size.width;
  }
  if (lefttop.y > total_size.height) {
    lefttop.y = total_size.height;
  }

  const Rect content_rect{lefttop, content_size};

  OnLayoutContent(content_rect);
}

Rect RenderObject::GetPaddingRect() const {
  const auto size = GetDesiredSize();
  Rect rect{Point{}, size};
  rect = rect.Shrink(GetMargin());
  rect.left = std::min(rect.left, size.width);
  rect.top = std::min(rect.top, size.height);
  rect.width = std::max(rect.width, 0.0f);
  rect.height = std::max(rect.height, 0.0f);
  return rect;
}

Rect RenderObject::GetContentRect() const {
  const auto size = GetDesiredSize();
  Rect rect{Point{}, size};
  rect = rect.Shrink(GetMargin());
  rect = rect.Shrink(GetPadding());
  rect.left = std::min(rect.left, size.width);
  rect.top = std::min(rect.top, size.height);
  rect.width = std::max(rect.width, 0.0f);
  rect.height = std::max(rect.height, 0.0f);
  return rect;
}

host::WindowHost* RenderObject::GetWindowHost() {
  if (control_) {
    return control_->GetWindowHost();
  }
  return nullptr;
}

void RenderObject::InvalidateLayout() {
  if (auto window_host = GetWindowHost()) {
    window_host->InvalidateLayout();
  }
}

void RenderObject::InvalidatePaint() {
  if (auto window_host = GetWindowHost()) {
    window_host->InvalidatePaint();
  }
}

String kUnamedName(u"UNNAMED");
String RenderObject::GetName() const { return kUnamedName; }

String RenderObject::GetDebugPathInTree() const {
  std::vector<String> chain;
  const RenderObject* parent = this;
  while (parent != nullptr) {
    chain.push_back(parent->GetName());
    parent = parent->GetParent();
  }

  String result(chain.back());
  for (auto iter = chain.crbegin() + 1; iter != chain.crend(); ++iter) {
    result += u" -> ";
    result += *iter;
  }

  return result;
}
}  // namespace cru::ui::render
