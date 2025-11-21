#pragma once
#include "cru/ui/controls/TextBlock.h"

namespace cru::theme_builder {
class LabeledMixin {
 public:
  std::string GetLabel() { return label_.GetText(); }
  void SetLabel(std::string label) { label_.SetText(std::move(label)); }

 protected:
  ui::controls::TextBlock label_;
};
}  // namespace cru::theme_builder
