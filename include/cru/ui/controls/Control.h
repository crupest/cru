#pragma once
#include "../Base.h"
#include "../DeleteLater.h"
#include "../events/KeyEventArgs.h"
#include "../events/MouseWheelEventArgs.h"
#include "../events/UiEvents.h"
#include "../render/MeasureRequirement.h"
#include "../render/RenderObject.h"
#include "../style/StyleRuleSet.h"

#include <cru/base/SelfResolvable.h>

namespace cru::ui::controls {

/**
 *  \remarks If you want to write a new control. You should override following
 * methods:
 *  - GetControlType()
 *  - GetRenderObject()
 *  - ForEachChild(const std::function<void(Control*)>& predicate)
 *  - RemoveChild(Control* child)
 * The last two methods are totally for convenient control tree management.
 */
class CRU_UI_API Control : public Object,
                           public DeleteLaterImpl,
                           public SelfResolvable<Control> {
  friend class ControlHost;

  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::controls::Control")

 protected:
  Control();

 public:
  ~Control() override;

 public:
  virtual std::string GetControlType() const = 0;

  std::string GetDebugId() const;

  //*************** region: tree ***************
 public:
  ControlHost* GetControlHost();

  Control* GetParent();
  bool HasAncestor(Control* control);
  const std::vector<Control*>& GetChildren();
  void RemoveChild(Control* child);
  void RemoveAllChild();
  void RemoveFromParent();

  template <typename F>
  void TraverseDescendents(F&& f, bool include_this) {
    if (include_this) {
      f(this);
    }

    for (auto child : GetChildren()) {
      child->TraverseDescendents(std::forward<F>(f), true);
    }
  }

  controls::Control* HitTest(const Point& point);

 protected:
  void InsertChildAt(Control* control, Index index);
  void RemoveChildAt(Index index);
  void AddChild(Control* control);

 public:
  virtual render::RenderObject* GetRenderObject() const = 0;

  virtual render::MeasureSize GetPreferredSize() const {
    return GetRenderObject()->GetPreferredSize();
  }
  virtual void SetPreferredSize(const render::MeasureSize& size) {
    GetRenderObject()->SetPreferredSize(size);
  }

  virtual Thickness GetMargin() const { return GetRenderObject()->GetMargin(); }
  virtual void SetMargin(const Thickness& margin) {
    GetRenderObject()->SetMargin(margin);
  }

  virtual Thickness GetPadding() const {
    return GetRenderObject()->GetPadding();
  }
  virtual void SetPadding(const Thickness& padding) {
    GetRenderObject()->SetPadding(padding);
  }

  //*************** region: focus ***************
 public:
  bool HasFocus();

  void SetFocus();

  //*************** region: mouse ***************
 public:
  bool IsMouseOver();

  bool CaptureMouse();

  bool ReleaseMouse();

  bool IsMouseCaptured();

  //*************** region: cursor ***************
  // Cursor is inherited from parent recursively if not set.
 public:
  // null for not set
  std::shared_ptr<platform::gui::ICursor> GetCursor();

  // will not return nullptr
  std::shared_ptr<platform::gui::ICursor> GetInheritedCursor();

  // null to unset
  void SetCursor(std::shared_ptr<platform::gui::ICursor> cursor);

 public:
  std::shared_ptr<style::StyleRuleSet> GetStyleRuleSet();

  //*************** region: events ***************
 public:
  // Raised when mouse enter the control. Even when the control itself captures
  // the mouse, this event is raised as regular. But if mouse is captured by
  // another control, the control will not receive any mouse enter event. You
  // can use `IsMouseCaptured` to get more info.
  CRU_DEFINE_ROUTED_EVENT(MouseEnter, events::MouseEventArgs)

  // Raised when mouse is leave the control. Even when the control itself
  // captures the mouse, this event is raised as regular. But if mouse is
  // captured by another control, the control will not receive any mouse leave
  // event. You can use `IsMouseCaptured` to get more info.
  CRU_DEFINE_ROUTED_EVENT(MouseLeave, events::MouseEventArgs)

  CRU_DEFINE_ROUTED_EVENT(MouseMove, events::MouseEventArgs)
  CRU_DEFINE_ROUTED_EVENT(MouseDown, events::MouseButtonEventArgs)
  CRU_DEFINE_ROUTED_EVENT(MouseUp, events::MouseButtonEventArgs)
  CRU_DEFINE_ROUTED_EVENT(MouseWheel, events::MouseWheelEventArgs)
  CRU_DEFINE_ROUTED_EVENT(KeyDown, events::KeyEventArgs)
  CRU_DEFINE_ROUTED_EVENT(KeyUp, events::KeyEventArgs)
  CRU_DEFINE_ROUTED_EVENT(GainFocus, events::FocusChangeEventArgs)
  CRU_DEFINE_ROUTED_EVENT(LoseFocus, events::FocusChangeEventArgs)

  //*************** region: tree ***************
 protected:
  virtual void OnParentChanged(Control* old_parent, Control* new_parent);
  virtual void OnChildInserted(Control* control, Index index);
  virtual void OnChildRemoved(Control* control, Index index);

 private:
  ControlHost* host_ = nullptr;
  Control* parent_ = nullptr;
  std::vector<Control*> children_;

  std::shared_ptr<platform::gui::ICursor> cursor_ = nullptr;

  std::shared_ptr<style::StyleRuleSet> style_rule_set_;
  std::unique_ptr<style::StyleRuleSetBind> style_rule_set_bind_;
};
}  // namespace cru::ui::controls
