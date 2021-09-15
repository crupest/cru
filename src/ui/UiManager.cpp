#include "cru/ui/UiManager.hpp"
#include <optional>

#include "Helper.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/graphics/Font.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "cru/ui/render/ScrollBar.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"
#include "cru/ui/style/Condition.hpp"
#include "cru/ui/style/Styler.hpp"

namespace cru::ui {
using namespace cru::platform::graphics;
using namespace cru::ui::style;
using namespace cru::ui::helper;

namespace {
std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush(IGraphicsFactory* factory,
                                                        const Color& color) {
  auto brush = factory->CreateSolidColorBrush();
  brush->SetColor(color);
  return brush;
}
}  // namespace

UiManager* UiManager::GetInstance() {
  static UiManager* instance = new UiManager();
  GetUiApplication()->AddOnQuitHandler([] {
    delete instance;
    instance = nullptr;
  });
  return instance;
}

UiManager::UiManager() {
  const auto factory = GetGraphFactory();

  theme_resource_.default_font_family = u"等线";

  theme_resource_.default_font =
      factory->CreateFont(theme_resource_.default_font_family, 24.0f);

  const auto black_brush =
      std::shared_ptr<platform::graphics::ISolidColorBrush>(
          CreateSolidColorBrush(factory, colors::black));
  theme_resource_.text_brush = black_brush;
  theme_resource_.text_selection_brush =
      CreateSolidColorBrush(factory, colors::skyblue);
  theme_resource_.caret_brush = black_brush;

  theme_resource_.button_style.AddStyleRule(
      {NoCondition::Create(),
       BorderStyler::Create(ApplyBorderStyleInfo{std::nullopt, Thickness(3),
                                                 CornerRadius(5), std::nullopt,
                                                 std::nullopt}),
       u"DefaultButton"});
  theme_resource_.button_style.AddStyleRule(
      {ClickStateCondition::Create(ClickState::None),
       CompoundStyler::Create(
           BorderStyler::Create(ApplyBorderStyleInfo{
               CreateSolidColorBrush(factory, Color::FromHex(0x00bfff))}),
           CursorStyler::Create(platform::gui::SystemCursorType::Arrow)),
       u"DefaultButtonNormal"});
  theme_resource_.button_style.AddStyleRule(
      {ClickStateCondition::Create(ClickState::Hover),
       CompoundStyler::Create(
           BorderStyler::Create(ApplyBorderStyleInfo{
               CreateSolidColorBrush(factory, Color::FromHex(0x47d1ff))}),
           CursorStyler::Create(platform::gui::SystemCursorType::Hand)),
       u"DefaultButtonHover"});
  theme_resource_.button_style.AddStyleRule(
      {ClickStateCondition::Create(ClickState::Press),
       CompoundStyler::Create(
           BorderStyler::Create(ApplyBorderStyleInfo{
               CreateSolidColorBrush(factory, Color::FromHex(0x91e4ff))}),
           CursorStyler::Create(platform::gui::SystemCursorType::Hand)),
       u"DefaultButtonPress"});
  theme_resource_.button_style.AddStyleRule(
      {ClickStateCondition::Create(ClickState::PressInactive),
       CompoundStyler::Create(
           BorderStyler::Create(ApplyBorderStyleInfo{
               CreateSolidColorBrush(factory, Color::FromHex(0x91e4ff))}),
           CursorStyler::Create(platform::gui::SystemCursorType::Arrow)),
       u"DefaultButtonPressInactive"});

  theme_resource_.text_box_style.AddStyleRule(
      {NoCondition::Create(),
       BorderStyler::Create(
           ApplyBorderStyleInfo{std::nullopt, Thickness{1}, CornerRadius{5}}),
       u"DefaultTextBox"});
  theme_resource_.text_box_style.AddStyleRule(
      {HoverCondition::Create(false),
       BorderStyler::Create(ApplyBorderStyleInfo{
           CreateSolidColorBrush(factory, Color::FromHex(0xced4da))}),
       u"DefaultTextBoxNormal"});
  theme_resource_.text_box_style.AddStyleRule(
      {HoverCondition::Create(true),
       BorderStyler::Create(ApplyBorderStyleInfo{
           CreateSolidColorBrush(factory, Color::FromHex(0xced4da))}),
       u"DefaultTextBoxHover"});
  theme_resource_.text_box_style.AddStyleRule(
      {FocusCondition::Create(true),
       BorderStyler::Create(ApplyBorderStyleInfo{
           CreateSolidColorBrush(factory, Color::FromHex(0x495057))}),
       u"DefaultTextBoxFocus"});

  theme_resource_.menu_item_style.AddStyleRule(
      {NoCondition::Create(),
       BorderStyler::Create(
           ApplyBorderStyleInfo{std::nullopt, Thickness{0}, CornerRadius{0}}),
       u"DefaultMenuItem"});
}

UiManager::~UiManager() = default;
}  // namespace cru::ui
