#pragma once
#include "cru/base/String.h"

#include "Nonterminal.h"
#include "Terminal.h"

#include <vector>

namespace cru::parse {
class CRU_PARSE_API Production : public Object {
 public:
  Production(Grammar* grammar, String name, Nonterminal* left,
             std::vector<Symbol*> right);

  CRU_DELETE_COPY(Production)
  CRU_DELETE_MOVE(Production)

  ~Production() override;

 public:
  Grammar* GetGrammar() const { return grammar_; }

  String GetName() const { return name_; }
  void SetName(String name) { name_ = std::move(name); }

  Nonterminal* GetLeft() const { return left_; }
  void SetLeft(Nonterminal* left);

  const std::vector<Symbol*>& GetRight() const { return right_; }
  void SetRight(std::vector<Symbol*> right);

  bool IsLeftRecursion() const {
    return !right_.empty() && left_ == right_.front();
  }

 private:
  Grammar* grammar_;
  String name_;

  Nonterminal* left_;
  std::vector<Symbol*> right_;
};
}  // namespace cru::parse
