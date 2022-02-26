#pragma once
#include "../Mapper.h"
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
class CRU_UI_API PaddingStylerMapper
    : public BasicPtrMapper<ui::style::PaddingStyler>,
      public virtual IStylerMapper {
 public:
  PaddingStylerMapper();
  ~PaddingStylerMapper();

 public:
  bool SupportMapFromXml() override { return true; }

  ClonablePtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonablePtr<ui::style::PaddingStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
