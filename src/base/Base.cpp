#include "cru/base/Base.h"

#include <exception>

namespace cru {
void UnreachableCode() { std::terminate(); }
void NotImplemented() { std::terminate(); }
}  // namespace cru
