#pragma once
#include "../Base.h"
#include "../events/KeyEventArgs.h"
#include "../events/MouseWheelEventArgs.h"
#include "../events/UiEvents.h"
#include "../render/MeasureRequirement.h"
#include "../render/RenderObject.h"
#include "../style/StyleRuleSet.h"

#include <cru/base/SelfResolvable.h>
#include <cru/base/TreeObject.h>
#include <cru/platform/gui/DeleteLater.h>
#include <cru/platform/gui/InputMethod.h>

#include <cstddef>
#include <string>

namespace cru::ui::controls {

struct ControlHostChangeEventArgs {
  ControlHost* old_host;
  ControlHost* new_host;
};

/**
 *  \remarks If you want to write a new control. You should override following
 * methods:
 *  - GetControlType()
 *  - GetRenderObject()
 */
class CRU_UI_API Control : public Object,
                           public TreeObjectMixin<Control>,
                           public cru::platform::gui::DeleteLaterImpl<Control>,
                           public SelfResolvable<Control> {
  friend class ControlHost;

 private:
  constexpr static auto kLogTag = "cru::ui::controls::Control";

 protected:
  Control(std::string name, render::RenderObject* root_render_object);

 public:
  ~Control() override;

 public:
  std::string GetName();
  std::string GetDebugId();
  ControlHost* GetControlHost();
  controls::Control* HitTest(const Point& point);

 public:
  render::RenderObject* GetRenderObject() { return root_render_object_; }

  Point GetRenderObjectOffset(render::RenderObject* render_object);

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

  CRU_DEFINE_EVENT(CompositionStart, std::nullptr_t)
  CRU_DEFINE_EVENT(Composition, const platform::gui::CompositionText&)
  CRU_DEFINE_EVENT(CompositionEnd, std::nullptr_t)
  CRU_DEFINE_EVENT(TextInput, const std::string&)

  CRU_DEFINE_EVENT(ControlHostChange, const ControlHostChangeEventArgs&)

  //*************** region: tree ***************
 protected:
  void OnChildInserted(Control* control, Index index) override;
  void OnChildRemoved(Control* control, Index index) override;

 private:
  std::string name_;
  ControlHost* host_;

  std::shared_ptr<platform::gui::ICursor> cursor_;

  std::shared_ptr<style::StyleRuleSet> style_rule_set_;
  std::unique_ptr<style::StyleRuleSetBind> style_rule_set_bind_;

 protected:
  // Maybe dangling.
  render::RenderObject* root_render_object_;
};
}  // namespace cru::ui::controls
