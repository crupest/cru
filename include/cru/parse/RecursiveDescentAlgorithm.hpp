#pragma once

#include "ParsingAlgorithm.hpp"

namespace cru::parse {
class RecursiveDescentAlgorithm : public ParsingAlgorithm {
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
