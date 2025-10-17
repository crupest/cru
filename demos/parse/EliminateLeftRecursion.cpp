#include <iostream>

#include "cru/parse/Grammar.h"

int main() {
  using namespace cru::parse;
  Grammar grammar;

  auto S = grammar.CreateNonterminal("S");
  auto a = grammar.CreateTerminal("a");

  grammar.CreateProduction("S := S a", S, {S, a});
  grammar.CreateProduction("S := a", S, {a});
  grammar.SetStartSymbol(S);

  grammar.EliminateLeftRecursions();

  std::cout << grammar.ProductionsToString();

  return 0;
}
