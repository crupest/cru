#include "cru/parse/ParsingTreeNode.hpp"

namespace cru::parse {
ParsingTreeNode::ParsingTreeNode(Symbol* symbol, Production* production)
    : symbol_(symbol), production_(production) {}

ParsingTreeNode::~ParsingTreeNode() {
  for (auto child : children_) {
    delete child;
  }
}

Grammar* ParsingTreeNode::GetGrammar() const { return symbol_->GetGrammar(); }

void ParsingTreeNode::AddChild(ParsingTreeNode* child) {
  children_.push_back(child);
}

void ParsingTreeNode::AddChild(ParsingTreeNode* child, Index index) {
  children_.insert(children_.begin() + index, child);
}

void ParsingTreeNode::RemoveChild(Index index) {
  delete children_[index];
  children_.erase(children_.begin() + index);
}
}  // namespace cru::parse
