#pragma once
#include "Base.h"

#include <cru/String.h>

namespace cru::parse {
class Grammar;

// Base class of Terminal and Nonterminal.
class CRU_PARSE_API Symbol : public Object {
 public:
  explicit Symbol(Grammar* grammar, String name);


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
