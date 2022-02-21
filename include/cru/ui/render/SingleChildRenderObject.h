#pragma once
#include "RenderObject.h"

namespace cru::ui::render {
class CRU_UI_API SingleChildRenderObject : public RenderObject {
  CRU_DEFINE_CLASS_LOG_TAG(u"SingleChildRenderObject")

 public:
  SingleChildRenderObject() = default;
  CRU_DELETE_COPY(SingleChildRenderObject)
  CRU_DELETE_MOVE(SingleChildRenderObject)
  ~SingleChildRenderObject() override = default;

  RenderObject* GetChild() const { return child_; }
  void SetChild(RenderObject* new_child);

 protected:
  virtual void OnChildChanged(RenderObject* old_child, RenderObject* new_child);

 private:
  RenderObject* child_ = nullptr;
};
}  // namespace cru::ui::render
