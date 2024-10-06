#include "cru/base/PropertyTree.h"
#include <unordered_map>
#include "cru/base/Exception.h"

namespace cru {
String PropertySubTreeRef::CombineKey(StringView left, StringView right) {
  return PropertyTree::CombineKey(left, right);
}

PropertySubTreeRef::PropertySubTreeRef(PropertyTree* tree, String path)
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

PropertySubTreeRef PropertySubTreeRef::GetChild(const String& key) const {
  return PropertySubTreeRef(tree_, CombineKey(path_, key));
}

String PropertySubTreeRef::GetValue(const String& key) const {
  return tree_->GetValue(CombineKey(path_, key));
}

void PropertySubTreeRef::SetValue(const String& key, String value) {
  tree_->SetValue(CombineKey(path_, key), std::move(value));
}

void PropertySubTreeRef::DeleteValue(const String& key) {
  tree_->DeleteValue(CombineKey(path_, key));
}

String PropertyTree::CombineKey(StringView left, StringView right) {
  return String(left) + String(left.empty() ? u"" : u".") + String(right);
}

PropertyTree::PropertyTree(std::unordered_map<String, String> values)
    : values_(std::move(values)) {}

String PropertyTree::GetValue(const String& key) const {
  auto it = values_.find(key);
  if (it == values_.end()) {
    throw Exception(u"Property tree has no value.");
  }
  return it->second;
}

void PropertyTree::SetValue(const String& key, String value) {
  values_[key] = std::move(value);
}

void PropertyTree::DeleteValue(const String& key) {
  auto it = values_.find(key);
  if (it != values_.end()) {
    values_.erase(it);
  }
}

PropertySubTreeRef PropertyTree::GetSubTreeRef(const String& path) {
  return PropertySubTreeRef(this, path);
}

}  // namespace cru
