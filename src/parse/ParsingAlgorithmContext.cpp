#include "cru/parse/ParsingAlgorithmContext.hpp"

namespace cru::parse {
ParsingAlgorithmContext::ParsingAlgorithmContext(Grammar* grammar,
                                                 ParsingAlgorithm* algorithm)
    : grammar_(grammar), algorithm_(algorithm) {}

ParsingAlgorithmContext::~ParsingAlgorithmContext() {}
}  // namespace cru::parse
