#pragma once
#include "Base.hpp"

#include "cru/common/String.hpp"

namespace cru::parse {
class Grammar;

// Base class of Terminal and Nonterminal.
class CRU_PARSE_API Symbol : public Object {
 public:
  explicit Symbol(Grammar* grammar, String name);

  CRU_DELETE_COPY(Symbol)
  CRU_DELETE_MOVE(Symbol)

  ~Symbol() override;

 public:
  Grammar* GetGrammar() { return grammar_; }

  String GetName() const { return name_; }
  void SetName(String name) { name_ = std::move(name); }

 private:
  Grammar* grammar_;

  String name_;
};
}  // namespace cru::parse
