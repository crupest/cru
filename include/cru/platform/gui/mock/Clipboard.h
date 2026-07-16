#pragma once

#include "Base.h"

#include <cru/platform/gui/Clipboard.h>

#include <string>

namespace cru::platform::gui::mock {
class CRU_PLATFORM_GUI_MOCK_API MockClipboard : public MockResource,
                                                public virtual IClipboard {
 public:
  std::string GetText() override;
  void SetText(std::string text) override;

  void Clear();

 private:
  std::string text_;
};
}  // namespace cru::platform::gui::mock
