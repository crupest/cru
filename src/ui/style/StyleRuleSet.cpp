#include "cru/ui/style/StyleRuleSet.h"
#include "cru/common/Event.h"
#include "cru/ui/controls/Control.h"
#include "gsl/gsl_assert"

#include <unordered_set>

namespace cru::ui::style {
namespace {
bool CheckCycle(StyleRuleSet* t, StyleRuleSet* p) {
  while (p != nullptr) {
    if (p == t) {
      return true;
    }
    p = p->GetParent().get();
  }

  return false;
}
}  // namespace

StyleRuleSet::StyleRuleSet(std::shared_ptr<StyleRuleSet> parent) {
  SetParent(std::move(parent));
}

void StyleRuleSet::SetParent(std::shared_ptr<StyleRuleSet> parent) {
  if (parent == parent_) return;
  if (CheckCycle(this, parent.get())) {
    throw Exception(u"Cycle detected in StyleRuleSet parent.");
  }
  parent_change_event_guard_.Reset();
  parent_ = std::move(parent);
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

void StyleRuleSet::SetStyleRule(Index index, StyleRule rule) {
  Expects(index >= 0 && index < GetSize());
  rules_[index] = std::move(rule);
  RaiseChangeEvent();
}

void StyleRuleSet::Set(const StyleRuleSet& other, bool set_parent) {
  rules_ = other.rules_;
  if (set_parent) parent_ = other.parent_;

  RaiseChangeEvent();
}

StyleRuleSetBind::StyleRuleSetBind(controls::Control* control,
                                   std::shared_ptr<StyleRuleSet> ruleset)
    : control_(control), ruleset_(std::move(ruleset)) {
  Expects(control);
  Expects(ruleset_);

  ruleset_->ChangeEvent()->AddSpyOnlyHandler([this] {
    UpdateRuleSetChainCache();
    UpdateChangeListener();
    UpdateStyle();
  });
}

void StyleRuleSetBind::UpdateRuleSetChainCache() {
  ruleset_chain_cache_.clear();
  auto parent = ruleset_;
  while (parent != nullptr) {
    ruleset_chain_cache_.push_back(parent.get());
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
    for (const auto& rule : (*iter)->GetRules()) {
      rule.CheckAndApply(control_);
    }
  }
}
}  // namespace cru::ui::style
