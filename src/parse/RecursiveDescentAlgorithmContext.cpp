#include "cru/parse/RecursiveDescentAlgorithmContext.h"
#include "cru/parse/ParsingTreeNode.h"

namespace cru::parse {
RecursiveDescentAlgorithmContext::RecursiveDescentAlgorithmContext(
    Grammar* grammar, const RecursiveDescentAlgorithm* algorithm)
    : ParsingAlgorithmContext(grammar, algorithm) {}

RecursiveDescentAlgorithmContext::~RecursiveDescentAlgorithmContext() = default;

ParsingTreeNode* RecursiveDescentAlgorithmContext::Parse(
    const std::vector<Terminal*>& input) {
  CRU_UNUSED(input)
  // TODO: Implement this.
  return nullptr;
}
}  // namespace cru::parse
