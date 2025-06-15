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


  ~RecursiveDescentAlgorithmContext() override;

 public:
  ParsingTreeNode* Parse(const std::vector<Terminal*>& input) override;
};
}  // namespace cru::parse
