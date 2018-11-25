#pragma once

#include "system_headers.hpp"

#include "base.hpp"
#include "border_property.hpp"

namespace cru::graph
{
    class GraphManager;
}

namespace cru::ui
{
    struct CaretInfo
    {
        std::chrono::milliseconds caret_blink_duration;
        float half_caret_width;
    };


    class PredefineResources : public Object
    {
    public:
        explicit PredefineResources(graph::GraphManager* graph_manager);
        PredefineResources(const PredefineResources& other) = delete;
        PredefineResources(PredefineResources&& other) = delete;
        PredefineResources& operator=(const PredefineResources& other) = delete;
        PredefineResources& operator=(PredefineResources&& other) = delete;
        ~PredefineResources() override = default;

        //region BorderProperty
        Microsoft::WRL::ComPtr<ID2D1Brush> border_property_brush;

        //region Button
        BorderProperty button_normal_border;
        BorderProperty button_press_border;

        //region TextControl
        Microsoft::WRL::ComPtr<ID2D1Brush> text_control_selection_brush;

        //region TextBox
        BorderProperty text_box_border;
        Microsoft::WRL::ComPtr<ID2D1Brush> text_box_text_brush;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> text_box_text_format;
        Microsoft::WRL::ComPtr<ID2D1Brush> text_box_caret_brush;

        //region TextBlock
        Microsoft::WRL::ComPtr<ID2D1Brush> text_block_text_brush;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> text_block_text_format;

        //region ToggleButton
        Microsoft::WRL::ComPtr<ID2D1Brush> toggle_button_on_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> toggle_button_off_brush;

        //region ListItem
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_normal_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_normal_fill_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_hover_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_hover_fill_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_select_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_select_fill_brush;

        //region ScrollControl
        Microsoft::WRL::ComPtr<ID2D1Brush> scroll_bar_background_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> scroll_bar_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> scroll_bar_brush;

#ifdef CRU_DEBUG_LAYOUT
        //region debug
        Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_out_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_margin_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_padding_brush;
#endif
    };

    class UiManager : public Object
    {
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

        CaretInfo GetCaretInfo() const
        {
            return caret_info_;
        }

        const PredefineResources* GetPredefineResources() const
        {
            return &predefine_resources_;
        }

    private:
        CaretInfo caret_info_;

        PredefineResources predefine_resources_;
    };
}
