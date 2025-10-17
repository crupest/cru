#include "cru/ui/render/RenderObject.h"

#include "cru/base/Exception.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/host/WindowHost.h"

namespace cru::ui::render {
const BoxConstraint BoxConstraint::kNotLimit{Size::kMax, Size::kZero};

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

void RenderObject::SetMinSize1(const Size& min_size) {
  min_size_ = min_size;
  InvalidateLayout();
}

void RenderObject::SetMaxSize1(const Size& max_size) {
  max_size_ = max_size;
  InvalidateLayout();
}

BoxConstraint RenderObject::CalculateMergedConstraint(
    const BoxConstraint& constraint) const {
  auto result = constraint;
  if (max_size_.width >= constraint.min.width &&
      max_size_.width < constraint.max.width) {
    result.max.width = max_size_.width;
  }

  if (max_size_.height >= constraint.min.height &&
      max_size_.height < constraint.max.height) {
    result.max.height = max_size_.height;
  }

  if (min_size_.width <= constraint.max.width &&
      min_size_.width > constraint.min.width) {
    result.min.width = min_size_.width;
  }

  if (min_size_.height <= constraint.max.height &&
      min_size_.height > constraint.min.height) {
    result.min.height = min_size_.height;
  }

  return result;
}

void RenderObject::Measure(const MeasureRequirement& requirement,
                           const MeasureSize& preferred_size) {
  MeasureRequirement merged_requirement =
      MeasureRequirement::Merge(requirement, custom_measure_requirement_)
          .Normalize();
  MeasureSize merged_preferred_size =
      preferred_size.OverrideBy(preferred_size_);

  if constexpr (cru::ui::debug_flags::layout) {
    CRU_LOG_TAG_DEBUG("{} Measure begins :\nrequirement: {}\npreferred size: {}",
                  this->GetDebugPathInTree().ToUtf8(), requirement.ToDebugString().ToUtf8(),
                  preferred_size.ToDebugString().ToUtf8());
  }

  desired_size_ = OnMeasureCore(merged_requirement, merged_preferred_size);

  if constexpr (cru::ui::debug_flags::layout) {
    CRU_LOG_TAG_DEBUG("{} Measure ends :\nresult size: {}",
                  this->GetDebugPathInTree().ToUtf8(), desired_size_);
  }

  Ensures(desired_size_.width >= 0);
  Ensures(desired_size_.height >= 0);
}

Size RenderObject::Measure1(const BoxConstraint& constraint) {
  Expects(constraint.Validate());
  desired_size_ = OnMeasureCore1(constraint);
  Ensures(constraint.Satisfy(desired_size_));
  return desired_size_;
}

void RenderObject::Layout(const Point& offset) {
  if constexpr (cru::ui::debug_flags::layout) {
    CRU_LOG_TAG_DEBUG("{} Layout :\noffset: {} size: {}",
                  this->GetDebugPathInTree().ToUtf8(), offset, desired_size_);
  }
  offset_ = offset;
  size_ = desired_size_;

  OnResize(size_);

  OnLayoutCore();
}

Thickness RenderObject::GetTotalSpaceThickness() const {
  return margin_ + padding_;
}

Thickness RenderObject::GetInnerSpaceThickness() const { return padding_; }

Size RenderObject::OnMeasureCore(const MeasureRequirement& requirement,
                                 const MeasureSize& preferred_size) {
  const Thickness outer_space = GetTotalSpaceThickness();
  const Size space_size{outer_space.GetHorizontalTotal(),
                        outer_space.GetVerticalTotal()};

  MeasureRequirement content_requirement = requirement;

  content_requirement.max = content_requirement.max.Minus(space_size);
  content_requirement.min = content_requirement.min.Minus(space_size);

  auto inner_space = GetInnerSpaceThickness();
  MeasureSize content_preferred_size =
      content_requirement.Coerce(preferred_size.Minus(
          {inner_space.GetHorizontalTotal(), inner_space.GetVerticalTotal()}));

  const auto content_size =
      OnMeasureContent(content_requirement, content_preferred_size);

  return space_size + content_size;
}

Size RenderObject::OnMeasureCore1(const BoxConstraint& constraint) {
  auto merged_constraint = CalculateMergedConstraint(constraint);

  const Thickness outer_space = GetTotalSpaceThickness();
  Size space_size{outer_space.GetHorizontalTotal(),
                  outer_space.GetVerticalTotal()};

  if (space_size.width > merged_constraint.max.width) {
    space_size.width = merged_constraint.max.width;
    CRU_LOG_TAG_WARN("{} space width is over constraint.max.width",
                 this->GetDebugPathInTree().ToUtf8());
  }

  if (space_size.height > merged_constraint.max.height) {
    space_size.height = merged_constraint.max.height;
    CRU_LOG_TAG_WARN("{} space height is over constraint.max.height",
                 this->GetDebugPathInTree().ToUtf8());
  }

  BoxConstraint content_constraint{merged_constraint.max - space_size,
                                   merged_constraint.min - space_size};

  const auto content_size = OnMeasureContent1(content_constraint);

  return space_size + content_size;
}

void RenderObject::OnLayoutCore() {
  Size total_size = GetDesiredSize();
  const Thickness outer_space = GetTotalSpaceThickness();
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

Size RenderObject::OnMeasureContent1(const BoxConstraint& constraint) {
  throw Exception("Not implemented.");
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
