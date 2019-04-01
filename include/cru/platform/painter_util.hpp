#pragma once
#include "painter.hpp"

#include <functional>

namespace cru::platform::util {
inline void WithTransform(Painter* painter, const Matrix& matrix,
                   const std::function<void(Painter*)>& action) {
  const auto old = painter->GetTransform();
  painter->SetTransform(old * matrix);
  action(painter);
  painter->SetTransform(old);
}
}
