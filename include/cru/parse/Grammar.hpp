#pragma once
#include "Production.hpp"

#include <vector>

namespace cru::parse {
class Grammar : public Object {
 public:
  Grammar();

  CRU_DELETE_COPY(Grammar)
  CRU_DELETE_MOVE(Grammar)

  ~Grammar() override;

 public:
  void SetStartSymbol(Nonterminal* start_symbol);

  Terminal* CreateTerminal(String name);
  Nonterminal* CreateNonterminal(String name);
  Production* CreateProduction(String name, Nonterminal* left,
                               std::vector<Symbol*> right);

  bool RemoveSymbol(Symbol* symbol);
  bool RemoveProduction(Production* production);

 public:  // Getters
  Nonterminal* GetStartSymbol() const { return start_symbol_; }
  const std::vector<Terminal*>& GetTerminals() const { return terminals_; }
  const std::vector<Nonterminal*>& GetNonterminals() const {
    return nonterminals_;
  }
  const std::vector<Symbol*>& GetSymbols() const { return symbols_; }
  const std::vector<Production*>& GetProductions() const {
    return productions_;
  }

 private:
  Nonterminal* start_symbol_;
  std::vector<Terminal*> terminals_;
  std::vector<Nonterminal*> nonterminals_;
  std::vector<Symbol*> symbols_;
  std::vector<Production*> productions_;
};
}  // namespace cru::parse
