#pragma once
#include "cru/common/String.hpp"

#include "Nonterminal.hpp"
#include "Terminal.hpp"

#include <vector>

namespace cru::parse {
class Production : public Object {
 public:
  Production(Grammar* grammar, String name, Nonterminal* left,
             std::vector<Symbol*> right);

  CRU_DELETE_COPY(Production)
  CRU_DELETE_MOVE(Production)

  ~Production() override;

 public:
  Grammar* GetGrammar() const { return grammar_; }

  Nonterminal* GetLeft() const { return left_; }
  void SetLeft(Nonterminal* left);

  const std::vector<Symbol*>& GetRight() const { return right_; }
  void SetRight(std::vector<Symbol*> right);

 private:
  Grammar* grammar_;
  String name_;

  Nonterminal* left_;
  std::vector<Symbol*> right_;
};
}  // namespace cru::parse
