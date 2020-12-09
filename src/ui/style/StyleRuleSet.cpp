#include "cru/ui/style/StyleRuleSet.hpp"
#include "cru/common/Event.hpp"
#include "cru/ui/controls/Control.hpp"
#include "gsl/gsl_assert"

#include <unordered_set>

namespace cru::ui::style {
StyleRuleSet::StyleRuleSet(StyleRuleSet* parent) { SetParent(parent); }

void StyleRuleSet::SetParent(StyleRuleSet* parent) {
  if (parent == parent_) return;
  parent_change_event_guard_.Reset();
  parent_ = parent;
  if (parent != nullptr) {
    parent_change_event_guard_.Reset(parent->ChangeEvent()->AddSpyOnlyHandler(
        [this] { this->RaiseChangeEvent(); }));
  }
  RaiseChangeEvent();
}

void StyleRuleSet::AddStyleRule(StyleRule rule, gsl::index index) {
  Expects(index >= 0 && index <= GetSize());

  rules_.insert(rules_.cbegin() + index, std::move(rule));

  RaiseChangeEvent();
}

void StyleRuleSet::RemoveStyleRule(gsl::index index, gsl::index count) {
  Expects(index >= 0);
  Expects(count >= 0 && index + count <= GetSize());

  rules_.erase(rules_.cbegin() + index, rules_.cbegin() + index + count);

  RaiseChangeEvent();
}

void StyleRuleSet::Set(const StyleRuleSet& other) {
  rules_ = other.rules_;

  RaiseChangeEvent();
}

StyleRuleSetBind::StyleRuleSetBind(controls::Control* control,
                                   StyleRuleSet* ruleset)
    : control_(control), ruleset_(ruleset) {
  Expects(control);
  Expects(ruleset);

  ruleset->ChangeEvent()->AddSpyOnlyHandler([this] {
    UpdateRuleSetChainCache();
    UpdateChangeListener();
    UpdateStyle();
  });
}

void StyleRuleSetBind::UpdateRuleSetChainCache() {
  ruleset_chain_cache_.clear();
  auto parent = ruleset_;
  while (parent != nullptr) {
    ruleset_chain_cache_.push_back(parent);
    parent = parent->GetParent();
  }
}

void StyleRuleSetBind::UpdateChangeListener() {
  guard_.Clear();

  std::unordered_set<IBaseEvent*> events;

  // ruleset order does not matter
  for (auto ruleset : ruleset_chain_cache_) {
    for (const auto& rule : ruleset->GetRules()) {
      auto e = rule.GetCondition()->ChangeOn(control_);
      events.insert(e.cbegin(), e.cend());
    }
  }

  for (auto e : events) {
    guard_ += e->AddSpyOnlyHandler([this] { this->UpdateStyle(); });
  }
}

void StyleRuleSetBind::UpdateStyle() {
  // cache is parent last, but when calculate style, parent first, so iterate
  // reverse.
  for (auto iter = ruleset_chain_cache_.crbegin();
       iter != ruleset_chain_cache_.crend(); ++iter) {
    for (const auto& rule : (*iter)->GetRules())
      if (rule.GetCondition()->Judge(control_)) {
        rule.GetStyler()->Apply(control_);
      }
  }
}
}  // namespace cru::ui::style
