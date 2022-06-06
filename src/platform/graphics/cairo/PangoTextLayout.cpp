#include "cru/platform/graphics/cairo/PangoTextLayout.h"
#include "cru/platform/Check.h"
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/graphics/cairo/PangoFont.h"

namespace cru::platform::graphics::cairo {
PangoTextLayout::PangoTextLayout(CairoGraphicsFactory* factory,
                                 std::shared_ptr<IFont> font)
    : CairoResource(factory) {
  Expects(font);
  font_ = CheckPlatform<PangoFont>(font, GetPlatformId());
  pango_layout_ = pango_layout_new(factory->GetDefaultPangoContext());
  pango_layout_set_font_description(pango_layout_,
                                    font_->GetPangoFontDescription());
};

PangoTextLayout::~PangoTextLayout() { g_object_unref(pango_layout_); }

String PangoTextLayout::GetText() { return text_; }

void PangoTextLayout::SetText(String new_text) {
  text_ = std::move(new_text);
  auto text_utf8 = text_.ToUtf8();
  pango_layout_set_text(pango_layout_, text_utf8.c_str(), text_utf8.size());
}

std::shared_ptr<IFont> PangoTextLayout::GetFont() { return font_; }

void PangoTextLayout::SetFont(std::shared_ptr<IFont> font) {
  Expects(font);
  font_ = CheckPlatform<PangoFont>(font, GetPlatformId());
  pango_layout_set_font_description(pango_layout_,
                                    font_->GetPangoFontDescription());
}
}  // namespace cru::platform::graphics::cairo
