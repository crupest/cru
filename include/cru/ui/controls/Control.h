#pragma once
#include "../Base.h"
#include "../events/KeyEventArgs.h"
#include "../events/MouseWheelEventArgs.h"
#include "../events/UiEvents.h"
#include "../render/MeasureRequirement.h"
#include "../render/RenderObject.h"
#include "../style/StyleRuleSet.h"

#include <cru/base/SelfResolvable.h>
#include <cru/platform/gui/DeleteLater.h>

namespace cru::ui::controls {

/**
 *  \remarks If you want to write a new control. You should override following
 * methods:
 *  - GetControlType()
 *  - GetRenderObject()
 */
class CRU_UI_API Control : public Object,
                           public cru::platform::gui::DeleteLaterImpl<Control>,
                           public SelfResolvable<Control> {
  friend class ControlHost;

  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::controls::Control")

 protected:
  explicit Control(std::string name);

 public:
  ~Control() override;

 public:
  std::string GetName();
  std::string GetDebugId();

  //*************** region: tree ***************
 public:
  ControlHost* GetControlHost();

  Control* GetParent();
  bool HasAncestor(Control* control);
  const std::vector<Control*>& GetChildren();
  Index GetChildCount() { return GetChildren().size(); }
  Control* GetChildAt(Index index) { return GetChildren()[index]; }
  Index IndexOfChild(Control* control);

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
  virtual render::RenderObject* GetRenderObject() = 0;

  virtual render::MeasureSize GetSuggestSize() {
    return GetRenderObject()->GetSuggestSize();
  }
  virtual void SetSuggestSize(const render::MeasureSize& size) {
    GetRenderObject()->SetSuggestSize(size);
  }

  virtual Thickness GetMargin() { return GetRenderObject()->GetMargin(); }
  virtual void SetMargin(const Thickness& margin) {
    GetRenderObject()->SetMargin(margin);
  }

  virtual Thickness GetPadding() { return GetRenderObject()->GetPadding(); }
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
  // Raised when mouse enter the control. Even when the control itself
  // captures the mouse, this event is raised as regular. But if mouse is
  // captured by another control, the control will not receive any mouse enter
  // event. You can use `IsMouseCaptured` to get more info.
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
  std::string name_;
  ControlHost* host_;
  Control* parent_;
  std::vector<Control*> children_;

  std::shared_ptr<platform::gui::ICursor> cursor_;

  std::shared_ptr<style::StyleRuleSet> style_rule_set_;
  std::unique_ptr<style::StyleRuleSetBind> style_rule_set_bind_;
};
}  // namespace cru::ui::controls
