#pragma once
#include "Mapper.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper {
class CRU_UI_API ColorMapper : public BasicMapper<Color> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ColorMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  Color DoMapFromString(String str) override;
  Color DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
