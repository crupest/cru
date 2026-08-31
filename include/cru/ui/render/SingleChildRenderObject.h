#pragma once
#include "RenderObject.h"

#include <cru/base/Event.h>

namespace cru::ui::render {
class CRU_UI_API SingleChildRenderObject : public RenderObject {
 private:
  constexpr static auto kLogTag = "cru::ui::render::SingleChildRenderObject";

 public:
  explicit SingleChildRenderObject(std::string name);
  ~SingleChildRenderObject();

  RenderObject* GetChild() const { return child_; }
  void SetChild(RenderObject* new_child);

 protected:
  virtual void OnChildChanged(RenderObject* old_child, RenderObject* new_child);

 private:
  RenderObject* child_ = nullptr;
  EventHandlerRevokerGuard child_destroy_guard_;
};
}  // namespace cru::ui::render
