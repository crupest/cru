#include "cru/base/PropertyTree.h"

namespace cru {
std::string PropertySubTreeRef::CombineKey(std::string_view left,
                                           std::string_view right) {
  return PropertyTree::CombineKey(left, right);
}

PropertySubTreeRef::PropertySubTreeRef(PropertyTree* tree, std::string path)
    : tree_(tree), path_(std::move(path)) {
  Expects(tree);
}

PropertySubTreeRef PropertySubTreeRef::GetParent() const {
  for (Index i = path_.size() - 1; i >= 0; i--) {
    if (path_[i] == '.') {
      return PropertySubTreeRef(tree_, path_.substr(0, i));
    }
  }

  return PropertySubTreeRef(tree_, {});
}

PropertySubTreeRef PropertySubTreeRef::GetChild(const std::string& key) const {
  return PropertySubTreeRef(tree_, CombineKey(path_, key));
}

std::string PropertySubTreeRef::GetValue(const std::string& key) const {
  return tree_->GetValue(CombineKey(path_, key));
}

void PropertySubTreeRef::SetValue(const std::string& key, std::string value) {
  tree_->SetValue(CombineKey(path_, key), std::move(value));
}

void PropertySubTreeRef::DeleteValue(const std::string& key) {
  tree_->DeleteValue(CombineKey(path_, key));
}

std::string PropertyTree::CombineKey(std::string_view left,
                                     std::string_view right) {
  return std::string(left) + std::string(left.empty() ? "" : ".") +
         std::string(right);
}

PropertyTree::PropertyTree(std::unordered_map<std::string, std::string> values)
    : values_(std::move(values)) {}

std::string PropertyTree::GetValue(const std::string& key) const {
  auto it = values_.find(key);
  if (it == values_.end()) {
    throw Exception("Property tree has no value.");
  }
  return it->second;
}

void PropertyTree::SetValue(const std::string& key, std::string value) {
  values_[key] = std::move(value);
}

void PropertyTree::DeleteValue(const std::string& key) {
  auto it = values_.find(key);
  if (it != values_.end()) {
    values_.erase(it);
  }
}

PropertySubTreeRef PropertyTree::GetSubTreeRef(const std::string& path) {
  return PropertySubTreeRef(this, path);
}

}  // namespace cru
