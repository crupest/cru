#include "cru/platform/gui/xcb/InputMethod.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/gui/xcb/Input.h"
#include "cru/platform/gui/xcb/UiApplication.h"
#include "cru/platform/gui/xcb/Window.h"

#include <xcb-imdkit/encoding.h>
#include <xcb-imdkit/imclient.h>
#include <xcb-imdkit/ximproto.h>

namespace cru::platform::gui::xcb {
namespace {
void XimLogger(const char *, ...) {}
}  // namespace

XcbXimInputMethodManager::XcbXimInputMethodManager(
    XcbUiApplication *application)
    : application_(application), focus_context_(nullptr) {
  auto XimOpenCallback = [](xcb_xim_t *im, void *user_data) {};

  xcb_xim_im_callback kXimCallbacks = {
      .forward_event =
          [](xcb_xim_t* im, xcb_xic_t ic, xcb_key_press_event_t* event,
             void* user_data) {
            CruLogDebug(kLogTag, "Key event forwarded back from XIM.");
            auto manager = static_cast<XcbXimInputMethodManager *>(user_data);
            if ((event->response_type & ~0x80) == XCB_KEY_PRESS) {
              auto text =
                  manager->application_->GetXcbKeyboardManager()->KeycodeToUtf8(
                      event->detail);
              auto modifiers = ConvertModifiersOfEvent(event->state);
              if (text.empty() || text == "\b" ||
                  modifiers.Has(KeyModifiers::Alt) ||
                  modifiers.Has(KeyModifiers::Ctrl)) {
                if (manager->forward_event_callback_) {
                  manager->forward_event_callback_(event);
                }
              } else {
                manager->DispatchCommit(im, ic, std::move(text));
              }
            } else {
              if (manager->forward_event_callback_) {
                manager->forward_event_callback_(event);
              }
            }
          },
      .commit_string =
          [](xcb_xim_t* im, xcb_xic_t ic, uint32_t flag, char* str,
             uint32_t length, uint32_t* keysym, size_t nKeySym,
             void* user_data) {
            auto manager = static_cast<XcbXimInputMethodManager *>(user_data);

            if (flag & XCB_XIM_LOOKUP_KEYSYM) {
              std::string text;
              for (int i = 0; i < nKeySym; i++) {
                text += manager->application_->GetXcbKeyboardManager()
                            ->KeysymToUtf8(keysym[i]);
              }
              manager->DispatchCommit(im, ic, std::move(text));
            }

            if (flag & XCB_XIM_LOOKUP_CHARS) {
              if (xcb_xim_get_encoding(im) == XCB_XIM_UTF8_STRING) {
                manager->DispatchCommit(im, ic, std::string(str, length));
              } else if (xcb_xim_get_encoding(im) == XCB_XIM_COMPOUND_TEXT) {
                size_t newLength = 0;
                char *utf8 = xcb_compound_text_to_utf8(str, length, &newLength);
                if (utf8) {
                  manager->DispatchCommit(im, ic, std::string(utf8, newLength));
                }
              }
            }
          }};

  xcb_compound_text_init();
  im_ = xcb_xim_create(application->GetXcbConnection(), 0, "@im=fcitx");
  xcb_xim_set_im_callback(im_, &kXimCallbacks, static_cast<void *>(this));
  xcb_xim_set_log_handler(im_, XimLogger);
  xcb_xim_set_use_compound_text(im_, true);
  xcb_xim_set_use_utf8_string(im_, true);
  xcb_xim_open(im_, XimOpenCallback, true, this);
  application->XcbFlush();
}

XcbXimInputMethodManager::~XcbXimInputMethodManager() {
  xcb_xim_close(im_);
  xcb_xim_destroy(im_);
}

xcb_xim_t *XcbXimInputMethodManager::GetXcbXim() { return im_; }

void XcbXimInputMethodManager::DispatchCommit(xcb_xim_t *im, xcb_xic_t ic,
                                              std::string text) {
  CruLogDebug(kLogTag, "IC {} dispatch commit string: {}", ic, text);
  if (focus_context_) {
    focus_context_->composition_event_.Raise(nullptr);
    focus_context_->composition_end_event_.Raise(nullptr);
    focus_context_->text_event_.Raise(std::move(text));
  }
}

bool XcbXimInputMethodManager::HandleXEvent(xcb_generic_event_t *event) {
  if (xcb_xim_filter_event(im_, event)) return true;
  if (focus_context_ && focus_context_->ic_ &&
      (((event->response_type & ~0x80) == XCB_KEY_PRESS) ||
       ((event->response_type & ~0x80) == XCB_KEY_RELEASE))) {
    CruLogDebug(kLogTag, "Forward key event to XIM.");
    xcb_xim_forward_event(im_, *focus_context_->ic_,
                          (xcb_key_press_event_t *)event);
    application_->XcbFlush();
    return true;
  }
  return false;
}

void XcbXimInputMethodManager::SetXimServerUnprocessedXEventCallback(
    std::function<void(xcb_key_press_event_t *event)> callback) {
  forward_event_callback_ = std::move(callback);
}

XcbXimInputMethodContext::XcbXimInputMethodContext(
    XcbXimInputMethodManager *manager, XcbWindow *window)
    : manager_(manager), window_(window), enabled_(false) {
  window->CreateEvent()->AddSpyOnlyHandler([this, window] {
    if (enabled_) {
      CreateIc(*window->GetXcbWindow());
    }
  });

  window->DestroyEvent()->AddSpyOnlyHandler([this] { DestroyIc(); });

  window->FocusEvent()->AddHandler([this, window](FocusChangeType type) {
    auto context = CheckPlatform<XcbXimInputMethodContext>(
        window->GetInputMethodContext(), GetPlatformId());
    if (type == FocusChangeType::Gain) {
      SetFocus();
    }
  });
}

XcbXimInputMethodContext::~XcbXimInputMethodContext() {}

bool XcbXimInputMethodContext::ShouldManuallyDrawCompositionText() {
  return true;
}

void XcbXimInputMethodContext::EnableIME() {
  if (enabled_) return;
  enabled_ = true;
  if (!ic_ && window_->GetXcbWindow()) {
    CreateIc(*window_->GetXcbWindow());
  }
}

void XcbXimInputMethodContext::DisableIME() {
  if (!enabled_) return;
  enabled_ = false;
  DestroyIc();
}

void XcbXimInputMethodContext::CompleteComposition() { CancelComposition(); }

void XcbXimInputMethodContext::CancelComposition() {
  if (!ic_) return;
  auto XimResetIcCallback = [](xcb_xim_t *im, xcb_xic_t ic,
                               xcb_im_reset_ic_reply_fr_t *reply,
                               void *user_data) {};
  xcb_xim_reset_ic(manager_->GetXcbXim(), *ic_, XimResetIcCallback, this);
  manager_->application_->XcbFlush();
}

static void EmptyXimDestroyIcCallback(xcb_xim_t *im, xcb_xic_t ic,
                                      void *user_data) {}

void XcbXimInputMethodContext::SetCandidateWindowPosition(const Point &point) {
  if (!ic_) return;

  CruLogDebug(kLogTag, "IC {} set candidate window position: {}", *ic_, point);

  xcb_point_t spot;
  spot.x = point.x;
  spot.y = point.y;
  xcb_xim_nested_list nested = xcb_xim_create_nested_list(
      manager_->GetXcbXim(), XCB_XIM_XNSpotLocation, &spot, NULL);
  xcb_xim_set_ic_values(manager_->GetXcbXim(), *ic_, EmptyXimDestroyIcCallback,
                        this, XCB_XIM_XNPreeditAttributes, &nested, NULL);
  ::free(nested.data);
  manager_->application_->XcbFlush();
}

CompositionText XcbXimInputMethodContext::GetCompositionText() {
  return composition_text_;
}

IEvent<std::nullptr_t> *XcbXimInputMethodContext::CompositionStartEvent() {
  return &composition_start_event_;
}

IEvent<std::nullptr_t> *XcbXimInputMethodContext::CompositionEndEvent() {
  return &composition_end_event_;
}

IEvent<std::nullptr_t> *XcbXimInputMethodContext::CompositionEvent() {
  return &composition_event_;
}

IEvent<const std::string&> *XcbXimInputMethodContext::TextEvent() {
  return &text_event_;
}

void XcbXimInputMethodContext::CreateIc(xcb_window_t window) {
  auto XimCreateIcCallback = [](xcb_xim_t *im, xcb_xic_t ic, void *user_data) {
    auto context = static_cast<XcbXimInputMethodContext *>(user_data);
    context->ic_ = ic;
    CruLogDebug(kLogTag, "XIM IC {} is created.", ic);
    if (context->window_->HasFocus()) {
      context->SetFocus();
    }
  };

  uint32_t input_style =
      XCB_IM_PreeditArea | XCB_IM_PreeditPosition | XCB_IM_StatusNothing;
  xcb_xim_create_ic(manager_->GetXcbXim(), XimCreateIcCallback, this,
                    XCB_XIM_XNInputStyle, &input_style, XCB_XIM_XNClientWindow,
                    &window, XCB_XIM_XNFocusWindow, &window, NULL);
  CruLogDebug(kLogTag, "Create XIM IC.");
  manager_->application_->XcbFlush();
}

void XcbXimInputMethodContext::SetFocus() {
  manager_->focus_context_ = this;
  if (ic_) {
    CruLogDebug(kLogTag, "Focus XIM IC {}.", *ic_);
    xcb_xim_set_ic_focus(manager_->GetXcbXim(), *ic_);
    manager_->application_->XcbFlush();
  }
}

void XcbXimInputMethodContext::DestroyIc() {
  auto destroy_callback = [](xcb_xim_t* im, xcb_xic_t ic, void* user_data) {
    CruLogDebug(kLogTag, "XIM IC {} is destroyed.", ic);
  };

  if (!ic_) return;
  xcb_xim_destroy_ic(manager_->GetXcbXim(), *ic_, destroy_callback, this);
  CruLogDebug(kLogTag, "Destroy XIM IC {}.", *ic_);
  ic_ = std::nullopt;
  manager_->application_->XcbFlush();
}

}  // namespace cru::platform::gui::xcb
