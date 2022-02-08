#pragma once

#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API CornerRadiusMapper : public BasicMapper<CornerRadius> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(CornerRadiusMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  CornerRadius DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
