#pragma once

#include "ParsingAlgorithm.h"

namespace cru::parse {
class CRU_PARSE_API RecursiveDescentAlgorithm : public ParsingAlgorithm {
 public:
  RecursiveDescentAlgorithm() = default;

  CRU_DELETE_COPY(RecursiveDescentAlgorithm)
  CRU_DELETE_MOVE(RecursiveDescentAlgorithm)

  ~RecursiveDescentAlgorithm() override = default;

 public:
  bool CanHandle(Grammar* grammar) const override;
  ParsingAlgorithmContext* CreateContext(Grammar* grammar) const override;
};
}  // namespace cru::parse
