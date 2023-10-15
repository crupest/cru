#pragma once

#include "cru/common/String.h"

#include "../Base.h"

namespace cru::ui::document {
class CRU_UI_API DocumentElementType : public Object {
 public:
  explicit DocumentElementType(String name);

  ~DocumentElementType() override;

 public:
  String GetName() const { return name_; }

 private:
  String name_;
};

struct CRU_UI_API DocumentElementTypes {
  static DocumentElementType* const kRootElementType;
};

}  // namespace cru::ui::document
