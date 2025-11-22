#include "cru/ui/render/RenderObject.h"

#include "cru/base/Base.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/ControlHost.h"

namespace cru::ui::render {
const BoxConstraint BoxConstraint::kNotLimit{Size::kMax, Size::kZero};

RenderObject::RenderObject(std::string name)
    : name_(std::move(name)), control_(nullptr), parent_(nullptr) {}

RenderObject::~RenderObject() { DestroyEvent_.Raise(this); }

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

Point RenderObject::GetTotalOffset() {
  Point result{};
  RenderObject* render_object = this;

  while (render_object != nullptr) {
    const auto o = render_object->GetOffset();
    result.x += o.x;
    result.y += o.y;
    render_object = render_object->GetParent();
  }

  return result;
}

Point RenderObject::FromRootToContent(const Point& point) {
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

void RenderObject::SetSuggestSize(const MeasureSize& suggest_size) {
  custom_measure_requirement_.suggest = suggest_size;
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

void RenderObject::Measure(const MeasureRequirement& requirement) {
  CRU_LOG_TAG_DEBUG("{} Measure begins, requirement {}.",
                    this->GetDebugPathInTree(), requirement);

  measure_result_size_ = OnMeasureCore(requirement);
  if (measure_result_size_.width < 0 || measure_result_size_.height < 0) {
    throw Exception("Measure result size is invalid.");
  }

  CRU_LOG_TAG_DEBUG("{} Measure ends, result size: {}.",
                    this->GetDebugPathInTree(), measure_result_size_);
}

void RenderObject::Layout(const Point& offset) {
  CRU_LOG_TAG_DEBUG("{} Layout begins, offset: {}, size: {}.",
                    this->GetDebugPathInTree(), offset, measure_result_size_);

  offset_ = offset;
  size_ = measure_result_size_;

  OnResize(size_);

  OnLayoutCore();

  CRU_LOG_TAG_DEBUG("{} Layout ends.", this->GetDebugPathInTree());
}

Thickness RenderObject::GetTotalSpaceThickness() { return margin_ + padding_; }

Thickness RenderObject::GetInnerSpaceThickness() { return padding_; }

Size RenderObject::OnMeasureCore(const MeasureRequirement& requirement) {
  auto space_size = GetTotalSpaceThickness().GetTotalSize();

  auto content_requirement =
      requirement.Merge(custom_measure_requirement_).Minus(space_size);

  auto content_size = OnMeasureContent(content_requirement);

  return space_size + content_size;
}

void RenderObject::OnLayoutCore() {
  auto total_size = GetMeasureResultSize();
  auto outer_space = GetTotalSpaceThickness();
  auto content_size = (total_size - outer_space.GetTotalSize()).AtLeast0();

  Point lefttop{outer_space.left, outer_space.top};
  lefttop.x = std::min(lefttop.x, total_size.width);
  lefttop.y = std::min(lefttop.y, total_size.height);

  const Rect content_rect{lefttop, content_size};

  OnLayoutContent(content_rect);
}

Rect RenderObject::GetPaddingRect() {
  const auto size = GetMeasureResultSize();
  Rect rect{Point{}, size};
  rect = rect.Shrink(GetMargin());
  rect.left = std::min(rect.left, size.width);
  rect.top = std::min(rect.top, size.height);
  rect.width = std::max(rect.width, 0.0f);
  rect.height = std::max(rect.height, 0.0f);
  return rect;
}

Rect RenderObject::GetContentRect() {
  const auto size = GetMeasureResultSize();
  Rect rect{Point{}, size};
  rect = rect.Shrink(GetMargin());
  rect = rect.Shrink(GetPadding());
  rect.left = std::min(rect.left, size.width);
  rect.top = std::min(rect.top, size.height);
  rect.width = std::max(rect.width, 0.0f);
  rect.height = std::max(rect.height, 0.0f);
  return rect;
}

controls::ControlHost* RenderObject::GetControlHost() {
  if (control_) {
    return control_->GetControlHost();
  }
  return nullptr;
}

void RenderObject::InvalidateLayout() {
  if (auto host = GetControlHost()) {
    host->InvalidateLayout();
  }
}

void RenderObject::InvalidatePaint() {
  if (auto window = GetControlHost()) {
    window->InvalidatePaint();
  }
}

std::string RenderObject::GetName() { return name_; }

std::string RenderObject::GetDebugPathInTree() {
  std::vector<std::string> chain;
  RenderObject* parent = this;
  while (parent != nullptr) {
    chain.push_back(parent->GetName());
    parent = parent->GetParent();
  }

  std::string result(chain.back());
  for (auto iter = chain.crbegin() + 1; iter != chain.crend(); ++iter) {
    result += " -> ";
    result += *iter;
  }

  return result;
}
}  // namespace cru::ui::render
