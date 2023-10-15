#pragma once

#include "../Base.h"
#include "DocumentElementType.h"

namespace cru::ui::document {
class CRU_UI_API DocumentElement : public Object {
 public:
  explicit DocumentElement(DocumentElementType* type);

  ~DocumentElement() override;

 public:
  DocumentElementType* GetType() const { return type_; }

 private:
  DocumentElementType* type_;
};
}  // namespace cru::ui::document
