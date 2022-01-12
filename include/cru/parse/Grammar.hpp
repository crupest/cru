#pragma once
#include "Production.hpp"

#include <unordered_map>
#include <vector>

namespace cru::parse {
class CRU_PARSE_API Grammar : public Object {
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

  Grammar* Clone() const;

 public:  // Algorithms
  std::unordered_map<Nonterminal*, std::vector<Production*>>
  GenerateLeftProductionMap() const;

  // Algorithm 4.19.
  // Require grammar has no cycles or empty-productions.
  void EliminateLeftRecursions();

  // Algorithm 4.21
  void LeftFactor();

 public:
  String ProductionsToString() const;

 private:
  Nonterminal* start_symbol_ = nullptr;
  std::vector<Terminal*> terminals_;
  std::vector<Nonterminal*> nonterminals_;
  std::vector<Symbol*> symbols_;
  std::vector<Production*> productions_;
};
}  // namespace cru::parse
