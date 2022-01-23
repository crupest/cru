#pragma once
#include "../Mapper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/style/Styler.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API BorderStylerMapper
    : public BasicPtrMapper<ui::style::BorderStyler> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(BorderStylerMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  ClonablePtr<ui::style::BorderStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
