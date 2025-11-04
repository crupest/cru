#pragma once
#include "Mapper.h"

#include "../Base.h"
#include "cru/base/Base.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper {
class CRU_UI_API ThicknessMapper : public BasicMapper<Thickness> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ThicknessMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  Thickness DoMapFromString(std::string str) override;
  Thickness DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
