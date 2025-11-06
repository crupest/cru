#pragma once

#include "Base.h"

#include <cru/base/Base.h>
#include <cru/platform/gui/InputMethod.h>

#include <xcb-imdkit/imclient.h>
#include <xcb/xcb.h>
#include <optional>

namespace cru::platform::gui::xcb {
class XcbUiApplication;
class XcbWindow;
class XcbXimInputMethodContext;

class XcbXimInputMethodManager : public XcbResource {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::gui::xcb::XcbXimInputMethodManager")

  friend XcbUiApplication;
  friend XcbXimInputMethodContext;

 public:
  XcbXimInputMethodManager(XcbUiApplication* application);
  ~XcbXimInputMethodManager() override;

  xcb_xim_t* GetXcbXim();

 private:
  void DispatchCommit(xcb_xim_t* im, xcb_xic_t ic, std::string text);

  bool HandleXEvent(xcb_generic_event_t* event);
  void SetXimServerUnprocessedXEventCallback(
      std::function<void(xcb_key_press_event_t* event)> callback);

 private:
  XcbUiApplication* application_;
  XcbXimInputMethodContext* focus_context_;
  xcb_xim_t* im_;
  std::function<void(xcb_key_press_event_t* event)> forward_event_callback_;
};

class XcbXimInputMethodContext : public XcbResource,
                                 public virtual IInputMethodContext {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::gui::xcb::XcbXimInputMethodContext")

  friend XcbXimInputMethodManager;

 public:
  XcbXimInputMethodContext(XcbXimInputMethodManager* manager,
                           XcbWindow* window);
  ~XcbXimInputMethodContext() override;

  bool ShouldManuallyDrawCompositionText() override;

  void EnableIME() override;
  void DisableIME() override;

  void CompleteComposition() override;
  void CancelComposition() override;
  CompositionText GetCompositionText() override;

  // Set the candidate window left-top. Relative to window left-top. Use this
  // method to prepare typing.
  void SetCandidateWindowPosition(const Point& point) override;

  IEvent<std::nullptr_t>* CompositionStartEvent() override;
  IEvent<std::nullptr_t>* CompositionEndEvent() override;
  IEvent<std::nullptr_t>* CompositionEvent() override;
  IEvent<const std::string&>* TextEvent() override;

 private:
  void CreateIc(xcb_window_t window);
  void SetFocus();
  void DestroyIc();

 private:
  XcbXimInputMethodManager* manager_;
  XcbWindow* window_;
  bool enabled_;
  std::optional<xcb_xic_t> ic_;
  CompositionText composition_text_;

  Event<std::nullptr_t> composition_start_event_;
  Event<std::nullptr_t> composition_end_event_;
  Event<std::nullptr_t> composition_event_;
  Event<const std::string&> text_event_;
};
}  // namespace cru::platform::gui::xcb
