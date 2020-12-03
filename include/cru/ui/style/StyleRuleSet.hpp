#pragma once
#include "StyleRule.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "gsl/gsl_assert"

namespace cru::ui::style {
class StyleRuleSet : public Object {
 public:
  StyleRuleSet() : control_(nullptr) {}
  explicit StyleRuleSet(controls::Control* control) : control_(control) {}

  CRU_DELETE_COPY(StyleRuleSet)
  CRU_DELETE_MOVE(StyleRuleSet)

  ~StyleRuleSet() override = default;

 public:
  gsl::index GetSize() const { return static_cast<gsl::index>(rules_.size()); }
  const std::vector<StyleRule>& GetRules() const { return rules_; }

  void AddStyleRule(StyleRule rule) {
    AddStyleRule(std::move(rule), GetSize());
  }

  void AddStyleRule(StyleRule rule, gsl::index index);

  template <typename Iter>
  void AddStyleRuleRange(Iter start, Iter end, gsl::index index) {
    Expects(index >= 0 && index <= GetSize());
    rules_.insert(rules_.cbegin() + index, std::move(start), std::move(end));
    UpdateChangeListener();
    UpdateStyle();
  }

  void RemoveStyleRule(gsl::index index, gsl::index count = 1);

  void Clear() { RemoveStyleRule(0, GetSize()); }

  void Set(const StyleRuleSet& other);

  const StyleRule& operator[](gsl::index index) const { return rules_[index]; }

 private:
  void UpdateChangeListener();
  void UpdateStyle();

 private:
  controls::Control* control_;

  std::vector<StyleRule> rules_;

  EventRevokerListGuard guard_;
};
}  // namespace cru::ui::style
