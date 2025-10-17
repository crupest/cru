#pragma once
#include "Symbol.h"
#include <cru/base/Base.h>

namespace cru::parse {
class CRU_PARSE_API Terminal : public Symbol {
 public:
  Terminal(Grammar* grammar, std::string name);

  CRU_DELETE_COPY(Terminal)
  CRU_DELETE_MOVE(Terminal)

  ~Terminal() override;
};
}  // namespace cru::parse
