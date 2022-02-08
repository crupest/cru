#pragma once

#include "Base.h"
#include "String.h"

#include <unordered_map>

namespace cru {
class PropertyTree;

class CRU_BASE_API PropertySubTreeRef {
 public:
  static String CombineKey(StringView left, StringView right);

  explicit PropertySubTreeRef(PropertyTree* tree, String path = {});

  CRU_DEFAULT_COPY(PropertySubTreeRef);
  CRU_DEFAULT_MOVE(PropertySubTreeRef);

  CRU_DEFAULT_DESTRUCTOR(PropertySubTreeRef);

 public:
  PropertyTree* GetTree() const { return tree_; }

  String GetPath() const { return path_; }
  void SetPath(String path) { path_ = std::move(path); }

  PropertySubTreeRef GetParent() const;
  PropertySubTreeRef GetChild(const String& key) const;

  String GetValue(const String& key) const;
  void SetValue(const String& key, String value);
  void DeleteValue(const String& key);

 private:
  PropertyTree* tree_;
  String path_;
};

class CRU_BASE_API PropertyTree {
 public:
  static String CombineKey(StringView left, StringView right);

  PropertyTree() = default;
  explicit PropertyTree(std::unordered_map<String, String> values);

  CRU_DELETE_COPY(PropertyTree);
  CRU_DELETE_MOVE(PropertyTree);

  CRU_DEFAULT_DESTRUCTOR(PropertyTree);

 public:
  String GetValue(const String& key) const;
  void SetValue(const String& key, String value);
  void DeleteValue(const String& key);

  PropertySubTreeRef GetSubTreeRef(const String& path);

 private:
  std::unordered_map<String, String> values_;
};

}  // namespace cru
