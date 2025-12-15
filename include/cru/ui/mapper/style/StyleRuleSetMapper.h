#pragma once
#include "../../style/StyleRuleSet.h"
#include "../Mapper.h"

namespace cru::ui::mapper::style {
class CRU_UI_API StyleRuleSetMapper
    : public BasicSharedPtrMapper<ui::style::StyleRuleSet> {
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(
      StyleRuleSet, std::shared_ptr<ui::style::StyleRuleSet>)
};
;
}  // namespace cru::ui::mapper::style
