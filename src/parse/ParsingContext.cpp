#include "cru/parse/ParsingContext.h"
#include "cru/parse/ParsingAlgorithmContext.h"

namespace cru::parse {
ParsingContext::ParsingContext(
    const ParsingAlgorithmContext* parsing_algorithm_context,
    std::vector<Terminal*> input)
    : parsing_algorithm_context_(parsing_algorithm_context),
      input_(std::move(input)) {}

ParsingContext::~ParsingContext() {}

}  // namespace cru::parse
