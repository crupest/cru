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
  friend class RootControl;

  CRU_DEFINE_CLASS_LOG_TAG("Control")

 protected:
  Control();

 public:
  ~Control() override;

 public:
  virtual std::string GetControlType() const = 0;

  std::string GetDebugId() const;

  //*************** region: tree ***************
 public:
  Window* GetWindow();

  Control* GetParent() const { return parent_; }
  void SetParent(Control* parent);
  bool HasAncestor(Control* control);

  virtual void ForEachChild(const std::function<void(Control*)>& predicate) = 0;

  /**
   *  \remarks This method should be permissive, which means if the specified
   * child control is not a real child of this then nothing will be done.
   */
  virtual void RemoveChild(Control* child) = 0;

  void RemoveFromParent();

  controls::Control* HitTest(const Point& point);

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
  bool IsMouseOver() const { return is_mouse_over_; }

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
  virtual void OnParentChanged(Control* old_parent, Control* new_parent) {}

 private:
  Control* parent_ = nullptr;

  bool is_mouse_over_ = false;

  std::shared_ptr<platform::gui::ICursor> cursor_ = nullptr;

  std::shared_ptr<style::StyleRuleSet> style_rule_set_;
  std::unique_ptr<style::StyleRuleSetBind> style_rule_set_bind_;
};
}  // namespace cru::ui::controls
