#include "cru/parse/ParsingContext.hpp"
#include "cru/parse/ParsingAlgorithmContext.hpp"

namespace cru::parse {
ParsingContext::ParsingContext(
    ParsingAlgorithmContext* parsing_algorithm_context,
    std::vector<Terminal*> input)
    : parsing_algorithm_context_(parsing_algorithm_context),
      input_(std::move(input)) {}

ParsingContext::~ParsingContext() {}

}  // namespace cru::parse
