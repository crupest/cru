#pragma once
#include "../Mapper.h"
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
class CRU_UI_API MarginStylerMapper
    : public BasicCopyPtrMapper<ui::style::MarginStyler>,
      public virtual IStylerMapper {
 public:
  MarginStylerMapper();
  ~MarginStylerMapper();

 public:
  bool SupportMapFromXml() override { return true; }

  CopyPtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  CopyPtr<ui::style::MarginStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
