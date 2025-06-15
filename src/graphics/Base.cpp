#include "cru/graphics/Base.h"

#include "cru/Format.h"

namespace cru::graphics {

String ToString(const Point& point) {
  return Format(u"(x: {}, y: {})", point.x, point.y);
}

String ToString(const Size& size) {
  return Format(u"(width: {}, height: {})", size.width, size.height);
}

}  // namespace cru::graphics
