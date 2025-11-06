#pragma once
#include "Base.h"

#include <cru/platform/gui/InputMethod.h>

namespace cru::platform::gui::osx {
class OsxWindow;

namespace details {
class OsxWindowPrivate;
class OsxInputMethodContextPrivate;
}  // namespace details

class OsxInputMethodContext : public OsxGuiResource,
                              public virtual IInputMethodContext {
  friend OsxWindow;
  friend details::OsxWindowPrivate;
  friend details::OsxInputMethodContextPrivate;

 public:
  explicit OsxInputMethodContext(OsxWindow* window);
  ~OsxInputMethodContext() override;

 public:
  bool ShouldManuallyDrawCompositionText() override;

  void EnableIME() override;

  void DisableIME() override;

  void CompleteComposition() override;

  void CancelComposition() override;

  CompositionText GetCompositionText() override;

  void SetCandidateWindowPosition(const Point& point) override;

  IEvent<std::nullptr_t>* CompositionStartEvent() override;

  IEvent<std::nullptr_t>* CompositionEndEvent() override;

  IEvent<std::nullptr_t>* CompositionEvent() override;

  IEvent<const std::string&>* TextEvent() override;

  bool IsEnabled();

 private:
  std::unique_ptr<details::OsxInputMethodContextPrivate> p_;
};
}  // namespace cru::platform::gui::osx
