#pragma once
#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API PointMapper : public BasicMapper<Point> {
  CRU_UI_DECLARE_CAN_MAP_FROM_STRING(Point)
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(Point, Point)
};
}  // namespace cru::ui::mapper
