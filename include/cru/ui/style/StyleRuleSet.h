#pragma once
#include "StyleRule.h"
#include "cru/common/Base.h"
#include "cru/common/Event.h"
#include "cru/ui/model/IListChangeNotify.h"

#include <cstddef>

namespace cru::ui::style {
/**
 *  \brief A set of style rules.
 *  \remarks Note this class is different from other constructs in style. It is
 * mutable and has reference semantics. Change of it will be notified by
 * StyleRuleSet::ChangeEvent.
 */
class CRU_UI_API StyleRuleSet : public Object, public model::IListChangeNotify {
 public:
  StyleRuleSet() = default;
  explicit StyleRuleSet(std::shared_ptr<StyleRuleSet> parent);

  CRU_DELETE_COPY(StyleRuleSet)
  CRU_DELETE_MOVE(StyleRuleSet)

  ~StyleRuleSet() override = default;

 public:
  std::shared_ptr<StyleRuleSet> GetParent() const { return parent_; }
  void SetParent(std::shared_ptr<StyleRuleSet> parent);

  gsl::index GetSize() const { return static_cast<gsl::index>(rules_.size()); }
  const std::vector<StyleRule>& GetRules() const { return rules_; }

  void AddStyleRule(StyleRule rule) {
    AddStyleRule(std::move(rule), GetSize());
  }

  void AddStyleRule(StyleRule rule, gsl::index index);

  void RemoveStyleRule(gsl::index index, gsl::index count = 1);

  const StyleRule& GetStyleRule(Index index) const { return rules_[index]; }
  void SetStyleRule(Index index, StyleRule rule);

  void Clear() { RemoveStyleRule(0, GetSize()); }

  void Set(const StyleRuleSet& other, bool set_parent = false);

  const StyleRule& operator[](gsl::index index) const { return rules_[index]; }

  // Triggered whenever a change happened to this (rule add or remove, parent
  // change ...). Subscribe to this and update style change listeners and style.
  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }

  IEvent<model::ListChange>* ListChangeEvent() override {
    return &list_change_event_;
  }

 private:
  void RaiseChangeEvent() { change_event_.Raise(nullptr); }
  void RaiseChangeEvent(model::ListChange list_change) {
    list_change_event_.Raise(list_change);
    change_event_.Raise(nullptr);
  }

 private:
  Event<std::nullptr_t> change_event_;
  Event<model::ListChange> list_change_event_;

  std::shared_ptr<StyleRuleSet> parent_ = nullptr;
  EventRevokerGuard parent_change_event_guard_;

  std::vector<StyleRule> rules_;
};

class CRU_UI_API StyleRuleSetBind {
 public:
  StyleRuleSetBind(controls::Control* control,
                   std::shared_ptr<StyleRuleSet> ruleset);

  CRU_DELETE_COPY(StyleRuleSetBind)
  CRU_DELETE_MOVE(StyleRuleSetBind)

  ~StyleRuleSetBind() = default;

 private:
  void UpdateRuleSetChainCache();
  void UpdateChangeListener();
  void UpdateStyle();

 private:
  controls::Control* control_;
  std::shared_ptr<StyleRuleSet> ruleset_;

  // child first, parent last.
  std::vector<StyleRuleSet*> ruleset_chain_cache_;

  EventRevokerListGuard guard_;
};
}  // namespace cru::ui::style
