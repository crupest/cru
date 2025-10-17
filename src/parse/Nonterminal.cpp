#include "cru/parse/Nonterminal.h"

namespace cru::parse {
Nonterminal::Nonterminal(Grammar* grammar, std::string name)
    : Symbol(grammar, std::move(name)) {}

Nonterminal::~Nonterminal() {}
}  // namespace cru::parse
