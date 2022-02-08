#pragma once
#include "Grammar.h"

namespace cru::parse {
class ParsingAlgorithmContext;

// Represents a parsing algorithm.
// It does not relate to any specific grammar.
// It is used to validate a grammar and create a parsing algorithm context.
class CRU_PARSE_API ParsingAlgorithm {
 public:
  ParsingAlgorithm() = default;

  CRU_DELETE_COPY(ParsingAlgorithm)
  CRU_DELETE_MOVE(ParsingAlgorithm)

  virtual ~ParsingAlgorithm() = default;

  virtual bool CanHandle(Grammar* grammar) const = 0;
  virtual ParsingAlgorithmContext* CreateContext(Grammar* grammar) const = 0;
};
}  // namespace cru::parse
