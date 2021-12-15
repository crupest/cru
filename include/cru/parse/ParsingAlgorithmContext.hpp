#pragma once
#include "Grammar.hpp"

namespace cru::parse {
class ParsingAlgorithm;

// A parsing algorithm context contains all data a parsing algorithm needs to
// parse for a grammar. It does not relate to any input. For example, it can
// contain any state machine.
class ParsingAlgorithmContext {
 public:
  ParsingAlgorithmContext(Grammar* grammar, ParsingAlgorithm* algorithm);

  CRU_DELETE_COPY(ParsingAlgorithmContext)
  CRU_DELETE_MOVE(ParsingAlgorithmContext)

  ~ParsingAlgorithmContext();

 private:
  Grammar* grammar_;
  ParsingAlgorithm* algorithm_;
};
}  // namespace cru::parse
