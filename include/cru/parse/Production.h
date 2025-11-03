#pragma once
#include "Nonterminal.h"

#include <vector>
#include <string>

namespace cru::parse {
class CRU_PARSE_API Production : public Object {
 public:
  Production(Grammar* grammar, std::string name, Nonterminal* left,
             std::vector<Symbol*> right);
  ~Production() override;

 public:
  Grammar* GetGrammar() const { return grammar_; }

  std::string GetName() const { return name_; }
  void SetName(std::string name) { name_ = std::move(name); }

  Nonterminal* GetLeft() const { return left_; }
  void SetLeft(Nonterminal* left);

  const std::vector<Symbol*>& GetRight() const { return right_; }
  void SetRight(std::vector<Symbol*> right);

  bool IsLeftRecursion() const {
    return !right_.empty() && left_ == right_.front();
  }

 private:
  Grammar* grammar_;
  std::string name_;

  Nonterminal* left_;
  std::vector<Symbol*> right_;
};
}  // namespace cru::parse
