#include <iostream>

#include "cru/parse/Grammar.h"

int main() {
  using namespace cru::parse;
  Grammar grammar;

  auto S = grammar.CreateNonterminal(u"S");
  auto a = grammar.CreateTerminal(u"a");

  grammar.CreateProduction(u"S := S a", S, {S, a});
  grammar.CreateProduction(u"S := a", S, {a});
  grammar.SetStartSymbol(S);

  grammar.EliminateLeftRecursions();

  std::cout << grammar.ProductionsToString().ToUtf8();

  return 0;
}
