#include "cru/ui/style/StyleRuleSet.hpp"
#include "cru/common/Event.hpp"
#include "gsl/gsl_assert"

#include <unordered_set>

namespace cru::ui::style {
void StyleRuleSet::AddStyleRule(StyleRule rule, gsl::index index) {
  Expects(index >= 0 && index <= GetSize());

  rules_.insert(rules_.cbegin() + index, std::move(rule));

  UpdateChangeListener();
  UpdateStyle();
}

void StyleRuleSet::RemoveStyleRule(gsl::index index, gsl::index count) {
  Expects(index >= 0);
  Expects(count >= 0 && index + count <= GetSize());

  rules_.erase(rules_.cbegin() + index, rules_.cbegin() + index + count);

  UpdateChangeListener();
  UpdateStyle();
}

void StyleRuleSet::Set(const StyleRuleSet& other) {
  rules_ = other.rules_;
  UpdateChangeListener();
  UpdateStyle();
}

void StyleRuleSet::UpdateChangeListener() {
  if (control_ == nullptr) return;

  guard_.Clear();

  std::unordered_set<IBaseEvent*> events;
  for (const auto& rule : rules_) {
    auto e = rule.GetCondition()->ChangeOn(control_);
    events.insert(e.cbegin(), e.cend());
  }

  for (auto e : events) {
    guard_ += e->AddSpyOnlyHandler([this] { this->UpdateStyle(); });
  }
}

void StyleRuleSet::UpdateStyle() {
  if (control_ == nullptr) return;

  for (const auto& rule : rules_) {
    if (rule.GetCondition()->Judge(control_)) {
      rule.GetStyler()->Apply(control_);
    }
  }
}
}  // namespace cru::ui::style
