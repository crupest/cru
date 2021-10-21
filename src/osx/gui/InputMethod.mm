#include "cru/osx/gui/InputMethod.hpp"

#import <AppKit/AppKit.h>
#include "InputMethodPrivate.h"
#include "WindowPrivate.h"
#include "cru/common/Logger.hpp"
#include "cru/osx/Convert.hpp"
#include "cru/osx/gui/Window.hpp"

namespace cru::platform::gui::osx {
namespace details {
OsxInputMethodContextPrivate::OsxInputMethodContextPrivate(
    OsxInputMethodContext* input_method_context, OsxWindow* window) {
  input_method_context_ = input_method_context;
  window_ = window;
}

OsxInputMethodContextPrivate::~OsxInputMethodContextPrivate() {}

void OsxInputMethodContextPrivate::RaiseCompositionStartEvent() {
  composition_start_event_.Raise(nullptr);
}
void OsxInputMethodContextPrivate::RaiseCompositionEndEvent() {
  composition_end_event_.Raise(nullptr);
}
void OsxInputMethodContextPrivate::RaiseCompositionEvent() { composition_event_.Raise(nullptr); }

void OsxInputMethodContextPrivate::RaiseTextEvent(StringView text) { text_event_.Raise(text); }

void OsxInputMethodContextPrivate::PerformSel(SEL sel) {
  [window_->p_->GetNSWindow() performSelector:sel];
}

void OsxInputMethodContextPrivate::Activate() {
  auto input_context = [[window_->p_->GetNSWindow() contentView] inputContext];
  Ensures(input_context);
  [input_context activate];
}

void OsxInputMethodContextPrivate::Deactivate() {
  auto input_context = [[window_->p_->GetNSWindow() contentView] inputContext];
  Ensures(input_context);
  [input_context deactivate];
}
}

OsxInputMethodContext::OsxInputMethodContext(OsxWindow* window)
    : OsxGuiResource(window->GetUiApplication()) {
  p_ = std::make_unique<details::OsxInputMethodContextPrivate>(this, window);
}

OsxInputMethodContext::~OsxInputMethodContext() {}

void OsxInputMethodContext::EnableIME() {
  log::Debug(u"Enable IME.");
  p_->Activate();
}

void OsxInputMethodContext::DisableIME() {
  log::Debug(u"Disable IME.");
  p_->Deactivate();
}

bool OsxInputMethodContext::ShouldManuallyDrawCompositionText() { return true; }

void OsxInputMethodContext::CompleteComposition() {
  // TODO: Implement this.
}

void OsxInputMethodContext::CancelComposition() {
  [[NSTextInputContext currentInputContext] discardMarkedText];
}

CompositionText OsxInputMethodContext::GetCompositionText() { return p_->composition_text_; }

void OsxInputMethodContext::SetCandidateWindowPosition(const Point& point) {
  p_->SetCandidateWindowPosition(point);
}

IEvent<std::nullptr_t>* OsxInputMethodContext::CompositionStartEvent() {
  return &p_->composition_start_event_;
}

IEvent<std::nullptr_t>* OsxInputMethodContext::CompositionEndEvent() {
  return &p_->composition_end_event_;
}

IEvent<std::nullptr_t>* OsxInputMethodContext::CompositionEvent() {
  return &p_->composition_event_;
}

IEvent<StringView>* OsxInputMethodContext::TextEvent() { return &p_->text_event_; }
}
