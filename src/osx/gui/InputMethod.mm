#include "cru/osx/gui/InputMethod.hpp"

#import <AppKit/AppKit.h>
#include "InputMethodPrivate.h"
#include "WindowPrivate.h"
#include "cru/osx/Convert.hpp"
#include "cru/osx/gui/Window.hpp"

using cru::platform::osx::Convert;

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

}

OsxInputMethodContext::OsxInputMethodContext(OsxWindow* window)
    : OsxGuiResource(window->GetUiApplication()) {
  p_ = std::make_unique<details::OsxInputMethodContextPrivate>(this, window);
}

OsxInputMethodContext::~OsxInputMethodContext() {}

void OsxInputMethodContext::EnableIME() { [[NSTextInputContext currentInputContext] activate]; }

void OsxInputMethodContext::DisableIME() { [[NSTextInputContext currentInputContext] deactivate]; }

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

@implementation CruInputClient {
  cru::platform::gui::osx::details::OsxInputMethodContextPrivate* _p;
  NSMutableAttributedString* _text;
}

- (id)init:(cru::platform::gui::osx::details::OsxInputMethodContextPrivate*)p {
  _p = p;
  return self;
}

- (BOOL)hasMarkedText {
  return _text != nil;
}

- (NSRange)markedRange {
  return _text == nil ? NSRange{NSNotFound, 0} : NSRange{0, [_text length]};
}

- (NSRange)selectedRange {
  return NSMakeRange(_p->GetSelectionRange().position, _p->GetSelectionRange().count);
}

- (void)setMarkedText:(id)string
        selectedRange:(NSRange)selectedRange
     replacementRange:(NSRange)replacementRange {
  if (_text == nil) {
    _text = [[NSMutableAttributedString alloc] init];
    _p->RaiseCompositionStartEvent();
  }

  [_text deleteCharactersInRange:replacementRange];
  [_text insertAttributedString:[[NSAttributedString alloc] initWithString:(NSString*)string]
                        atIndex:replacementRange.location];

  cru::platform::gui::CompositionText composition_text;
  composition_text.text = Convert((CFStringRef)[_text string]);
  composition_text.selection.position = replacementRange.location + selectedRange.location;
  composition_text.selection.count = selectedRange.length;
  _p->SetCompositionText(composition_text);
  _p->RaiseCompositionEvent();
}

- (void)unmarkText {
  _text = nil;
  _p->RaiseCompositionEndEvent();
}

- (NSArray<NSAttributedStringKey>*)validAttributesForMarkedText {
  return @[
    (NSString*)kCTUnderlineColorAttributeName, (NSString*)kCTUnderlineStyleAttributeName,
    (NSString*)kCTForegroundColorAttributeName, (NSString*)kCTBackgroundColorAttributeName
  ];
}

- (NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range
                                               actualRange:(NSRangePointer)actualRange {
  return [_text attributedSubstringFromRange:range];
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {
  _text = nil;
  cru::String s = Convert((CFStringRef)string);
  _p->RaiseCompositionEndEvent();
  _p->RaiseTextEvent(s);
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point {
  return NSNotFound;
}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(NSRangePointer)actualRange {
  NSRect result;
  result.origin.x = _p->GetCandidateWindowPosition().x;
  result.origin.y = _p->GetCandidateWindowPosition().y;
  result.size.height = 16;
  result.size.width = 0;
  return result;
}

- (void)doCommandBySelector:(SEL)selector {
  _p->PerformSel(selector);
}
@end
