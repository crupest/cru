#pragma once

#include "ParsingAlgorithmContext.hpp"
#include "cru/parse/ParsingTreeNode.hpp"
#include "cru/parse/RecursiveDescentAlgorithm.hpp"
#include "cru/parse/Terminal.hpp"

namespace cru::parse {
class CRU_PARSE_API RecursiveDescentAlgorithmContext : public ParsingAlgorithmContext {
 public:
  RecursiveDescentAlgorithmContext(Grammar* grammar,
                                   const RecursiveDescentAlgorithm* algorithm);

  CRU_DELETE_COPY(RecursiveDescentAlgorithmContext)
  CRU_DELETE_MOVE(RecursiveDescentAlgorithmContext)

  ~RecursiveDescentAlgorithmContext() override;

 public:
  ParsingTreeNode* Parse(const std::vector<Terminal*>& input) override;
};
}  // namespace cru::parse
