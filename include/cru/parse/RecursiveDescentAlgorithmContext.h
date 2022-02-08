#pragma once

#include "ParsingAlgorithmContext.h"
#include "cru/parse/ParsingTreeNode.h"
#include "cru/parse/RecursiveDescentAlgorithm.h"
#include "cru/parse/Terminal.h"

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
