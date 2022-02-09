#pragma once
#include "LayoutControl.h"

#include "cru/ui/render/FlexLayoutRenderObject.h"

namespace cru::ui::controls {

using render::FlexChildLayoutData;
using render::FlexCrossAlignment;
using render::FlexDirection;
using render::FlexMainAlignment;

class CRU_UI_API FlexLayout
    : public LayoutControl<render::FlexLayoutRenderObject> {
 public:
  static constexpr StringView kControlType = u"FlexLayout";

  static FlexLayout* Create() { return new FlexLayout(); }

 protected:
  FlexLayout();

 public:
  FlexLayout(const FlexLayout& other) = delete;
  FlexLayout(FlexLayout&& other) = delete;
  FlexLayout& operator=(const FlexLayout& other) = delete;
  FlexLayout& operator=(FlexLayout&& other) = delete;
  ~FlexLayout() override;

  String GetControlType() const final { return kControlType.ToString(); }

  FlexMainAlignment GetContentMainAlign() const;
  void SetContentMainAlign(FlexMainAlignment value);

  FlexDirection GetFlexDirection() const;
  void SetFlexDirection(FlexDirection direction);

  FlexCrossAlignment GetItemCrossAlign() const;
  void SetItemCrossAlign(FlexCrossAlignment alignment);
};
}  // namespace cru::ui::controls
