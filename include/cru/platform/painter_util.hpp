#pragma once
#include "painter.hpp"

#include <functional>
#include <type_traits>

namespace cru::platform::util {
template <typename Fn>
inline void WithTransform(Painter* painter, const Matrix& matrix,
                          const Fn& action) {
  static_assert(std::is_invocable_v<decltype(action), Painter*>,
                "Action must can be be invoked with painter.");
  const auto old = painter->GetTransform();
  painter->SetTransform(old * matrix);
  action(painter);
  painter->SetTransform(old);
}
}  // namespace cru::platform::util
