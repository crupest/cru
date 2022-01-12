#pragma once
#include "Grammar.hpp"

#include <vector>

namespace cru::parse {
class CRU_PARSE_API ParsingTreeNode {
 public:
  ParsingTreeNode(Symbol* symbol, Production* production);

  CRU_DELETE_COPY(ParsingTreeNode)
  CRU_DELETE_MOVE(ParsingTreeNode)

  // In destructor, it will delete all children.
  ~ParsingTreeNode();

 public:
  Symbol* GetSymbol() const { return symbol_; }
  Production* GetProduction() const { return production_; }
  Grammar* GetGrammar() const;

  const std::vector<ParsingTreeNode*>& GetChildren() const { return children_; }

  void AddChild(ParsingTreeNode* child);
  void AddChild(ParsingTreeNode* child, Index index);
  void RemoveChild(Index index);

 private:
  Symbol* symbol_;
  Production* production_;
  std::vector<ParsingTreeNode*> children_;
};
}  // namespace cru::parse
