#include "cru/ui/render/layout_utility.hpp"

#include <algorithm>

namespace cru::ui::render {
Size Min(const Size& left, const Size& right) {
  return Size{std::min(left.width, right.width),
              std::min(left.height, right.height)};
}

Size Max(const Size& left, const Size& right) {
  return Size{std::max(left.width, right.width),
              std::max(left.height, right.height)};
}
}  // namespace cru::ui::render
