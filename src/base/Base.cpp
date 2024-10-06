#include "cru/base/Base.h"

#include <exception>

namespace cru {
void UnreachableCode() { std::terminate(); }
}  // namespace cru
