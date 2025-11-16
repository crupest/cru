#pragma once
#include "../Mapper.h"
#include "IStylerMapper.h"

namespace cru::ui::mapper::style {
class FontStylerMapper : public BasicClonePtrMapper<ui::style::FontStyler>,
                         public virtual IStylerMapper {
 public:
  FontStylerMapper();
  ~FontStylerMapper() override;

 public:
  bool SupportMapFromXml() override { return true; }

  ClonePtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonePtr<ui::style::FontStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
