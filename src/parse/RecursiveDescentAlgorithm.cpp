#include "cru/parse/RecursiveDescentAlgorithm.h"
#include "cru/parse/ParsingAlgorithmContext.h"
#include "cru/parse/RecursiveDescentAlgorithmContext.h"

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
