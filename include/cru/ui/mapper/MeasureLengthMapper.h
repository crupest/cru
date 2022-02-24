#pragma once
#include "../render/MeasureRequirement.h"
#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API MeasureLengthMapper
    : public BasicMapper<render::MeasureLength> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(MeasureLengthMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  render::MeasureLength DoMapFromString(String str) override;
  render::MeasureLength DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
