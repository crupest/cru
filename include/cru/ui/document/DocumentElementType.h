#pragma once

#include "../Base.h"

#include <vector>

namespace cru::ui::document {
class CRU_UI_API DocumentElementType : public Object {
 public:
  explicit DocumentElementType(std::string name,
                               std::vector<DocumentElementType*> parents);

  ~DocumentElementType() override;

 public:
  std::string GetName() const { return name_; }
  const std::vector<DocumentElementType*>& GetParents() const {
    return parents_;
  }

 private:
  std::string name_;
  std::vector<DocumentElementType*> parents_;
};

struct CRU_UI_API DocumentElementTypes {
  static DocumentElementType* const kBaseElementType;
  static DocumentElementType* const kRootElementType;
  static DocumentElementType* const kTextElementType;
};

}  // namespace cru::ui::document
