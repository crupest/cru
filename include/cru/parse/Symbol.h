#pragma once
#include "Base.h"

#include <cru/base/Base.h>
#include <string>

namespace cru::parse {
class Grammar;

// Base class of Terminal and Nonterminal.
class CRU_PARSE_API Symbol : public Object {
 public:
  explicit Symbol(Grammar* grammar, std::string name);
  ~Symbol() override;

 public:
  Grammar* GetGrammar() { return grammar_; }

  std::string GetName() const { return name_; }
  void SetName(std::string name) { name_ = std::move(name); }

 private:
  Grammar* grammar_;

  std::string name_;
};
}  // namespace cru::parse
