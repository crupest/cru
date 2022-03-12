#include "cru/platform/GraphicsBase.h"

#include <limits>

namespace cru::platform {
const Size Size::kZero(0.0f, 0.0f);
const Size Size::kMax(std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max());
}  // namespace cru::platform
