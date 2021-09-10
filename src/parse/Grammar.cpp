#include "cru/parse/Grammar.hpp"
#include "cru/parse/Symbol.hpp"

#include <algorithm>

namespace cru::parse {
Grammar::Grammar() {}

Grammar::~Grammar() {}

Terminal* Grammar::CreateTerminal(String name) {
  auto terminal = new Terminal(this, std::move(name));
  terminals_.push_back(terminal);
  symbols_.push_back(terminal);
  return terminal;
}

Nonterminal* Grammar::CreateNonterminal(String name) {
  auto nonterminal = new Nonterminal(this, std::move(name));
  nonterminals_.push_back(nonterminal);
  symbols_.push_back(nonterminal);
  return nonterminal;
}

Production* Grammar::CreateProduction(String name, Nonterminal* left,
                                      std::vector<Symbol*> right) {
  Expects(left->GetGrammar() == this);
  Expects(std::all_of(right.cbegin(), right.cend(), [this](Symbol* symbol) {
    return symbol->GetGrammar() == this;
  }));

  auto production =
      new Production(this, std::move(name), left, std::move(right));
  productions_.push_back(production);
  return production;
}

}  // namespace cru::parse
