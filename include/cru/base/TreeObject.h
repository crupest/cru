#pragma once

#include "Base.h"

#include <concepts>

namespace cru {

template <typename Self, typename TreeObject>
class SingleChildTreeObjectMixin;

/**
 * Derived class should call DestroyTreeObject() in its destructor.
 */
template <typename Self>
class TreeObjectMixin {
 public:
  template <typename T>
  using SingleChildMixin = ::cru::SingleChildTreeObjectMixin<T, Self>;

 private:
  template <typename, typename>
  friend class SingleChildTreeObjectMixin;

 public:
  ~TreeObjectMixin() {
    // Derived class should call DestroyTreeObject() in its destructor.
    assert(under_destroying_ && parent_ == nullptr && children_.empty());
  }

  bool IsUnderTreeObjectDestroying() { return under_destroying_; }

  Self* GetParent() { return parent_; }
  bool HasAncestor(Self* object) {
    auto parent = AsSelf();
    while (parent) {
      if (parent == object) return true;
      parent = parent->GetParent();
    }
    return false;
  }
  const std::vector<Self*>& GetChildren() { return children_; }
  Index GetChildCount() { return GetChildren().size(); }
  Self* GetChildAt(Index index) { return GetChildren()[index]; }
  Index IndexOfChild(Self* child) {
    const auto& children = GetChildren();
    auto iter = std::ranges::find(children, child);
    if (iter == children.cend()) {
      return -1;
    }
    return iter - children.begin();
  }
  bool HasChild(Self* child) {
    return std::ranges::find(children_, child) != children_.cend();
  }

  bool RemoveChild(Self* child) {
    auto iter = std::ranges::find(children_, child);
    if (iter != children_.cend()) {
      RemoveChildAt(iter - children_.cbegin());
      return true;
    }
    return false;
  }

  void RemoveAllChildren() {
    while (!GetChildren().empty()) {
      RemoveChildAt(GetChildCount() - 1);
    }
  }

  bool RemoveFromParent() {
    if (parent_) {
      return parent_->RemoveChild(AsSelf());
    }
    return false;
  }

  void DetachFromTree() {
    RemoveFromParent();
    RemoveAllChildren();
  }

  template <typename F>
  void TraverseDescendents(F&& f, bool include_this) {
    if (include_this) {
      f(AsSelf());
    }

    for (auto child : GetChildren()) {
      child->TraverseDescendents(std::forward<F>(f), true);
    }
  }

 protected:
  void InsertChildAt(Self* child, Index index) {
    CheckArgumentRange(index, 0, GetChildCount() + 1);
    auto c = static_cast<TreeObjectMixin*>(child);
    if (c->parent_) {
      throw Exception("Control already has a parent.");
    }

    children_.insert(children_.cbegin() + index, child);
    c->parent_ = AsSelf();
    OnChildInserted(child, index);
    c->OnParentChanged(nullptr, AsSelf());
  }

  void RemoveChildAt(Index index) {
    CheckArgumentRange(index, 0, GetChildCount());
    auto child = children_[index];
    auto c = static_cast<TreeObjectMixin*>(child);
    children_.erase(children_.cbegin() + index);
    c->parent_ = nullptr;
    OnChildRemoved(child, index);
    c->OnParentChanged(AsSelf(), nullptr);
  }

  void AddChild(Self* child) { InsertChildAt(child, GetChildCount()); }

  void DestroyTreeObject() {
    under_destroying_ = true;
    DetachFromTree();
  }

  /**
   * Override should call base class's OnParentChanged.
   */
  virtual void OnParentChanged(Self* old_parent, Self* new_parent) {}

  /**
   * Override should call base class's OnChildInserted.
   */
  virtual void OnChildInserted(Self* child, Index index) {}

  /**
   * Override should call base class's OnChildRemoved.
   */
  virtual void OnChildRemoved(Self* child, Index index) {}

 private:
  Self* AsSelf()
    requires(std::derived_from<Self, TreeObjectMixin>)
  {
    return static_cast<Self*>(this);
  }

 private:
  bool under_destroying_ = false;
  Self* parent_ = nullptr;
  std::vector<Self*> children_;
};

template <typename Self, typename TreeObject>
class SingleChildTreeObjectMixin {
 public:
  TreeObject* GetChild() {
    const auto& children = AsSelf()->GetChildren();
    assert(children.empty() || children.size() == 1);
    return children.empty() ? nullptr : children.front();
  }

  void SetChild(TreeObject* child) {
    auto old_child = GetChild();
    if (old_child == child) return;
    auto self = AsSelf();
    if (old_child) {
      self->RemoveChildAt(0);
    }
    if (child) {
      self->InsertChildAt(child, 0);
    }
    OnChildChanged(old_child, child);
  }

 protected:
  virtual void OnChildChanged(TreeObject* old_child, TreeObject* new_child) {}

 private:
  Self* AsSelf()
    requires(
        std::derived_from<Self, TreeObjectMixin<TreeObject>> &&
        std::derived_from<Self, SingleChildTreeObjectMixin<Self, TreeObject>>)
  {
    return static_cast<Self*>(this);
  }
};

}  // namespace cru
