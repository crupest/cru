#include "cru/parse/Production.h"

namespace cru::parse {
Production::Production(Grammar* grammar, String name, Nonterminal* left,
                       std::vector<Symbol*> right)
    : grammar_(grammar),
      name_(std::move(name)),
      left_(left),
      right_(std::move(right)) {}

Production::~Production() {}
}  // namespace cru::parse
