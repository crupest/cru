#pragma once
#include "pre.hpp"

#include <d2d1.h>
#include <wrl/client.h>

#include "base.hpp"

namespace cru::graph {
class GraphManager;
}

namespace cru::ui {
struct CaretInfo {
  std::chrono::milliseconds caret_blink_duration;
  float half_caret_width;
};

class PredefineResources : public Object {
 public:
  explicit PredefineResources(graph::GraphManager* graph_manager);
  PredefineResources(const PredefineResources& other) = delete;
  PredefineResources(PredefineResources&& other) = delete;
  PredefineResources& operator=(const PredefineResources& other) = delete;
  PredefineResources& operator=(PredefineResources&& other) = delete;
  ~PredefineResources() override = default;

  // region TextBlock
  Microsoft::WRL::ComPtr<ID2D1Brush> text_block_selection_brush;
  Microsoft::WRL::ComPtr<ID2D1Brush> text_block_text_brush;
  Microsoft::WRL::ComPtr<IDWriteTextFormat> text_block_text_format;

  // region debug
  Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_out_border_brush;
  Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_margin_brush;
  Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_padding_brush;
};

class UiManager : public Object {
 public:
  static UiManager* GetInstance();

 private:
  UiManager();

 public:
  UiManager(const UiManager& other) = delete;
  UiManager(UiManager&& other) = delete;
  UiManager& operator=(const UiManager& other) = delete;
  UiManager& operator=(UiManager&& other) = delete;
  ~UiManager() override = default;

  CaretInfo GetCaretInfo() const { return caret_info_; }

  const PredefineResources* GetPredefineResources() const {
    return &predefine_resources_;
  }

 private:
  CaretInfo caret_info_;

  PredefineResources predefine_resources_;
};
}  // namespace cru::ui
