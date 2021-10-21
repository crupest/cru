#pragma once
#include "Resource.hpp"

#include "cru/platform/gui/InputMethod.hpp"

namespace cru::platform::gui::osx {
class OsxWindow;

namespace details {
class OsxInputMethodContextPrivate;
}

class OsxInputMethodContext : public OsxGuiResource,
                              public virtual IInputMethodContext {
  friend OsxWindow;
  friend details::OsxInputMethodContextPrivate;

 public:
  explicit OsxInputMethodContext(OsxWindow* window);

  CRU_DELETE_COPY(OsxInputMethodContext)
  CRU_DELETE_MOVE(OsxInputMethodContext)

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

  IEvent<StringView>* TextEvent() override;

 private:
  std::unique_ptr<details::OsxInputMethodContextPrivate> p_;
};
}  // namespace cru::platform::gui::osx
