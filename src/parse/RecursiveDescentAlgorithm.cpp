#include "cru/parse/RecursiveDescentAlgorithm.hpp"
#include "cru/parse/ParsingAlgorithmContext.hpp"
#include "cru/parse/RecursiveDescentAlgorithmContext.hpp"

namespace cru::parse {
bool RecursiveDescentAlgorithm::CanHandle(Grammar *grammar) const {
  CRU_UNUSED(grammar);
  return true;
}

ParsingAlgorithmContext *RecursiveDescentAlgorithm::CreateContext(
    Grammar *grammar) const {
  return new RecursiveDescentAlgorithmContext(grammar, this);
}
}  // namespace cru::parse
