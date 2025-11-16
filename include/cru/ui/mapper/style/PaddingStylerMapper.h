#pragma once
#include "../Mapper.h"
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
class CRU_UI_API PaddingStylerMapper
    : public BasicClonePtrMapper<ui::style::PaddingStyler>,
      public virtual IStylerMapper {
 public:
  PaddingStylerMapper();
  ~PaddingStylerMapper();

 public:
  bool SupportMapFromXml() override { return true; }

  ClonePtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonePtr<ui::style::PaddingStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
