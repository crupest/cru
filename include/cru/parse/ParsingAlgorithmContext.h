#pragma once
#include <vector>
#include "Grammar.h"
#include "cru/parse/ParsingTreeNode.h"
#include "cru/parse/Terminal.h"

namespace cru::parse {
class ParsingAlgorithm;

// A parsing algorithm context contains all data a parsing algorithm needs to
// parse for a grammar. It does not relate to any input. For example, it can
// contain any state machine.
class CRU_PARSE_API ParsingAlgorithmContext {
 public:
  ParsingAlgorithmContext(Grammar* grammar, const ParsingAlgorithm* algorithm);

  CRU_DELETE_COPY(ParsingAlgorithmContext)
  CRU_DELETE_MOVE(ParsingAlgorithmContext)

  virtual ~ParsingAlgorithmContext();

  virtual ParsingTreeNode* Parse(const std::vector<Terminal*>& input) = 0;

 private:
  Grammar* grammar_;
  const ParsingAlgorithm* algorithm_;
};
}  // namespace cru::parse
