#pragma once
#include "Grammar.hpp"

namespace cru::parse {
class ParsingAlgorithmContext;

// Represents a parsing algorithm.
// It does not relate to any specific grammar.
// It is used to validate a grammar and create a parsing algorithm context.
class ParsingAlgorithm {
 public:
  ParsingAlgorithm() = default;

  CRU_DELETE_COPY(ParsingAlgorithm)
  CRU_DELETE_MOVE(ParsingAlgorithm)

  ~ParsingAlgorithm() = default;

  virtual bool CanHandle(Grammar* grammar) const = 0;
  virtual ParsingAlgorithmContext* CreateContext(Grammar* grammar) const = 0;
};
}  // namespace cru::parse
