#include "Editor.h"

namespace cru::theme_builder::components {

void Editor::RaiseChangeEvent() {
  if (suppress_next_change_event_) {
    suppress_next_change_event_ = false;
  } else {
    change_event_.Raise(nullptr);
  }
}

void Editor::SuppressNextChangeEvent() { suppress_next_change_event_ = true; }

void Editor::ConnectChangeEvent(IEvent<std::nullptr_t>* event) {
  event->AddHandler([this](std::nullptr_t) { RaiseChangeEvent(); });
}

void Editor::ConnectChangeEvent(Editor* editor) {
  ConnectChangeEvent(editor->ChangeEvent());
}

}  // namespace cru::theme_builder::components
