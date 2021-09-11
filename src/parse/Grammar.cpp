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

bool Grammar::RemoveSymbol(Symbol* symbol) {
  for (auto production : productions_) {
    if (production->GetLeft() == symbol) return false;
    for (auto s : production->GetRight()) {
      if (s == symbol) return false;
    }
  }

  auto i = std::find(symbols_.begin(), symbols_.end(), symbol);

  if (i == symbols_.cend()) return false;

  symbols_.erase(i);

  if (auto t = dynamic_cast<Terminal*>(symbol)) {
    terminals_.erase(std::find(terminals_.begin(), terminals_.end(), t));
  } else if (auto n = dynamic_cast<Nonterminal*>(symbol)) {
    nonterminals_.erase(
        std::find(nonterminals_.begin(), nonterminals_.end(), n));
  }

  delete symbol;

  return true;
}

bool Grammar::RemoveProduction(Production* production) {
  auto i = std::find(productions_.begin(), productions_.end(), production);
  if (i == productions_.cend()) return false;
  productions_.erase(i);
  delete production;
  return true;
}

}  // namespace cru::parse
