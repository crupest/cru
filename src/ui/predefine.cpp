#include "predefine.hpp"

#include "border_property.hpp"
#include "graph/graph.hpp"
#include "exception.hpp"

namespace cru::ui::predefine
{
    Microsoft::WRL::ComPtr<ID2D1Brush> CreateSolidBrush(const D2D1_COLOR_F& color)
    {
        const auto device_context = graph::GraphManager::GetInstance()->GetD2D1DeviceContext();
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> solid_color_brush;
        device_context->CreateSolidColorBrush(color, &solid_color_brush);
        return solid_color_brush;
    }

    void InitThemes(AnyMap* resource_map)
    {
        resource_map->SetValue(key_border_property_brush, CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black)));

        BorderProperty button_normal_border{CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue))};
        button_normal_border.SetStrokeWidth(2);
        button_normal_border.SetRadiusX(6);
        button_normal_border.SetRadiusY(6);
        resource_map->SetValue(key_button_normal_border, std::move(button_normal_border));

        BorderProperty button_press_border{CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Blue))};
        button_press_border.SetStrokeWidth(2);
        button_press_border.SetRadiusX(6);
        button_press_border.SetRadiusY(6);
        resource_map->SetValue(key_button_press_border, std::move(button_press_border));


        const auto dwrite_factory = graph::GraphManager::GetInstance()->GetDWriteFactory();
        Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format;

        ThrowIfFailed(dwrite_factory->CreateTextFormat(
            L"等线", nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            24.0, L"zh-cn",
            &text_format
        ));

        ThrowIfFailed(text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
        ThrowIfFailed(text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));

        resource_map->SetValue(key_text_block_text_format, text_format);
        resource_map->SetValue(key_text_box_text_format, text_format);

        auto text_brush = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black));
        resource_map->SetValue(key_text_block_text_brush, text_brush);
        resource_map->SetValue(key_text_box_text_brush, text_brush);

        auto text_selection_brush = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
        resource_map->SetValue(key_text_control_selection_brush, text_selection_brush);

        BorderProperty text_box_border;
        resource_map->SetValue(key_text_box_border, text_box_border);

        resource_map->SetValue(key_text_box_caret_brush, CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black)));

        resource_map->SetValue(key_toggle_button_on_brush, CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::DeepSkyBlue)));
        resource_map->SetValue(key_toggle_button_off_brush, CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::LightGray)));

#ifdef CRU_DEBUG_LAYOUT
        resource_map->SetValue(key_debug_layout_out_border_brush, CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Crimson)));
        resource_map->SetValue(key_debug_layout_margin_brush, CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::LightCoral, 0.25f)));
        resource_map->SetValue(key_debug_layout_padding_brush, CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue, 0.25f)));
#endif
    }
}
