#pragma once

#include "Base.h"

#include <string>
#include <string_view>
#include <unordered_map>

namespace cru {
class PropertyTree;

class CRU_BASE_API PropertySubTreeRef {
 public:
  static std::string CombineKey(std::string_view left, std::string_view right);

  explicit PropertySubTreeRef(PropertyTree* tree, std::string path = {});

 public:
  PropertyTree* GetTree() const { return tree_; }

  std::string GetPath() const { return path_; }
  void SetPath(std::string path) { path_ = std::move(path); }

  PropertySubTreeRef GetParent() const;
  PropertySubTreeRef GetChild(const std::string& key) const;

  std::string GetValue(const std::string& key) const;
  void SetValue(const std::string& key, std::string value);
  void DeleteValue(const std::string& key);

 private:
  PropertyTree* tree_;
  std::string path_;
};

class CRU_BASE_API PropertyTree : public Object2 {
 public:
  static std::string CombineKey(std::string_view left, std::string_view right);

  PropertyTree() = default;
  explicit PropertyTree(std::unordered_map<std::string, std::string> values);

 public:
  std::string GetValue(const std::string& key) const;
  void SetValue(const std::string& key, std::string value);
  void DeleteValue(const std::string& key);

  PropertySubTreeRef GetSubTreeRef(const std::string& path);

 private:
  std::unordered_map<std::string, std::string> values_;
};

}  // namespace cru
