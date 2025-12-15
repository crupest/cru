#pragma once
#include "../../style/StyleRule.h"
#include "../Mapper.h"

namespace cru::ui::mapper::style {
class CRU_UI_API StyleRuleMapper
    : public BasicClonePtrMapper<ui::style::StyleRule> {
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(StyleRule,
                                              ClonePtr<ui::style::StyleRule>)
};
}  // namespace cru::ui::mapper::style
