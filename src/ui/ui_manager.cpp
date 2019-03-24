#include "ui_manager.hpp"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include "application.hpp"
#include "exception.hpp"
#include "graph/graph_manager.hpp"
#include "graph/graph_util.hpp"
#include "util/com_util.hpp"

namespace cru::ui {
namespace {
void GetSystemCaretInfo(CaretInfo* caret_info) {
  caret_info->caret_blink_duration =
      std::chrono::milliseconds(::GetCaretBlinkTime());
  DWORD caret_width;
  if (!::SystemParametersInfoW(SPI_GETCARETWIDTH, 0, &caret_width, 0))
    throw Win32Error(::GetLastError(), "Failed to get system caret width.");
  caret_info->half_caret_width = caret_width / 2.0f;
}

IDWriteTextFormat* CreateDefaultTextFormat() {
  const auto dwrite_factory =
      graph::GraphManager::GetInstance()->GetDWriteFactory();
  IDWriteTextFormat* text_format;

  ThrowIfFailed(dwrite_factory->CreateTextFormat(
      L"等线", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL, 24.0, L"zh-cn", &text_format));

  ThrowIfFailed(text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
  ThrowIfFailed(
      text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));

  return text_format;
}
}  // namespace

PredefineResources::PredefineResources() {
  try {
    text_block_selection_brush =
        graph::CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
    text_block_text_brush =
        graph::CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black));
    text_block_text_format = CreateDefaultTextFormat();
  } catch (...) {
    util::SafeRelease(text_block_selection_brush);
    util::SafeRelease(text_block_text_brush);
    util::SafeRelease(text_block_text_format);
  }
}

PredefineResources::~PredefineResources() {
  util::SafeRelease(text_block_selection_brush);
  util::SafeRelease(text_block_text_brush);
  util::SafeRelease(text_block_text_format);
}

UiManager* UiManager::GetInstance() {
  return Application::GetInstance()->ResolveSingleton<UiManager>(
      [](auto) { return new UiManager{}; });
}

UiManager::UiManager() : predefine_resources_() {
  GetSystemCaretInfo(&caret_info_);
}
}  // namespace cru::ui
