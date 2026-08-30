#include "cru/ui/components/TextButton.h"

namespace cru::ui::components {
TextButton::TextButton(std::string text) {
  text_.SetText(std::move(text));
  button_.SetChild(&text_);
}
}  // namespace cru::ui::components
