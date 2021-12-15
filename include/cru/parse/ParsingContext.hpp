#pragma once
#include "ParsingAlgorithmContext.hpp"

namespace cru::parse {
// A parsing context contains all info that a program needs to know when parsing
// a input sequence of terminals.
class ParsingContext {
 public:
  ParsingContext(ParsingAlgorithmContext* parsing_algorithm_context,
                 std::vector<Terminal*> input);

  CRU_DELETE_COPY(ParsingContext)
  CRU_DELETE_MOVE(ParsingContext)

  ~ParsingContext();

 private:
  ParsingAlgorithmContext* parsing_algorithm_context_;
  std::vector<Terminal*> input_;
};
}  // namespace cru::parse
